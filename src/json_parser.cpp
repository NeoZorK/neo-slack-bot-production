#include "json_parser.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace json_parser {
    std::string extract_string(const std::string& json, const std::string& key) {
        std::string search_key = "\"" + key + "\"";
        size_t key_pos = json.find(search_key);
        if (key_pos == std::string::npos) {
            return "";
        }
        
        size_t colon_pos = json.find(':', key_pos);
        if (colon_pos == std::string::npos) {
            return "";
        }
        
        size_t start = colon_pos + 1;
        while (start < json.length() && (json[start] == ' ' || json[start] == '\t')) {
            start++;
        }
        
        if (start >= json.length()) {
            return "";
        }
        
        if (json[start] == '"') {
            start++;
            size_t end = json.find('"', start);
            if (end == std::string::npos) {
                return "";
            }
            return json.substr(start, end - start);
        } else if (json[start] == '{') {
            int brace_count = 1;
            size_t end = start + 1;
            while (end < json.length() && brace_count > 0) {
                if (json[end] == '{') brace_count++;
                else if (json[end] == '}') brace_count--;
                end++;
            }
            if (brace_count == 0) {
                return json.substr(start, end - start);
            }
        } else if (json[start] == '[') {
            int bracket_count = 1;
            size_t end = start + 1;
            while (end < json.length() && bracket_count > 0) {
                if (json[end] == '[') bracket_count++;
                else if (json[end] == ']') bracket_count--;
                end++;
            }
            if (bracket_count == 0) {
                return json.substr(start, end - start);
            }
        }
        
        return "";
    }
    
    bool extract_bool(const std::string& json, const std::string& key, bool default_val) {
        std::string search_key = "\"" + key + "\"";
        size_t key_pos = json.find(search_key);
        if (key_pos == std::string::npos) {
            return default_val;
        }
        
        size_t colon_pos = json.find(':', key_pos);
        if (colon_pos == std::string::npos) {
            return default_val;
        }
        
        size_t start = colon_pos + 1;
        while (start < json.length() && (json[start] == ' ' || json[start] == '\t')) {
            start++;
        }
        
        if (start >= json.length()) {
            return default_val;
        }
        
        if (json[start] == '"') {
            std::string val = extract_string(json, key);
            std::string lower_val = val;
            std::transform(lower_val.begin(), lower_val.end(), lower_val.begin(), ::tolower);
            if (lower_val == "true" || lower_val == "1") {
                return true;
            }
            return false;
        } else {
            if (start + 4 <= json.length() && json.substr(start, 4) == "true") {
                return true;
            }
            if (start + 5 <= json.length() && json.substr(start, 5) == "false") {
                return false;
            }
        }
        
        return default_val;
    }
    
    int extract_int(const std::string& json, const std::string& key, int default_val) {
        std::string search_key = "\"" + key + "\"";
        size_t key_pos = json.find(search_key);
        if (key_pos == std::string::npos) {
            return default_val;
        }
        
        size_t colon_pos = json.find(':', key_pos);
        if (colon_pos == std::string::npos) {
            return default_val;
        }
        
        size_t start = colon_pos + 1;
        while (start < json.length() && (json[start] == ' ' || json[start] == '\t')) {
            start++;
        }
        
        if (start >= json.length()) {
            return default_val;
        }
        
        size_t end = start;
        while (end < json.length() && (std::isdigit(json[end]) || json[end] == '-')) {
            end++;
        }
        
        if (end > start) {
            try {
                return std::stoi(json.substr(start, end - start));
            } catch (...) {
                return default_val;
            }
        }
        
        return default_val;
    }
    
    std::vector<std::string> extract_string_array(const std::string& json, const std::string& key) {
        std::vector<std::string> result;
        std::string array_json = extract_string(json, key);
        
        if (array_json.empty() || array_json[0] != '[') {
            return result;
        }
        
        size_t pos = 1;
        while (pos < array_json.length() && array_json[pos] != ']') {
            while (pos < array_json.length() && (array_json[pos] == ' ' || array_json[pos] == '\t' || array_json[pos] == ',')) {
                pos++;
            }
            
            if (pos >= array_json.length() || array_json[pos] == ']') {
                break;
            }
            
            if (array_json[pos] == '"') {
                pos++;
                size_t end = array_json.find('"', pos);
                if (end != std::string::npos) {
                    result.push_back(array_json.substr(pos, end - pos));
                    pos = end + 1;
                } else {
                    break;
                }
            } else {
                pos++;
            }
        }
        
        return result;
    }
}
