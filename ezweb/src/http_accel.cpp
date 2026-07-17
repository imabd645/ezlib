#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <sstream>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <chrono>
#include <sys/stat.h>
#include <zlib.h>   // statically linked (libz.a): no new runtime DLL dependency

#pragma comment(lib, "ws2_32.lib")

// Per-request connection state. This used to be a bare map<int, SOCKET>; the
// extra fields are what make keep-alive and gzip possible -- both need to know
// something the request said (whether it accepts gzip, whether it wants the
// connection kept) at the point where the response goes out.
struct Conn {
    SOCKET sock = INVALID_SOCKET;
    bool accepts_gzip = false;   // client sent Accept-Encoding: gzip
    bool keep_alive = false;     // negotiated from HTTP version + Connection header
    bool responded = false;      // set by http_server_respond/send_file
    bool close_after = true;     // decided when the response is sent
};

static std::map<int, Conn> g_active_sockets;
static std::mutex g_sockets_mutex;
static std::atomic<int> g_next_id{1};

static bool g_server_running = false;
static SOCKET g_listen_socket = INVALID_SOCKET;
static std::atomic<int> g_active_handlers{0};

// ── Tunables ────────────────────────────────────────────────────────────────
// A connection that never finishes sending its headers used to hold a thread
// and a socket forever (a slowloris just opens sockets and dribbles bytes).
// These give every blocking socket op a deadline.
static const int RECV_TIMEOUT_MS = 15000;  // per recv() while reading a request
static const int SEND_TIMEOUT_MS = 15000;  // per send() while writing a response
static const int IDLE_TIMEOUT_MS = 5000;   // waiting for the NEXT keep-alive request

// A per-recv() timeout alone does NOT stop a slowloris: it only bounds the gap
// between two bytes, and every trickled byte resets it. A client sending one
// byte every 5s never trips a 15s recv timeout and never reaches the 64KB header
// cap, so it holds a socket and a thread indefinitely -- measured at 9 bytes
// buying 45+ seconds, which means ~512 near-idle clients can exhaust
// MAX_CONNECTIONS. This is a deadline on the WHOLE request instead: total wall
// time from the first byte of a request line to the last byte of its body.
static const int REQUEST_DEADLINE_MS = 20000;

// Hard ceiling on concurrent connections. Without one, a flood spawns unbounded
// threads (1MB of stack each) until the process dies.
static const int MAX_CONNECTIONS = 512;

// Only compress payloads worth the CPU, and only when the client asked.
static const size_t GZIP_MIN_BYTES = 256;

static std::atomic<int> g_open_connections{0};

// Number of worker processes this instance supervises (0 = not a supervisor).
static std::atomic<int> g_worker_count{0};

// Reject absurd Content-Length values outright. Without a cap, a request that
// merely CLAIMS a huge body makes us grow `body` until the process dies -- a
// one-line denial of service. 32 MB is far above any form post this framework
// is meant to serve.
static const long long MAX_BODY_BYTES = 32LL * 1024 * 1024;

static void* g_ez_callback = nullptr;
// NOTE: this signature is duplicated on the EZ side in main.ez
// (os_ffi_create_callback in WebApp.run). libffi marshals by this exact shape,
// so the two MUST be changed together -- a mismatch corrupts the stack rather
// than failing cleanly.
typedef void (*EZCallback)(int req_id, const char* method, const char* path, const char* query_json, const char* headers_json, const char* body, const char* client_ip);

// ── Prefork supervisor ──────────────────────────────────────────────────────
// Set in each worker to the numeric value of the inherited listening socket.
// Its presence is also how a process knows it is a worker rather than the
// supervisor, so nothing else may set it.
static const char* EZWEB_LISTEN_ENV = "EZWEB_LISTEN_HANDLE";

