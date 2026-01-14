#include "slack_api.hpp"
#include "rate_limiter.hpp"
#include "error_recovery.hpp"
#include "logger.hpp"
#include "http_client.hpp"
#include <sstream>
#include <thread>
#include <chrono>

SlackAPI::SlackAPI(const std::string& token) 
    : token_(token), api_base_url_("https://slack.com/api/") {
    http_client_ = std::make_unique<HttpClient>();
    http_client_->set_header("Authorization", "Bearer " + token_);
    http_client_->set_header("Content-Type", "application/json");
    rate_limiter_ = std::make_unique<RateLimiter>();
    error_recovery_ = std::make_unique<ErrorRecovery>();
}

SlackAPI::SlackAPI(const std::string& token, std::unique_ptr<IHttpClient> http_client)
    : token_(token), http_client_(std::move(http_client)), api_base_url_("https://slack.com/api/") {
    http_client_->set_header("Authorization", "Bearer " + token_);
    http_client_->set_header("Content-Type", "application/json");
    rate_limiter_ = std::make_unique<RateLimiter>();
    error_recovery_ = std::make_unique<ErrorRecovery>();
}

SlackAPI::~SlackAPI() {
}

std::string SlackAPI::build_api_url(const std::string& endpoint) {
    return api_base_url_ + endpoint;
}

std::string SlackAPI::escape_json_string(const std::string& str) {
    std::ostringstream escaped;
    for (char c : str) {
        if (c == '"') escaped << "\\\"";
        else if (c == '\\') escaped << "\\\\";
        else if (c == '\n') escaped << "\\n";
        else if (c == '\r') escaped << "\\r";
        else if (c == '\t') escaped << "\\t";
        else escaped << c;
    }
    return escaped.str();
}

bool SlackAPI::make_api_request(const std::string& url, const std::string& data, 
                                std::string& response, bool is_post) {
    int retry_count = 0;
    const int max_retries = 3;
    
    while (retry_count <= max_retries) {
        if (!rate_limiter_->can_make_request()) {
            int wait_seconds = rate_limiter_->get_retry_after_seconds();
            Logger::get_instance().warn("Rate limited, waiting " + 
                                        std::to_string(wait_seconds) + " seconds");
            std::this_thread::sleep_for(std::chrono::seconds(wait_seconds));
            continue;
        }
        
        bool success = false;
        if (is_post) {
            success = http_client_->post(url, data, response);
        } else {
            success = http_client_->get(url, response);
        }
        
        rate_limiter_->record_request();
        rate_limiter_->handle_rate_limit_response(response);
        
        // Log response for debugging (even on failure)
        if (!response.empty()) {
            Logger::get_instance().debug("API response: " + response.substr(0, 200));
        } else {
            Logger::get_instance().warn("Empty API response received");
        }
        
        if (success && response.find("\"ok\":true") != std::string::npos) {
            error_recovery_->reset_error_count(ErrorType::API_ERROR);
            return true;
        }
        
        // Check for specific error types even if response is not empty
        if (response.find("rate_limited") != std::string::npos) {
            error_recovery_->handle_error(ErrorType::RATE_LIMIT_ERROR, 
                                         "Slack API rate limit exceeded");
            continue;
        }
        
        // Permanent errors that should not be retried
        if (response.find("invalid_auth") != std::string::npos ||
            response.find("account_inactive") != std::string::npos) {
            error_recovery_->handle_error(ErrorType::AUTHENTICATION_ERROR, 
                                         "Authentication failed");
            Logger::get_instance().error("Authentication error: " + response);
            return false;
        }
        
        // Permanent client errors that should not be retried
        if (response.find("channel_not_found") != std::string::npos ||
            response.find("not_in_channel") != std::string::npos ||
            response.find("missing_scope") != std::string::npos ||
            response.find("invalid_channel") != std::string::npos ||
            response.find("user_not_found") != std::string::npos) {
            // These are permanent errors - don't retry
            // Logger will automatically suppress duplicate errors
            Logger::get_instance().error("Permanent API error: " + response);
            return false;
        }
        
        // Log error with response if available
        std::string error_msg = "API request failed";
        if (!response.empty()) {
            error_msg += ": " + response.substr(0, 200);
        }
        error_recovery_->handle_error(ErrorType::API_ERROR, error_msg);
        
        if (!error_recovery_->should_retry(ErrorType::API_ERROR)) {
            return false;
        }
        
        retry_count++;
        int delay = error_recovery_->get_retry_delay(ErrorType::API_ERROR, retry_count - 1);
        Logger::get_instance().warn("Retrying API request after " + 
                                   std::to_string(delay) + " seconds");
        std::this_thread::sleep_for(std::chrono::seconds(delay));
    }
    
    return false;
}
