#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cctype>
#include <unordered_map>
#include <mutex>

extern "C" {

    // ---------------------------------------------------------
    // REGEX MODULE
    // ---------------------------------------------------------
    __declspec(dllexport) int regex_replace_ext(const char* subject, const char* pattern, const char* replacement, char* out_buf, int max_len) {
        try {
            std::regex re(pattern);
            std::string result = std::regex_replace(subject, re, replacement);
            if (result.length() >= (size_t)max_len) return -1;
            strcpy(out_buf, result.c_str());
            return result.length();
        } catch (...) { return -2; }
    }

    __declspec(dllexport) int regex_match_ext(const char* subject, const char* pattern) {
        try { return std::regex_match(subject, std::regex(pattern)) ? 1 : 0; } catch (...) { return -1; }
    }

    __declspec(dllexport) int regex_search_ext(const char* subject, const char* pattern) {
        try { return std::regex_search(subject, std::regex(pattern)) ? 1 : 0; } catch (...) { return -1; }
    }

    __declspec(dllexport) void free_template_buffer(char* ptr) {
        if (ptr) {
            delete[] ptr;
        }
    }

} // extern "C"

// ---------------------------------------------------------
// CUSTOM JSON PARSER
// ---------------------------------------------------------

enum JsonType { JSON_NULL, JSON_OBJECT, JSON_ARRAY, JSON_STRING, JSON_NUMBER, JSON_BOOLEAN };

struct JsonValue {
    JsonType type = JSON_NULL;
    std::string strVal;
    double numVal = 0.0;
    bool boolVal = false;
    std::map<std::string, JsonValue> objVal;
    std::vector<JsonValue> arrVal;

    JsonValue() {}
    JsonValue(JsonType t) : type(t) {}
    JsonValue(const std::string& s) : type(JSON_STRING), strVal(s) {}
    JsonValue(double n) : type(JSON_NUMBER), numVal(n) {}
    JsonValue(bool b) : type(JSON_BOOLEAN), boolVal(b) {}
    
    // operator[] for objects
    JsonValue& operator[](const std::string& key) {
        if (type == JSON_NULL) type = JSON_OBJECT;
        return objVal[key];
    }
};

class JsonParser {
    const char* p;
    
    void skipWs() {
        while (*p && isspace((unsigned char)*p)) p++;
    }
    
    std::string parseString() {
        std::string res;
        if (*p != '"') return "";
        p++; // skip "
        while (*p && *p != '"') {
            if (*p == '\\') {
                p++;
                if (*p == '"') res += '"';
                else if (*p == '\\') res += '\\';
                else if (*p == '/') res += '/';
                else if (*p == 'b') res += '\b';
                else if (*p == 'f') res += '\f';
                else if (*p == 'n') res += '\n';
                else if (*p == 'r') res += '\r';
                else if (*p == 't') res += '\t';
                else res += *p;
                if (*p) p++;
            } else {
                res += *p++;
            }
        }
        if (*p == '"') p++;
        return res;
    }
    
    JsonValue parseObject() {
        JsonValue obj(JSON_OBJECT);
        p++; // skip {
        skipWs();
        if (*p == '}') { p++; return obj; }
        while (*p) {
            skipWs();
            std::string key = parseString();
            skipWs();
            if (*p == ':') p++;
            skipWs();
            JsonValue val = parseValue();
            obj.objVal[key] = val;
            skipWs();
            if (*p == ',') { p++; }
            else if (*p == '}') { p++; break; }
            else break;
        }
        return obj;
    }
    
    JsonValue parseArray() {
        JsonValue arr(JSON_ARRAY);
        p++; // skip [
        skipWs();
        if (*p == ']') { p++; return arr; }
        while (*p) {
            skipWs();
            arr.arrVal.push_back(parseValue());
            skipWs();
            if (*p == ',') { p++; }
            else if (*p == ']') { p++; break; }
            else break;
        }
        return arr;
    }
    
    JsonValue parseValue() {
        skipWs();
        if (*p == '"') return JsonValue(parseString());
        if (*p == '{') return parseObject();
        if (*p == '[') return parseArray();
        
        if (strncmp(p, "true", 4) == 0) { p += 4; return JsonValue(true); }
        if (strncmp(p, "false", 5) == 0) { p += 5; return JsonValue(false); }
        if (strncmp(p, "null", 4) == 0) { p += 4; return JsonValue(JSON_NULL); }
        
        // Parse number
        const char* start = p;
        if (*p == '-') p++;
        while (*p && (isdigit((unsigned char)*p) || *p == '.' || *p == 'e' || *p == 'E' || *p == '+' || *p == '-')) p++;
        std::string numStr(start, p - start);
        try { return JsonValue(std::stod(numStr)); } 
        catch (...) { return JsonValue(JSON_NULL); }
    }

public:
    bool parse(const char* jsonStr, JsonValue& root) {
        p = jsonStr;
        skipWs();
        if (!*p) return false;
        root = parseValue();
        return true;
    }
};

