#include "event_parser.hpp"
#include <algorithm>
#include <sstream>

event_parser::event_type event_parser::parse_type(const std::string& json) {
    if (json.find("\"type\":\"hello\"") != std::string::npos) {
        return event_type::hello;
    }
    if (json.find("\"type\":\"events_api\"") != std::string::npos) {
        return event_type::events_api;
    }
    if (json.find("\"type\":\"interactive\"") != std::string::npos) {
        return event_type::interactive;
    }
    if (json.find("\"type\":\"slash_commands\"") != std::string::npos) {
        return event_type::slash_commands;
    }
    if (json.find("\"type\":\"disconnect\"") != std::string::npos) {
        return event_type::disconnect;
    }
    return event_type::unknown;
}

std::optional<std::string> event_parser::extract_envelope_id(const std::string& json) {
    std::string id = extract_json_value(json, "envelope_id");
    if (!id.empty()) {
        return id;
    }
    return std::nullopt;
}

std::optional<std::string> event_parser::extract_payload(const std::string& json) {
    size_t payload_start = json.find("\"payload\":");
    if (payload_start == std::string::npos) {
        return std::nullopt;
    }
    
    payload_start += 10;
    size_t brace_count = 0;
    bool in_string = false;
    size_t start = payload_start;
    
    for (size_t i = payload_start; i < json.length(); ++i) {
        if (json[i] == '"' && (i == 0 || json[i-1] != '\\')) {
            in_string = !in_string;
        }
        if (!in_string) {
            if (json[i] == '{') {
                if (brace_count == 0) start = i;
                brace_count++;
            } else if (json[i] == '}') {
                brace_count--;
                if (brace_count == 0) {
                    return json.substr(start, i - start + 1);
                }
            }
        }
    }
    return std::nullopt;
}

std::optional<std::string> event_parser::extract_message_text(const std::string& json) {
    auto payload = extract_payload(json);
    if (!payload) {
        return std::nullopt;
    }
    
    std::string text = extract_json_value(*payload, "text");
    if (!text.empty()) {
        return text;
    }
    
    auto event = extract_json_value(*payload, "event");
    if (!event.empty()) {
        return extract_json_value(event, "text");
    }
    
    return std::nullopt;
}

std::optional<std::string> event_parser::extract_channel_id(const std::string& json) {
    auto payload = extract_payload(json);
    if (!payload) {
        return std::nullopt;
    }
    
    std::string channel = extract_json_value(*payload, "channel");
    if (!channel.empty()) {
        return channel;
    }
    
    auto event = extract_json_value(*payload, "event");
    if (!event.empty()) {
        return extract_json_value(event, "channel");
    }
    
    return std::nullopt;
}

std::optional<std::string> event_parser::extract_user_id(const std::string& json) {
    auto payload = extract_payload(json);
    if (!payload) {
        return std::nullopt;
    }
    
    std::string user = extract_json_value(*payload, "user");
    if (!user.empty()) {
        return user;
    }
    
    auto event = extract_json_value(*payload, "event");
    if (!event.empty()) {
        return extract_json_value(event, "user");
    }
    
    return std::nullopt;
}

bool event_parser::is_message_event(const std::string& json) {
    auto payload = extract_payload(json);
    if (!payload) {
        return false;
    }
    
    auto event = extract_json_value(*payload, "event");
    if (event.empty()) {
        return false;
    }
    
    return extract_json_value(event, "type").find("message") != std::string::npos;
}

bool event_parser::is_app_mention(const std::string& json) {
    auto payload = extract_payload(json);
    if (!payload) {
        return false;
    }
    
    auto event = extract_json_value(*payload, "event");
    if (event.empty()) {
        return false;
    }
    
    return extract_json_value(event, "type") == "app_mention";
}

std::optional<std::string> event_parser::extract_command_text(const std::string& json) {
    auto payload = extract_payload(json);
    if (!payload) {
        return std::nullopt;
    }
    
    std::string command = extract_json_value(*payload, "command");
    std::string text = extract_json_value(*payload, "text");
    
    if (!command.empty()) {
        std::string full_command = command;
        if (!text.empty()) {
            full_command += " " + text;
        }
        return full_command;
    }
    
    return std::nullopt;
}

std::string event_parser::extract_json_value(const std::string& json, const std::string& key) {
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
    }
    
    return "";
}
