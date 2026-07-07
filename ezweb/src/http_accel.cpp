#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <thread>
#include <mutex>
#include <sstream>
#include <atomic>

#pragma comment(lib, "ws2_32.lib")

struct HttpRequest {
    int id;
    SOCKET socket;
    std::string method;
    std::string path;
    std::string body;
    std::map<std::string, std::string> headers;
};

static std::queue<std::string> g_request_queue;
static std::mutex g_queue_mutex;

static std::map<int, SOCKET> g_active_sockets;
static std::mutex g_sockets_mutex;
static std::atomic<int> g_next_id{1};

static bool g_server_running = false;
static SOCKET g_listen_socket = INVALID_SOCKET;
static std::atomic<int> g_active_handlers{0};

// Internal buffer for poll
static std::string g_poll_buffer;
static std::mutex g_poll_mutex;

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

auto status_text = [](int code) -> std::string {
    if (code == 200) return "OK";
    if (code == 201) return "Created";
    if (code == 301) return "Moved Permanently";
    if (code == 302) return "Found";
    if (code == 400) return "Bad Request";
    if (code == 401) return "Unauthorized";
    if (code == 403) return "Forbidden";
    if (code == 404) return "Not Found";
    if (code == 429) return "Too Many Requests";
    if (code == 500) return "Internal Server Error";
    return "Unknown";
};

void client_handler(SOCKET client_socket, int req_id) {
    g_active_handlers++;

    char buffer[16384];
    std::string request_str;
    size_t header_end = std::string::npos;
    
    // Read headers loop
    while (true) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            closesocket(client_socket);
            g_active_handlers--;
            return;
        }
        request_str.append(buffer, bytes_received);
        header_end = request_str.find("\r\n\r\n");
        if (header_end != std::string::npos) {
            break;
        }
        if (request_str.length() > 65536) { // Max header size
            closesocket(client_socket);
            g_active_handlers--;
            return;
        }
    }

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
        g_active_handlers--;
        return;
    }

    std::string method = line.substr(0, space1);
    std::string full_path = line.substr(space1 + 1, space2 - space1 - 1);
    
    std::map<std::string, std::string> headers;
    int content_length = 0;

    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string val = line.substr(colon + 1);
            val.erase(0, val.find_first_not_of(" \t"));
            
            std::string lower_key = key;
            for(char &c : lower_key) c = std::tolower(c);
            
            headers[lower_key] = val;
            
            if (lower_key == "content-length") {
                content_length = std::stoi(val);
            }
        }
    }

    // Read remaining body
    if (content_length > body.length()) {
        int remaining = content_length - body.length();
        while (remaining > 0) {
            int bytes_received = recv(client_socket, buffer, std::min(remaining, (int)sizeof(buffer)), 0);
            if (bytes_received <= 0) break;
            body.append(buffer, bytes_received);
            remaining -= bytes_received;
        }
    }

    // Queue limit check
    {
        std::lock_guard<std::mutex> lock(g_queue_mutex);
        if (g_request_queue.size() > 1000) {
            std::string resp = "HTTP/1.1 503 Service Unavailable\r\nConnection: close\r\n\r\n";
            send(client_socket, resp.c_str(), resp.length(), 0);
            closesocket(client_socket);
            g_active_handlers--;
            return;
        }
    }

    // Build JSON object
    std::string json = "{";
    json += "\"id\":" + std::to_string(req_id) + ",";
    json += "\"method\":\"" + method + "\",";
    json += "\"fullPath\":\"" + escape_json(full_path) + "\",";
    
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
    json += "\"path\":\"" + escape_json(path) + "\",";
    json += "\"query\":" + query_json + ",";

    // Headers
    json += "\"headers\":{";
    bool first = true;
    std::string cookie_str = "";
    for (const auto& pair : headers) {
        std::string lower_key = pair.first;
        for(char &c : lower_key) c = std::tolower(c);
        if (lower_key == "cookie") cookie_str = pair.second;
        
        if (!first) json += ",";
        json += "\"" + escape_json(pair.first) + "\":\"" + escape_json(pair.second) + "\"";
        first = false;
    }
    json += "},";

    // Cookies parsing
    std::string cookies_json = "{";
    if (!cookie_str.empty()) {
        size_t start = 0;
        bool cfirst = true;
        while (start < cookie_str.length()) {
            size_t end = cookie_str.find(';', start);
            if (end == std::string::npos) end = cookie_str.length();
            std::string pair = cookie_str.substr(start, end - start);
            pair.erase(0, pair.find_first_not_of(" \t"));
            size_t eq = pair.find('=');
            if (eq != std::string::npos) {
                if (!cfirst) cookies_json += ",";
                std::string k = pair.substr(0, eq);
                std::string v = pair.substr(eq + 1);
                cookies_json += "\"" + escape_json(k) + "\":\"" + escape_json(v) + "\"";
                cfirst = false;
            }
            start = end + 1;
        }
    }
    cookies_json += "},";
    json += "\"cookies\":" + cookies_json;

    json += "\"body\":\"" + escape_json(body) + "\"";
    json += "}";

    {
        std::lock_guard<std::mutex> lock(g_sockets_mutex);
        g_active_sockets[req_id] = client_socket;
    }

    {
        std::lock_guard<std::mutex> lock(g_queue_mutex);
        g_request_queue.push(json);
    }
}

void server_loop() {
    while (g_server_running) {
        sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        SOCKET client_socket = accept(g_listen_socket, (sockaddr*)&client_addr, &client_len);
        if (client_socket != INVALID_SOCKET) {
            int req_id = g_next_id++;
            std::thread(client_handler, client_socket, req_id).detach();
        }
    }
}

extern "C" {
    __declspec(dllexport) int http_server_start(int port) {
        if (g_server_running) return 0;
        
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return -1;

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

    __declspec(dllexport) const char* http_server_poll() {
        std::lock_guard<std::mutex> qlock(g_queue_mutex);
        if (g_request_queue.empty()) return nullptr;

        std::string json = g_request_queue.front();
        g_request_queue.pop();
        
        std::lock_guard<std::mutex> plock(g_poll_mutex);
        g_poll_buffer = json;
        return g_poll_buffer.c_str();
    }

    __declspec(dllexport) void http_server_respond(int client_id, const char* response_string, int response_len) {
        SOCKET client_socket = INVALID_SOCKET;
        {
            std::lock_guard<std::mutex> lock(g_sockets_mutex);
            auto it = g_active_sockets.find(client_id);
            if (it != g_active_sockets.end()) {
                client_socket = it->second;
                g_active_sockets.erase(it);
            }
        }

        if (client_socket != INVALID_SOCKET) {
            if (response_string && response_len > 0) {
                int sent = 0;
                while (sent < response_len) {
                    int s = send(client_socket, response_string + sent, response_len - sent, 0);
                    if (s <= 0) break;
                    sent += s;
                }
            }
            closesocket(client_socket);
            g_active_handlers--;
        }
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