// ---------------------------------------------------------
// TEMPLATE ENGINE MODULE
// ---------------------------------------------------------

struct TemplateContext {
    JsonValue root;
    std::string baseDir;
    std::map<std::string, std::string> blocks;
    
    const JsonValue* resolvePath(const std::string& path) const {
        if (path == "this" || path == ".") return &root;
        
        std::istringstream iss(path);
        std::string token;
        const JsonValue* current = &root;
        
        while (std::getline(iss, token, '.')) {
            token.erase(0, token.find_first_not_of(" \t\r\n"));
            token.erase(token.find_last_not_of(" \t\r\n") + 1);
            if (token.empty()) continue;
            
            if (current->type == JSON_OBJECT) {
                auto it = current->objVal.find(token);
                if (it != current->objVal.end()) {
                    current = &it->second;
                } else {
                    return nullptr;
                }
            } else {
                return nullptr;
            }
        }
        return current;
    }
};

// Returns a string representation without trailing zeros if integer
std::string numToString(double d) {
    if (d == std::floor(d)) {
        return std::to_string((long long)d);
    }
    std::ostringstream ss;
    ss << d;
    return ss.str();
}

std::string resolve_var_str(const TemplateContext& ctx, std::string expression) {
    expression.erase(0, expression.find_first_not_of(" \t\r\n"));
    expression.erase(expression.find_last_not_of(" \t\r\n") + 1);
    
    std::vector<std::string> filters;
    size_t pipePos = expression.find('|');
    std::string varPath = expression;
    if (pipePos != std::string::npos) {
        varPath = expression.substr(0, pipePos);
        varPath.erase(varPath.find_last_not_of(" \t\r\n") + 1);
        
        std::string rest = expression.substr(pipePos + 1);
        std::istringstream piss(rest);
        std::string filt;
        while (std::getline(piss, filt, '|')) {
            filt.erase(0, filt.find_first_not_of(" \t\r\n"));
            filt.erase(filt.find_last_not_of(" \t\r\n") + 1);
            filters.push_back(filt);
        }
    }
    
    std::string rawVal = "";
    
    if (varPath.size() >= 2 && varPath.front() == '"' && varPath.back() == '"') {
        rawVal = varPath.substr(1, varPath.length() - 2);
    } else if (varPath.size() >= 2 && varPath.front() == '\'' && varPath.back() == '\'') {
        rawVal = varPath.substr(1, varPath.length() - 2);
    }
    else if (!varPath.empty() && (isdigit((unsigned char)varPath[0]) || varPath[0] == '-')) {
        rawVal = varPath;
    }
    else {
        const JsonValue* v = ctx.resolvePath(varPath);
        if (v) {
            if (v->type == JSON_STRING) rawVal = v->strVal;
            else if (v->type == JSON_NUMBER) rawVal = numToString(v->numVal);
            else if (v->type == JSON_BOOLEAN) rawVal = v->boolVal ? "true" : "false";
            else if (v->type == JSON_OBJECT || v->type == JSON_ARRAY) rawVal = "[Object]";
        }
    }
    
    bool isSafe = false;
    for (const std::string& f : filters) {
        if (f == "safe") isSafe = true;
        else if (f.find("default(") == 0) {
            if (rawVal.empty()) {
                size_t start = f.find('"');
                size_t end = f.rfind('"');
                if (start != std::string::npos && end != std::string::npos && end > start) {
                    rawVal = f.substr(start + 1, end - start - 1);
                } else {
                    start = f.find('\'');
                    end = f.rfind('\'');
                    if (start != std::string::npos && end != std::string::npos && end > start) {
                        rawVal = f.substr(start + 1, end - start - 1);
                    }
                }
            }
        }
        else if (f.find("round(") == 0) {
            try {
                double d = std::stod(rawVal);
                size_t s = f.find('(');
                size_t e = f.find(')');
                int precision = 0;
                if (s != std::string::npos && e != std::string::npos && e > s + 1) {
                    precision = std::stoi(f.substr(s + 1, e - s - 1));
                }
                double mult = std::pow(10.0, precision);
                double rounded = std::round(d * mult) / mult;
                std::ostringstream out;
                out.precision(precision);
                out << std::fixed << rounded;
                rawVal = out.str();
            } catch (...) {}
        }
    }
    
    if (!isSafe) {
        std::string escaped;
        for (char c : rawVal) {
            if (c == '<') escaped += "&lt;";
            else if (c == '>') escaped += "&gt;";
            else if (c == '&') escaped += "&amp;";
            else if (c == '"') escaped += "&quot;";
            else escaped += c;
        }
        return escaped;
    }
    return rawVal;
}