// Bind a listening socket that CreateProcess can hand to a child.
// Windows has no SO_REUSEPORT, so workers cannot each bind the port themselves;
// instead one socket is bound here and inherited by every worker, and the kernel
// distributes accepts across them (measured: even, 4/4/4 over 12 requests).
static SOCKET bind_inheritable_listener(int port) {
    // WSASocketW, not socket(): we need explicit control of the flags.
    SOCKET s = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
    if (s == INVALID_SOCKET) return INVALID_SOCKET;
    if (!SetHandleInformation((HANDLE)s, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT)) {
        closesocket(s);
        return INVALID_SOCKET;
    }
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    sockaddr_in a{};
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = INADDR_ANY;
    a.sin_port = htons(port);
    if (bind(s, (sockaddr*)&a, sizeof(a)) == SOCKET_ERROR) { closesocket(s); return INVALID_SOCKET; }
    if (listen(s, SOMAXCONN) == SOCKET_ERROR)              { closesocket(s); return INVALID_SOCKET; }
    return s;
}

// Bind the port, launch `workers` copies of this same process to serve it, and
// wait. Returns 2 once every worker has exited; -1 if setup failed.
static int supervise_workers(int port, int workers) {
    SOCKET ls = bind_inheritable_listener(port);
    if (ls == INVALID_SOCKET) return -1;

    char exe[MAX_PATH];
    if (!GetModuleFileNameA(nullptr, exe, MAX_PATH)) { closesocket(ls); return -1; }

    // Set the handle on OURSELVES and pass lpEnvironment = nullptr, so each child
    // inherits the full parent environment plus this one variable. Handing
    // CreateProcess a hand-built block containing only this variable wipes PATH
    // and SystemRoot, and the child then dies in the loader before main() -- with
    // no error anywhere, looking exactly like "handle inheritance is unsupported".
    SetEnvironmentVariableA(EZWEB_LISTEN_ENV, std::to_string((unsigned long long)ls).c_str());

    // Put the workers in a job that kills them when its last handle closes. If
    // the supervisor is Ctrl-C'd or killed, the workers would otherwise survive
    // as orphans still holding the port, and the next run would fail to bind.
    HANDLE job = CreateJobObjectA(nullptr, nullptr);
    if (job) {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION li{};
        li.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        SetInformationJobObject(job, JobObjectExtendedLimitInformation, &li, sizeof(li));
    }

    std::vector<HANDLE> kids;
    for (int i = 0; i < workers; ++i) {
        STARTUPINFOA si{}; si.cb = sizeof(si);
        PROCESS_INFORMATION pi{};
        // bInheritHandles = TRUE is what carries the listening socket across.
        // CREATE_SUSPENDED so the child cannot run (or spawn anything of its own)
        // before it is inside the job.
        if (!CreateProcessA(exe, GetCommandLineA(), nullptr, nullptr, TRUE,
                            CREATE_SUSPENDED, nullptr, nullptr, &si, &pi)) {
            break;
        }
        if (job) AssignProcessToJobObject(job, pi.hProcess);
        ResumeThread(pi.hThread);
        CloseHandle(pi.hThread);
        kids.push_back(pi.hProcess);
    }

    // Do not leave the variable set in the supervisor: anything it spawns later
    // would otherwise mistake itself for a worker.
    SetEnvironmentVariableA(EZWEB_LISTEN_ENV, nullptr);

    if (kids.empty()) { closesocket(ls); if (job) CloseHandle(job); return -1; }

    g_worker_count = (int)kids.size();
    // The supervisor never accepts. Its only jobs are to hold the listening
    // socket open (closing it here would tear the port out from under the
    // workers) and to take the whole group down together.
    WaitForMultipleObjects((DWORD)kids.size(), kids.data(), TRUE, INFINITE);
    for (HANDLE h : kids) CloseHandle(h);
    if (job) CloseHandle(job);
    closesocket(ls);
    return 2;
}

