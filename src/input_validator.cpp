#include "input_validator.hpp"
#include <algorithm>
#include <cctype>

bool InputValidator::is_valid_bot_token(const std::string& token) {
    if (token.empty() || token.length() < MIN_TOKEN_LENGTH || 
        token.length() > MAX_TOKEN_LENGTH) {
        return false;
    }
    
    if (!starts_with_prefix(token, "xoxb-")) {
        return false;
    }
    
    return is_alphanumeric_or_special(token);
}

bool InputValidator::is_valid_app_token(const std::string& token) {
    if (token.empty() || token.length() < MIN_TOKEN_LENGTH || 
        token.length() > MAX_TOKEN_LENGTH) {
        return false;
    }
    
    if (!starts_with_prefix(token, "xapp-")) {
        return false;
    }
    
    return is_alphanumeric_or_special(token);
}

bool InputValidator::is_valid_channel_id(const std::string& channel_id) {
    if (channel_id.empty() || 
        channel_id.length() < MIN_CHANNEL_ID_LENGTH || 
        channel_id.length() > MAX_CHANNEL_ID_LENGTH) {
        return false;
    }
    
    if (!starts_with_prefix(channel_id, "C") && 
        !starts_with_prefix(channel_id, "D") &&
        !starts_with_prefix(channel_id, "G")) {
        return false;
    }
    
    return std::all_of(channel_id.begin() + 1, channel_id.end(),
                      [](char c) { return std::isalnum(c) || c == '_'; });
}

bool InputValidator::is_valid_user_id(const std::string& user_id) {
    if (user_id.empty() || 
        user_id.length() < MIN_USER_ID_LENGTH || 
        user_id.length() > MAX_USER_ID_LENGTH) {
        return false;
    }
    
    if (!starts_with_prefix(user_id, "U")) {
        return false;
    }
    
    return std::all_of(user_id.begin() + 1, user_id.end(),
                      [](char c) { return std::isalnum(c) || c == '_'; });
}

bool InputValidator::is_valid_message_text(const std::string& text) {
    if (text.length() > MAX_MESSAGE_LENGTH) {
        return false;
    }
    
    return true;
}

std::optional<std::string> InputValidator::sanitize_channel_id(const std::string& channel_id) {
    if (!is_valid_channel_id(channel_id)) {
        return std::nullopt;
    }
    
    std::string sanitized = channel_id;
    sanitized.erase(std::remove_if(sanitized.begin(), sanitized.end(),
                                  [](char c) { return !std::isalnum(c) && c != '_'; }),
                   sanitized.end());
    
    if (sanitized.length() < MIN_CHANNEL_ID_LENGTH || 
        sanitized.length() > MAX_CHANNEL_ID_LENGTH) {
        return std::nullopt;
    }
    
    return sanitized;
}

std::optional<std::string> InputValidator::sanitize_user_id(const std::string& user_id) {
    if (!is_valid_user_id(user_id)) {
        return std::nullopt;
    }
    
    std::string sanitized = user_id;
    sanitized.erase(std::remove_if(sanitized.begin(), sanitized.end(),
                                  [](char c) { return !std::isalnum(c) && c != '_'; }),
                   sanitized.end());
    
    if (sanitized.length() < MIN_USER_ID_LENGTH || 
        sanitized.length() > MAX_USER_ID_LENGTH) {
        return std::nullopt;
    }
    
    return sanitized;
}

std::optional<std::string> InputValidator::sanitize_message_text(const std::string& text) {
    if (text.length() > MAX_MESSAGE_LENGTH) {
        return std::nullopt;
    }
    
    std::string sanitized = text;
    sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), '\0'), sanitized.end());
    
    return sanitized;
}

bool InputValidator::is_alphanumeric_or_special(const std::string& str) {
    return std::all_of(str.begin(), str.end(),
                      [](char c) {
                          return std::isalnum(c) || c == '-' || c == '_' || c == '.';
                      });
}

bool InputValidator::starts_with_prefix(const std::string& str, const std::string& prefix) {
    if (str.length() < prefix.length()) {
        return false;
    }
    return str.compare(0, prefix.length(), prefix) == 0;
}