bool eval_condition(const TemplateContext& ctx, std::string cond) {
    cond.erase(0, cond.find_first_not_of(" \t\r\n"));
    cond.erase(cond.find_last_not_of(" \t\r\n") + 1);
    
    bool negate = false;
    if (cond.find("not ") == 0 || cond.find("!") == 0) {
        negate = true;
        cond = cond.substr(cond.find("not ") == 0 ? 4 : 1);
        cond.erase(0, cond.find_first_not_of(" \t\r\n"));
    }
    
    bool result = false;
    size_t eqPos = cond.find("==");
    size_t neqPos = cond.find("!=");
    
    if (eqPos != std::string::npos) {
        std::string lhs = cond.substr(0, eqPos);
        std::string rhs = cond.substr(eqPos + 2);
        result = (resolve_var_str(ctx, lhs + " | safe") == resolve_var_str(ctx, rhs + " | safe"));
    } else if (neqPos != std::string::npos) {
        std::string lhs = cond.substr(0, neqPos);
        std::string rhs = cond.substr(neqPos + 2);
        result = (resolve_var_str(ctx, lhs + " | safe") != resolve_var_str(ctx, rhs + " | safe"));
    } else {
        std::string val = resolve_var_str(ctx, cond + " | safe");
        result = (!val.empty() && val != "false" && val != "0");
    }
    return negate ? !result : result;
}

size_t find_matching_tag(const std::string& tmpl, size_t startPos, const std::string& openTagPrefix, const std::string& closeTag) {
    int depth = 1;
    size_t pos = startPos;
    
    std::string tagType = openTagPrefix.substr(3);
    size_t spacePos = tagType.find(' ');
    if (spacePos != std::string::npos) tagType = tagType.substr(0, spacePos);
    
    while (pos < tmpl.length()) {
        size_t nextTagStart = tmpl.find("{%", pos);
        if (nextTagStart == std::string::npos) return std::string::npos;
        
        size_t nextTagEnd = tmpl.find("%}", nextTagStart);
        if (nextTagEnd == std::string::npos) return std::string::npos;
        
        std::string inner = tmpl.substr(nextTagStart + 2, nextTagEnd - nextTagStart - 2);
        inner.erase(0, inner.find_first_not_of(" \t\r\n"));
        
        if (inner.find(tagType) == 0 && (inner.length() == tagType.length() || isspace(inner[tagType.length()]))) {
            depth++;
        }
        else if (inner.find(closeTag.substr(3, closeTag.length()-6)) == 0) {
            depth--;
            if (depth == 0) return nextTagStart;
        }
        
        pos = nextTagEnd + 2;
    }
    return std::string::npos;
}