// Safe URL decoder
std::string url_decode(const std::string& str) {
    std::string ret;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%' && i + 2 < str.length()) {
            std::string hex = str.substr(i + 1, 2);
            bool valid = true;
            for(char c : hex) {
                if(!std::isxdigit(c)) { valid = false; break; }
            }
            if (valid) {
                int ii;
                sscanf(hex.c_str(), "%x", &ii);
                ret += static_cast<char>(ii);
                i += 2;
            } else {
                ret += str[i];
            }
        } else if (str[i] == '+') {
            ret += ' ';
        } else {
            ret += str[i];
        }
    }
    return ret;
}

// JSON escaper
std::string escape_json(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else out += c;
    }
    return out;
}

// ── Helpers for response rewriting ──────────────────────────────────────────

static std::string lower_copy(const std::string& s) {
    std::string o = s;
    for (auto& c : o) c = (char)std::tolower((unsigned char)c);
    return o;
}

// Case-insensitive search restricted to `hay[0..limit)`.
static bool contains_ci(const std::string& hay, const std::string& needle, size_t limit) {
    std::string h = lower_copy(hay.substr(0, limit));
    return h.find(lower_copy(needle)) != std::string::npos;
}

// gzip-compress `in`. Returns false if zlib declines (we then send it plain).
static bool gzip_compress(const std::string& in, std::string& out) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));
    // windowBits 15|16 selects a gzip wrapper rather than raw zlib/deflate.
    if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 | 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        return false;
    }
    zs.next_in = (Bytef*)in.data();
    zs.avail_in = (uInt)in.size();

    char buf[32768];
    out.clear();
    int rc;
    do {
        zs.next_out = (Bytef*)buf;
        zs.avail_out = sizeof(buf);
        rc = deflate(&zs, Z_FINISH);
        if (out.size() < zs.total_out) out.append(buf, zs.total_out - out.size());
    } while (rc == Z_OK);
    deflateEnd(&zs);
    return rc == Z_STREAM_END;
}

// Is this Content-Type worth compressing? Images/video/zips are already
// compressed; gzipping them burns CPU and usually grows the payload.
static bool is_compressible(const std::string& headers) {
    std::string h = lower_copy(headers);
    size_t p = h.find("content-type:");
    if (p == std::string::npos) return false;
    size_t e = h.find("\r\n", p);
    std::string ct = h.substr(p, (e == std::string::npos ? h.size() : e) - p);
    if (ct.find("text/") != std::string::npos) return true;
    if (ct.find("application/json") != std::string::npos) return true;
    if (ct.find("application/javascript") != std::string::npos) return true;
    if (ct.find("image/svg+xml") != std::string::npos) return true;
    if (ct.find("application/xml") != std::string::npos) return true;
    return false;
}

// Drop a header line (e.g. an existing Content-Length) from a header block.
static void strip_header(std::string& headers, const std::string& name) {
    std::string lname = lower_copy(name) + ":";
    size_t pos = 0;
    std::string lh = lower_copy(headers);
    while ((pos = lh.find(lname, pos)) != std::string::npos) {
        // Must be at the start of a line.
        if (pos != 0 && !(pos >= 2 && headers.compare(pos - 2, 2, "\r\n") == 0)) { pos += lname.size(); continue; }
        size_t end = headers.find("\r\n", pos);
        if (end == std::string::npos) break;
        headers.erase(pos, end - pos + 2);
        lh = lower_copy(headers);
        pos = 0;
    }
}

static void set_socket_timeouts(SOCKET s, int recv_ms, int send_ms) {
    DWORD r = (DWORD)recv_ms, w = (DWORD)send_ms;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&r, sizeof(r));
    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char*)&w, sizeof(w));
}

static bool send_all(SOCKET s, const char* data, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        int n = send(s, data + sent, (int)(len - sent), 0);
        if (n <= 0) return false;
        sent += (size_t)n;
    }
    return true;
}

