#include "slack_api.hpp"
#include "rate_limiter.hpp"
#include "error_recovery.hpp"
#include "logger.hpp"
#include "input_validator.hpp"
#include <iostream>
#include <sstream>
#include <regex>
#include <thread>
#include <chrono>

// Core implementation moved to slack_api_core.cpp:
// - Constructor/Destructor
// - make_api_request
// - build_api_url
// - escape_json_string

bool SlackAPI::send_message(const std::string& channel, const std::string& text) {
    std::string error_response;
    return send_message(channel, text, error_response);
}

bool SlackAPI::send_message(const std::string& channel, const std::string& text, std::string& error_response) {
    if (!InputValidator::is_valid_channel_id(channel)) {
        Logger::get_instance().error("Invalid channel ID: " + channel);
        error_recovery_->handle_error(ErrorType::API_ERROR, "Invalid channel ID");
        error_response = "Invalid channel ID: " + channel;
        return false;
    }
    
    if (!InputValidator::is_valid_message_text(text)) {
        Logger::get_instance().error("Invalid message text (empty or too long)");
        error_recovery_->handle_error(ErrorType::API_ERROR, "Invalid message text");
        error_response = "Invalid message text (empty or too long)";
        return false;
    }
    
    std::string url = build_api_url("chat.postMessage");
    std::ostringstream json;
    json << "{\"channel\":\"" << escape_json_string(channel) 
         << "\",\"text\":\"" << escape_json_string(text) << "\"}";
    std::string response;
    bool success = make_api_request(url, json.str(), response, true);
    if (!success) {
        error_response = response.empty() ? "Unknown error" : response;
    }
    return success;
}

bool SlackAPI::get_channel_info(const std::string& channel_id, std::string& info) {
    if (!InputValidator::is_valid_channel_id(channel_id)) {
        Logger::get_instance().error("Invalid channel ID: " + channel_id);
        error_recovery_->handle_error(ErrorType::API_ERROR, "Invalid channel ID");
        return false;
    }
    
    std::string url = build_api_url("conversations.info?channel=" + channel_id);
    std::string response;
    if (make_api_request(url, "", response, false) && 
        response.find("\"ok\":true") != std::string::npos) {
        info = response;
        return true;
    }
    return false;
}

bool SlackAPI::get_user_info(const std::string& user_id, std::string& info) {
    if (!InputValidator::is_valid_user_id(user_id)) {
        Logger::get_instance().error("Invalid user ID: " + user_id);
        error_recovery_->handle_error(ErrorType::API_ERROR, "Invalid user ID");
        return false;
    }
    
    std::string url = build_api_url("users.info?user=" + user_id);
    std::string response;
    if (make_api_request(url, "", response, false) && 
        response.find("\"ok\":true") != std::string::npos) {
        info = response;
        return true;
    }
    return false;
}