std::string render_ast(const std::string& tmpl, TemplateContext& ctx) {
    std::string result;
    size_t i = 0;
    size_t len = tmpl.length();
    
    while (i < len) {
        size_t vStart = tmpl.find("{{", i);
        size_t sStart = tmpl.find("{%", i);
        
        if (vStart == std::string::npos && sStart == std::string::npos) {
            result += tmpl.substr(i);
            break;
        }
        
        size_t closest = (vStart != std::string::npos && sStart != std::string::npos) ? std::min(vStart, sStart) : (vStart != std::string::npos ? vStart : sStart);
        bool isStmt = (closest == sStart);
        
        result += tmpl.substr(i, closest - i);
        
        if (!isStmt) {
            size_t vEnd = tmpl.find("}}", closest);
            if (vEnd == std::string::npos) { result += tmpl.substr(closest); break; }
            std::string varPath = tmpl.substr(closest + 2, vEnd - closest - 2);
            result += resolve_var_str(ctx, varPath);
            i = vEnd + 2;
        } else {
            size_t sEnd = tmpl.find("%}", closest);
            if (sEnd == std::string::npos) { result += tmpl.substr(closest); break; }
            
            std::string stmtRaw = tmpl.substr(closest + 2, sEnd - closest - 2);
            stmtRaw.erase(0, stmtRaw.find_first_not_of(" \t\r\n"));
            stmtRaw.erase(stmtRaw.find_last_not_of(" \t\r\n") + 1);
            
            if (stmtRaw.find("if ") == 0) {
                std::string cond = stmtRaw.substr(3);
                
                size_t endIfPos = find_matching_tag(tmpl, sEnd + 2, "{% if ", "{% endif %}");
                if (endIfPos == std::string::npos) { i = sEnd + 2; continue; }
                
                std::string fullIfBlock = tmpl.substr(sEnd + 2, endIfPos - (sEnd + 2));
                
                std::vector<std::pair<std::string, std::string>> branches;
                branches.push_back({cond, ""});
                
                int depth = 0;
                size_t blockScan = 0;
                size_t lastBranchStart = 0;
                while (blockScan < fullIfBlock.length()) {
                    size_t nextOpen = fullIfBlock.find("{%", blockScan);
                    if (nextOpen == std::string::npos) break;
                    size_t nextClose = fullIfBlock.find("%}", nextOpen);
                    if (nextClose == std::string::npos) break;
                    
                    std::string innerStmt = fullIfBlock.substr(nextOpen + 2, nextClose - nextOpen - 2);
                    innerStmt.erase(0, innerStmt.find_first_not_of(" \t\r\n"));
                    
                    if (innerStmt.find("if ") == 0) depth++;
                    else if (innerStmt.find("endif") == 0) depth--;
                    else if (depth == 0 && (innerStmt.find("elif ") == 0 || innerStmt.find("else") == 0)) {
                        branches.back().second = fullIfBlock.substr(lastBranchStart, nextOpen - lastBranchStart);
                        if (innerStmt.find("elif ") == 0) {
                            branches.push_back({innerStmt.substr(5), ""});
                        } else {
                            branches.push_back({"\"true\"", ""});
                        }
                        lastBranchStart = nextClose + 2;
                    }
                    blockScan = nextClose + 2;
                }
                branches.back().second = fullIfBlock.substr(lastBranchStart);
                
                for (const auto& branch : branches) {
                    if (eval_condition(ctx, branch.first)) {
                        result += render_ast(branch.second, ctx);
                        break;
                    }
                }
                
                i = tmpl.find("%}", endIfPos) + 2;
            }
            else if (stmtRaw.find("for ") == 0) {
                size_t inPos = stmtRaw.find(" in ");
                if (inPos != std::string::npos) {
                    std::string loopVar = stmtRaw.substr(4, inPos - 4);
                    std::string arrayPath = stmtRaw.substr(inPos + 4);
                    
                    loopVar.erase(0, loopVar.find_first_not_of(" \t\r\n"));
                    loopVar.erase(loopVar.find_last_not_of(" \t\r\n") + 1);
                    arrayPath.erase(0, arrayPath.find_first_not_of(" \t\r\n"));
                    arrayPath.erase(arrayPath.find_last_not_of(" \t\r\n") + 1);
                    
                    size_t endForPos = find_matching_tag(tmpl, sEnd + 2, "{% for ", "{% endfor %}");
                    if (endForPos != std::string::npos) {
                        std::string loopBody = tmpl.substr(sEnd + 2, endForPos - (sEnd + 2));
                        const JsonValue* arrNode = ctx.resolvePath(arrayPath);
                        
                        if (arrNode && arrNode->type == JSON_ARRAY) {
                            for (size_t idx = 0; idx < arrNode->arrVal.size(); idx++) {
                                const auto& item = arrNode->arrVal[idx];
                                TemplateContext subCtx = ctx; 
                                subCtx.root.objVal[loopVar] = item;
                                
                                JsonValue loopObj(JSON_OBJECT);
                                loopObj.objVal["index"] = JsonValue((double)(idx + 1));
                                loopObj.objVal["index0"] = JsonValue((double)idx);
                                loopObj.objVal["first"] = JsonValue(idx == 0);
                                loopObj.objVal["last"] = JsonValue(idx == arrNode->arrVal.size() - 1);
                                subCtx.root.objVal["loop"] = loopObj;
                                
                                result += render_ast(loopBody, subCtx);
                            }
                        }
                        i = tmpl.find("%}", endForPos) + 2;
                    } else {
                        i = sEnd + 2;
                    }
                } else {
                    i = sEnd + 2;
                }
            }
            else if (stmtRaw.find("block ") == 0) {
                std::string blockName = stmtRaw.substr(6);
                blockName.erase(0, blockName.find_first_not_of(" \t\r\n"));
                
                size_t endBlockPos = find_matching_tag(tmpl, sEnd + 2, "{% block ", "{% endblock %}");
                if (endBlockPos != std::string::npos) {
                    std::string defaultBody = tmpl.substr(sEnd + 2, endBlockPos - (sEnd + 2));
                    
                    if (ctx.blocks.find(blockName) != ctx.blocks.end()) {
                        result += render_ast(ctx.blocks[blockName], ctx);
                    } else {
                        result += render_ast(defaultBody, ctx);
                    }
                    
                    i = tmpl.find("%}", endBlockPos) + 2;
                } else {
                    i = sEnd + 2;
                }
            }
            else if (stmtRaw.find("include ") == 0) {
                std::string incPath = stmtRaw.substr(8);
                incPath.erase(0, incPath.find_first_not_of(" \"\'\t\r\n"));
                incPath.erase(incPath.find_last_not_of(" \"\'\t\r\n") + 1);
                
                std::string fullPath = ctx.baseDir + "/" + incPath;
                std::ifstream ifs(fullPath);
                if (ifs.is_open()) {
                    std::string incContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                    result += render_ast(incContent, ctx);
                } else {
                    result += "[Error: Could not include " + incPath + "]";
                }
                i = sEnd + 2;
            }
            else {
                i = sEnd + 2;
            }
        }
    }
    return result;
}