// Serve one connection. Loops while the client wants the connection kept open
// (HTTP/1.1 default), so a browser fetching a page plus its assets pays for one
// TCP handshake and one thread instead of one per request. Every response used
// to be hardcoded `Connection: close`.
void client_handler(SOCKET client_socket, std::string client_ip) {
    g_active_handlers++;
    g_open_connections++;
    set_socket_timeouts(client_socket, RECV_TIMEOUT_MS, SEND_TIMEOUT_MS);

    char buffer[16384];
    std::string pending;   // bytes already read that belong to the next request

  next_request:
    {
    std::string request_str = pending;
    pending.clear();
    size_t header_end = request_str.find("\r\n\r\n");

    // Deadline for receiving this ENTIRE request. It starts at the first byte
    // actually received for this request, so the keep-alive idle wait (which has
    // its own, shorter timeout) is not charged against it.
    bool deadline_started = !request_str.empty();
    auto deadline_start = std::chrono::steady_clock::now();
    auto past_deadline = [&]() -> bool {
        if (!deadline_started) return false;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - deadline_start).count();
        return ms > REQUEST_DEADLINE_MS;
    };

    // Read headers loop
    while (header_end == std::string::npos) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            // Timeout or peer closed: done with this connection.
            closesocket(client_socket);
            g_open_connections--;
            g_active_handlers--;
            return;
        }
        if (!deadline_started) {
            // First byte of a real request: start the clock, and swap the idle
            // timeout back out for the (longer) in-request one.
            deadline_started = true;
            deadline_start = std::chrono::steady_clock::now();
            set_socket_timeouts(client_socket, RECV_TIMEOUT_MS, SEND_TIMEOUT_MS);
        }
        request_str.append(buffer, bytes_received);
        header_end = request_str.find("\r\n\r\n");
        if (header_end == std::string::npos && request_str.length() > 65536) { // Max header size
            closesocket(client_socket);
            g_open_connections--;
            g_active_handlers--;
            return;
        }
        // Trickling bytes forever resets the per-recv timeout but not this.
        if (header_end == std::string::npos && past_deadline()) {
            std::string resp = "HTTP/1.1 408 Request Timeout\r\nConnection: close\r\n"
                               "Content-Length: 0\r\n\r\n";
            send(client_socket, resp.c_str(), (int)resp.length(), 0);
            closesocket(client_socket);
            g_open_connections--;
            g_active_handlers--;
            return;
        }
    }

    int req_id = g_next_id++;
    std::string headers_str = request_str.substr(0, header_end);
    std::string body = request_str.substr(header_end + 4);

    std::istringstream stream(headers_str);
    std::string line;
    std::getline(stream, line);
    if (!line.empty() && line.back() == '\r') line.pop_back();

    size_t space1 = line.find(' ');
    size_t space2 = line.rfind(' ');
    if (space1 == std::string::npos || space2 == std::string::npos || space1 == space2) {
        closesocket(client_socket);
        g_open_connections--;
        g_active_handlers--;
        return;
    }

    std::string method = line.substr(0, space1);
    std::string full_path = line.substr(space1 + 1, space2 - space1 - 1);
    std::string version = line.substr(space2 + 1);

    // HTTP/1.1 keeps connections alive unless the client says otherwise;
    // HTTP/1.0 is the reverse.
    bool keep_alive = (version.find("1.1") != std::string::npos);
    bool accepts_gzip = false;

    std::string headers_json = "";
    int content_length = 0;
    while (std::getline(stream, line) && line != "\r" && !line.empty()) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string val = line.substr(colon + 1);
            
            // Trim key and val
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            val.erase(0, val.find_first_not_of(" \t"));
            val.erase(val.find_last_not_of(" \t") + 1);
            
            // Escape quotes in val
            std::string escaped_val = "";
            for (char c : val) {
                if (c == '"') escaped_val += "\\\"";
                else if (c == '\\') escaped_val += "\\\\";
                else escaped_val += c;
            }
            
            // lowercase key
            for (auto &c : key) c = std::tolower(c);
            
            if (!headers_json.empty()) headers_json += ",";
            headers_json += "\"" + key + "\":\"" + escaped_val + "\"";
            
            if (key == "content-length") {
                // std::stoi THROWS on a non-numeric or out-of-range value. This
                // runs on a detached thread with no handler, so
                // `Content-Length: abc` propagated out and called
                // std::terminate() -- killing the entire server. Any client
                // could do it with one request. Parse defensively instead and
                // reject a malformed or oversized length with 400.
                errno = 0;
                char* endp = nullptr;
                long long parsed = std::strtoll(val.c_str(), &endp, 10);
                bool bad = (endp == val.c_str()) || (parsed < 0) || (parsed > MAX_BODY_BYTES);
                if (bad) {
                    std::string resp =
                        "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n"
                        "Content-Length: 0\r\n\r\n";
                    send(client_socket, resp.c_str(), (int)resp.length(), 0);
                    closesocket(client_socket);
                    g_open_connections--;
                    g_active_handlers--;
                    return;
                }
                content_length = (int)parsed;
            }
            // Remember what the client will accept, for the response side.
            else if (key == "connection") {
                std::string lv = lower_copy(val);
                if (lv.find("close") != std::string::npos) keep_alive = false;
                else if (lv.find("keep-alive") != std::string::npos) keep_alive = true;
            }
            else if (key == "accept-encoding") {
                if (lower_copy(val).find("gzip") != std::string::npos) accepts_gzip = true;
            }
        }
    }
    headers_json = "{" + headers_json + "}";

    // Read remaining body
    if (content_length > (int)body.length()) {
        int remaining = content_length - (int)body.length();
        while (remaining > 0) {
            int bytes_received = recv(client_socket, buffer, std::min(remaining, (int)sizeof(buffer)), 0);
            // A timeout or a hangup here used to `break` and fall through with a
            // SHORT body, handing the handler a truncated request that looked
            // complete -- a half-sent form post would be parsed and acted on.
            // An incomplete body is an error, not a smaller request.
            if (bytes_received <= 0 || past_deadline()) {
                std::string resp = "HTTP/1.1 408 Request Timeout\r\nConnection: close\r\n"
                                   "Content-Length: 0\r\n\r\n";
                send(client_socket, resp.c_str(), (int)resp.length(), 0);
                closesocket(client_socket);
                g_open_connections--;
                g_active_handlers--;
                return;
            }
            body.append(buffer, bytes_received);
            remaining -= bytes_received;
        }
    } else if ((int)body.length() > content_length) {
        // We over-read: the extra bytes are the start of the NEXT pipelined
        // request on this keep-alive connection. Hold them for the next loop
        // instead of dropping them (which would corrupt that request).
        pending = body.substr(content_length);
        body.resize(content_length);
    }

    // Parse query params out of fullPath
    std::string path = full_path;
    std::string query_json = "{}";
    size_t qmark = full_path.find('?');
    if (qmark != std::string::npos) {
        path = full_path.substr(0, qmark);
        std::string query_str = full_path.substr(qmark + 1);
        query_json = "{";
        size_t start = 0;
        bool first = true;
        while (start < query_str.length()) {
            size_t end = query_str.find('&', start);
            if (end == std::string::npos) end = query_str.length();
            std::string pair = query_str.substr(start, end - start);
            size_t eq = pair.find('=');
            if (eq != std::string::npos) {
                if (!first) query_json += ",";
                std::string k = url_decode(pair.substr(0, eq));
                std::string v = url_decode(pair.substr(eq + 1));
                query_json += "\"" + escape_json(k) + "\":\"" + escape_json(v) + "\"";
                first = false;
            }
            start = end + 1;
        }
        query_json += "}";
    }

    // Headers already built in headers_json

    {
        std::lock_guard<std::mutex> lock(g_sockets_mutex);
        Conn c;
        c.sock = client_socket;
        c.accepts_gzip = accepts_gzip;
        c.keep_alive = keep_alive;
        c.responded = false;
        c.close_after = !keep_alive;
        g_active_sockets[req_id] = c;
    }

    if (!g_ez_callback) {
        std::string resp = "HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\nContent-Length: 0\r\n\r\n";
        send(client_socket, resp.c_str(), (int)resp.length(), 0);
        std::lock_guard<std::mutex> lock(g_sockets_mutex);
        g_active_sockets.erase(req_id);
        closesocket(client_socket);
        g_open_connections--;
        g_active_handlers--;
        return;
    }

    // Blocks until the EZ handler has run AND answered: the FFI callback
    // marshals onto the interpreter's main thread and waits on a future. That is
    // what makes this loop safe -- once it returns, the response is on the wire.
    ((EZCallback)g_ez_callback)(req_id, method.c_str(), path.c_str(), query_json.c_str(), headers_json.c_str(), body.c_str(), client_ip.c_str());

    // How did the response leave things?
    bool close_now = true;
    bool answered = false;
    {
        std::lock_guard<std::mutex> lock(g_sockets_mutex);
        auto it = g_active_sockets.find(req_id);
        if (it != g_active_sockets.end()) {
            answered = it->second.responded;
            close_now = it->second.close_after;
            g_active_sockets.erase(it);
        }
    }

    if (!answered) {
        // The handler never responded (it threw, or EZ failed to call back).
        // Say something and reclaim the socket rather than leaking it.
        std::string resp = "HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\nContent-Length: 0\r\n\r\n";
        send(client_socket, resp.c_str(), (int)resp.length(), 0);
        close_now = true;
    }

    if (!close_now && g_server_running) {
        // Keep-alive: wait for the next request on this same socket, but only
        // briefly -- an idle peer must not hold a thread indefinitely.
        set_socket_timeouts(client_socket, IDLE_TIMEOUT_MS, SEND_TIMEOUT_MS);
        goto next_request;
    }

    closesocket(client_socket);
    g_open_connections--;
    g_active_handlers--;
    }
}