bool SlackAPI::get_socket_mode_url(std::string& websocket_url) {
    std::string url = build_api_url("apps.connections.open");
    std::string response;
    
    Logger::get_instance().debug("Calling apps.connections.open");
    Logger::get_instance().debug("URL: " + url);
    Logger::get_instance().debug("Token: " + token_.substr(0, 15) + "...");
    
    // Save original Content-Type and temporarily change to form-urlencoded
    // Note: apps.connections.open requires form-urlencoded format
    http_client_->set_header("Content-Type", "application/x-www-form-urlencoded");
    
    bool success = make_api_request(url, "", response, true);
    
    // Restore original Content-Type
    http_client_->set_header("Content-Type", "application/json");
    
    if (!success || response.empty()) {
        Logger::get_instance().error("Failed to get Socket Mode URL from apps.connections.open");
        if (response.empty()) {
            Logger::get_instance().error("Empty response - possible causes:");
            Logger::get_instance().error("  1. Network connection issue");
            Logger::get_instance().error("  2. Invalid App-Level Token");
            Logger::get_instance().error("  3. App-Level Token missing 'connections:write' scope");
            Logger::get_instance().error("  4. Socket Mode not enabled in Slack App settings");
        } else {
            Logger::get_instance().error("Response: " + response);
        }
        return false;
    }
    
    // Parse response: {"ok":true,"url":"wss://wss.slack.com/link/?ticket=..."}
    if (response.find("\"ok\":true") == std::string::npos) {
        Logger::get_instance().error("apps.connections.open returned error: " + response);
        // Check for specific error messages
        if (response.find("invalid_auth") != std::string::npos) {
            Logger::get_instance().error("Authentication failed - check if App-Level Token is valid");
        } else if (response.find("missing_scope") != std::string::npos) {
            Logger::get_instance().error("Missing scope - App-Level Token needs 'connections:write' scope");
        } else if (response.find("HTTP ") == 0) {
            Logger::get_instance().error("HTTP error: " + response);
        }
        return false;
    }
    
    // Extract URL from response
    size_t url_start = response.find("\"url\":\"");
    if (url_start == std::string::npos) {
        Logger::get_instance().error("No URL found in apps.connections.open response");
        Logger::get_instance().debug("Full response: " + response);
        return false;
    }
    
    url_start += 7; // Skip past "url":"
    size_t url_end = response.find("\"", url_start);
    if (url_end == std::string::npos) {
        Logger::get_instance().error("Invalid URL format in apps.connections.open response");
        return false;
    }
    
    websocket_url = response.substr(url_start, url_end - url_start);
    
    // Decode escaped slashes in URL (JSON encoding: \/ becomes /)
    size_t pos = 0;
    while ((pos = websocket_url.find("\\/", pos)) != std::string::npos) {
        websocket_url.replace(pos, 2, "/");
        pos += 1;
    }
    
    Logger::get_instance().info("Got Socket Mode URL from apps.connections.open: " + 
                                websocket_url.substr(0, 50) + "...");
    return true;
}

bool SlackAPI::get_bot_user_id(std::string& bot_user_id) {
    std::string url = build_api_url("auth.test");
    std::string response;
    if (make_api_request(url, "", response, false) && 
        response.find("\"ok\":true") != std::string::npos) {
        // Extract user_id from response
        size_t user_id_start = response.find("\"user_id\":\"");
        if (user_id_start != std::string::npos) {
            user_id_start += 11; // Length of "\"user_id\":\""
            size_t user_id_end = response.find("\"", user_id_start);
            if (user_id_end != std::string::npos) {
                bot_user_id = response.substr(user_id_start, user_id_end - user_id_start);
                return true;
            }
        }
    }
    return false;
}

bool SlackAPI::join_channel(const std::string& channel_id) {
    if (!InputValidator::is_valid_channel_id(channel_id)) {
        Logger::get_instance().error("Invalid channel ID: " + channel_id);
        error_recovery_->handle_error(ErrorType::API_ERROR, "Invalid channel ID");
        return false;
    }
    
    // First, get bot user ID
    std::string bot_user_id;
    if (!get_bot_user_id(bot_user_id)) {
        Logger::get_instance().error("Failed to get bot user ID");
        return false;
    }
    
    // Try to join channel (for public channels)
    std::string url = build_api_url("conversations.join");
    std::ostringstream json;
    json << "{\"channel\":\"" << escape_json_string(channel_id) << "\"}";
    std::string response;
    if (make_api_request(url, json.str(), response, true) && 
        response.find("\"ok\":true") != std::string::npos) {
        Logger::get_instance().info("Successfully joined channel: " + channel_id);
        return true;
    }
    
    // If join failed, try to invite bot to channel
    url = build_api_url("conversations.invite");
    json.str("");
    json << "{\"channel\":\"" << escape_json_string(channel_id) 
         << "\",\"users\":\"" << escape_json_string(bot_user_id) << "\"}";
    response.clear();
    if (make_api_request(url, json.str(), response, true) && 
        response.find("\"ok\":true") != std::string::npos) {
        Logger::get_instance().info("Successfully invited bot to channel: " + channel_id);
        return true;
    }
    
    Logger::get_instance().error("Failed to join or invite bot to channel: " + channel_id);
    Logger::get_instance().error("Response: " + response);
    return false;
}