extern "C" {
    __declspec(dllexport) char* render_template_ext(const char* tmpl, const char* ctx_json, const char* base_dir) {
        TemplateContext ctx;
        ctx.baseDir = base_dir ? base_dir : "";
        
        JsonParser parser;
        if (!parser.parse(ctx_json, ctx.root)) {
            const char* err = "[TEMPLATE ERROR: Failed to parse JSON context]";
            char* out = new char[strlen(err) + 1];
            strcpy(out, err);
            return out;
        }
        
        std::string t(tmpl);
        
        size_t extPos = t.find("{% extends ");
        if (extPos != std::string::npos) {
            size_t extEnd = t.find("%}", extPos);
            if (extEnd != std::string::npos) {
                std::string parentPath = t.substr(extPos + 11, extEnd - extPos - 11);
                parentPath.erase(0, parentPath.find_first_not_of(" \"\'\t\r\n"));
                parentPath.erase(parentPath.find_last_not_of(" \"\'\t\r\n") + 1);
                
                size_t bSearch = extEnd + 2;
                while (bSearch < t.length()) {
                    size_t bStart = t.find("{% block ", bSearch);
                    if (bStart == std::string::npos) break;
                    size_t bEnd = t.find("%}", bStart);
                    if (bEnd == std::string::npos) break;
                    
                    std::string blockName = t.substr(bStart + 9, bEnd - bStart - 9);
                    blockName.erase(0, blockName.find_first_not_of(" \t\r\n"));
                    blockName.erase(blockName.find_last_not_of(" \t\r\n") + 1);
                    
                    size_t bClose = find_matching_tag(t, bEnd + 2, "{% block ", "{% endblock %}");
                    if (bClose != std::string::npos) {
                        ctx.blocks[blockName] = t.substr(bEnd + 2, bClose - bEnd - 2);
                        bSearch = t.find("%}", bClose) + 2;
                    } else {
                        break;
                    }
                }
                
                std::string fullParentPath = ctx.baseDir + "/" + parentPath;
                static std::unordered_map<std::string, std::string> g_templateCache;
                static std::mutex g_cacheMutex;
                
                {
                    std::lock_guard<std::mutex> lock(g_cacheMutex);
                    auto it = g_templateCache.find(fullParentPath);
                    if (it != g_templateCache.end()) {
                        t = it->second;
                    } else {
                        std::ifstream ifs(fullParentPath);
                        if (ifs.is_open()) {
                            t = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                            g_templateCache[fullParentPath] = t;
                        } else {
                            t = "[TEMPLATE ERROR: Could not read parent template " + parentPath + "]";
                        }
                    }
                }
            }
        }
        
        std::string rendered = render_ast(t, ctx);
        
        char* out = new char[rendered.length() + 1];
        strcpy(out, rendered.c_str());
        return out;
    }
}