void server_loop() {
    while (g_server_running) {
        sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        SOCKET client_socket = accept(g_listen_socket, (sockaddr*)&client_addr, &client_len);
        if (client_socket != INVALID_SOCKET) {
            // Bound concurrency. Previously every accepted socket spawned a
            // thread unconditionally, so a connection flood spawned threads
            // until the process died. Shed load instead.
            if (g_open_connections.load() >= MAX_CONNECTIONS) {
                std::string resp =
                    "HTTP/1.1 503 Service Unavailable\r\nConnection: close\r\n"
                    "Retry-After: 1\r\nContent-Length: 0\r\n\r\n";
                send(client_socket, resp.c_str(), (int)resp.length(), 0);
                closesocket(client_socket);
                continue;
            }

            // accept() already gives us the peer address; it used to be filled
            // in here and then thrown away, leaving the EZ side with no way to
            // tell one client from another. That is why useRateLimiter() keyed
            // every request to the constant "client" and rate-limited the whole
            // world as a single bucket.
            char ip_buf[INET_ADDRSTRLEN] = {0};
            const char* ip = inet_ntop(AF_INET, &client_addr.sin_addr, ip_buf, sizeof(ip_buf));
            std::string client_ip = ip ? ip : "";

            std::thread(client_handler, client_socket, client_ip).detach();
        }
    }
}

extern "C" {
    // Start serving on `port`.
    //
    // `workers` > 1 selects the prefork model: EZ handlers are serialized on the
    // interpreter's single main thread (the FFI callback marshals onto its event
    // loop and blocks), so extra THREADS buy nothing -- a slow handler stalls
    // every in-flight request regardless. Extra PROCESSES each get their own VM
    // and their own main thread, which is what actually parallelizes handlers.
    //
    // Returns  1 = serving (worker or single-process)
    //          2 = this call was the supervisor and all workers have now exited
    //         -1 = failure
    __declspec(dllexport) int http_server_start(int port, void* ez_callback, int workers) {
        if (g_server_running) return 1;
        g_ez_callback = ez_callback;

        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return -1;

        // A worker re-runs the same script from the top and arrives here with the
        // listening socket already bound by its parent, handed over as an
        // inherited handle. It must NOT bind again -- it adopts and accepts.
        if (const char* inherited = std::getenv(EZWEB_LISTEN_ENV)) {
            g_listen_socket = (SOCKET)(uintptr_t)_strtoui64(inherited, nullptr, 10);
            if (g_listen_socket == INVALID_SOCKET || g_listen_socket == 0) return -1;
            g_server_running = true;
            std::thread(server_loop).detach();
            return 1;
        }

        if (workers > 1) return supervise_workers(port, workers);

        g_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (g_listen_socket == INVALID_SOCKET) return -1;

        int opt = 1;
        setsockopt(g_listen_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        if (bind(g_listen_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            closesocket(g_listen_socket);
            return -1;
        }

        if (listen(g_listen_socket, SOMAXCONN) == SOCKET_ERROR) {
            closesocket(g_listen_socket);
            return -1;
        }

        g_server_running = true;
        std::thread(server_loop).detach();
        return 1;
    }

    // 1 if this process is a prefork worker (i.e. it was spawned by a supervisor
    // and inherited the listening socket). Lets EZ print the startup banner once,
    // from the supervisor, instead of once per worker.
    __declspec(dllexport) int http_is_worker() {
        return std::getenv(EZWEB_LISTEN_ENV) ? 1 : 0;
    }

    // This process's id. Under `workers`, every request is served by one of
    // several identical processes, so log lines are ambiguous without it.
    __declspec(dllexport) int http_getpid() {
        return (int)GetCurrentProcessId();
    }

    // Send a complete response (status line + headers + body, as built by EZ).
    //
    // The socket is NOT closed here any more: client_handler owns the connection
    // lifetime so it can keep it alive for the next request. This also gzips the
    // body when the client advertised gzip and the payload is worth it -- doing
    // that here means it runs on the connection's own C++ thread, never on the
    // interpreter's single main thread.
    __declspec(dllexport) void http_server_respond(int client_id, const char* response_string, int response_len) {
        SOCKET client_socket = INVALID_SOCKET;
        bool accepts_gzip = false;
        bool keep_alive = false;
        {
            std::lock_guard<std::mutex> lock(g_sockets_mutex);
            auto it = g_active_sockets.find(client_id);
            if (it == g_active_sockets.end()) return;
            client_socket = it->second.sock;
            accepts_gzip = it->second.accepts_gzip;
            keep_alive = it->second.keep_alive;
            it->second.responded = true;
        }
        if (client_socket == INVALID_SOCKET || !response_string || response_len <= 0) return;

        std::string resp(response_string, response_len);
        size_t split = resp.find("\r\n\r\n");

        bool close_after = true;
        if (split != std::string::npos) {
            std::string headers = resp.substr(0, split + 2); // keep trailing CRLF
            std::string body = resp.substr(split + 4);

            // The response's own Connection header is the source of truth: EZ
            // decides, we honour it.
            if (keep_alive && !contains_ci(headers, "connection: close", headers.size())) {
                close_after = false;
            }

            if (accepts_gzip && body.size() >= GZIP_MIN_BYTES && is_compressible(headers)
                && !contains_ci(headers, "content-encoding:", headers.size())) {
                std::string gz;
                if (gzip_compress(body, gz) && gz.size() < body.size()) {
                    strip_header(headers, "Content-Length");
                    headers += "Content-Encoding: gzip\r\n";
                    headers += "Vary: Accept-Encoding\r\n";
                    headers += "Content-Length: " + std::to_string(gz.size()) + "\r\n";
                    body.swap(gz);
                }
            }
            resp = headers + "\r\n" + body;
        }

        send_all(client_socket, resp.data(), resp.size());

        {
            std::lock_guard<std::mutex> lock(g_sockets_mutex);
            auto it = g_active_sockets.find(client_id);
            if (it != g_active_sockets.end()) it->second.close_after = close_after;
        }
        // client_handler closes (or reuses) the socket and owns g_active_handlers.
    }

    // Send `header_str` followed by the raw bytes of `file_path`.
    //
    // Binary files cannot go through http_server_respond(): the EZ side has to
    // hand it a C string, so the payload is truncated at the first NUL byte --
    // and the Content-Length was computed with strlen(), truncating identically.
    // Every .png/.jpg serve_static() advertised was therefore corrupt. Doing the
    // read and the send here keeps the bytes intact and avoids copying whole
    // files through the interpreter.
    //
    // Returns 1 on success, 0 if the file could not be opened, -1 if the request
    // id is unknown.
    __declspec(dllexport) int http_server_send_file(int client_id, const char* header_str, const char* file_path) {
        SOCKET client_socket = INVALID_SOCKET;
        bool keep_alive = false;
        {
            std::lock_guard<std::mutex> lock(g_sockets_mutex);
            auto it = g_active_sockets.find(client_id);
            if (it == g_active_sockets.end()) return -1;
            client_socket = it->second.sock;
            keep_alive = it->second.keep_alive;
            it->second.responded = true;
        }
        if (client_socket == INVALID_SOCKET) return -1;

        bool close_after = true;
        if (keep_alive && header_str && !contains_ci(std::string(header_str), "connection: close", strlen(header_str))) {
            close_after = false;
        }

        FILE* f = nullptr;
        if (file_path) f = fopen(file_path, "rb");
        if (!f) {
            std::string resp =
                "HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Length: 0\r\n\r\n";
            send(client_socket, resp.c_str(), (int)resp.length(), 0);
            std::lock_guard<std::mutex> lock(g_sockets_mutex);
            auto it = g_active_sockets.find(client_id);
            if (it != g_active_sockets.end()) it->second.close_after = true;
            return 0;
        }

        // Headers first (caller supplies the full block including the blank line).
        if (header_str) send_all(client_socket, header_str, strlen(header_str));

        // Then the file, streamed so memory use stays flat regardless of size.
        // Deliberately NOT gzipped: this path exists for binary payloads, which
        // are already compressed (png/jpg/zip/mp4).
        char buf[64 * 1024];
        size_t n;
        while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
            if (!send_all(client_socket, buf, n)) break;
        }
        fclose(f);

        {
            std::lock_guard<std::mutex> lock(g_sockets_mutex);
            auto it = g_active_sockets.find(client_id);
            if (it != g_active_sockets.end()) it->second.close_after = close_after;
        }
        return 1;
    }

    // Last-modified time (unix seconds), or -1. Combined with the size this
    // gives a cheap, strong-enough ETag so unchanged assets can be answered with
    // 304 Not Modified instead of resending the bytes every time.
    __declspec(dllexport) long long http_file_mtime(const char* file_path) {
        if (!file_path) return -1;
        struct _stat64 st;
        if (_stat64(file_path, &st) != 0) return -1;
        return (long long)st.st_mtime;
    }

    // Size of a file in bytes, or -1 if it cannot be opened. Needed to write an
    // accurate Content-Length for binary files, where strlen() is wrong.
    __declspec(dllexport) long long http_file_size(const char* file_path) {
        if (!file_path) return -1;
        FILE* f = fopen(file_path, "rb");
        if (!f) return -1;
        if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return -1; }
        long long sz = _ftelli64(f);
        fclose(f);
        return sz;
    }

    __declspec(dllexport) void http_server_stop() {
        g_server_running = false;
        if (g_listen_socket != INVALID_SOCKET) {
            closesocket(g_listen_socket);
            g_listen_socket = INVALID_SOCKET;
        }
        while (g_active_handlers > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        WSACleanup();
    }
}
