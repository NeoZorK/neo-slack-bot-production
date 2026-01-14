#include "rate_limiter.hpp"
#include "logger.hpp"
#include <regex>
#include <algorithm>

namespace {
    constexpr int DEFAULT_MAX_REQUESTS_PER_MINUTE = 20;
    constexpr int RATE_LIMIT_WINDOW_SECONDS = 60;
}

RateLimiter::RateLimiter()
    : requests_in_window_(0),
      max_requests_per_minute_(DEFAULT_MAX_REQUESTS_PER_MINUTE),
      window_start_(std::chrono::steady_clock::now()),
      rate_limited_(false),
      retry_after_seconds_(0) {
}

bool RateLimiter::can_make_request() {
    std::lock_guard<std::mutex> lock(rate_limit_mutex_);
    
    if (rate_limited_) {
        auto now = std::chrono::steady_clock::now();
        if (now >= rate_limit_until_) {
            rate_limited_ = false;
            retry_after_seconds_ = 0;
        } else {
            return false;
        }
    }
    
    check_rate_limit_window();
    
    if (requests_in_window_ >= max_requests_per_minute_) {
        rate_limited_ = true;
        rate_limit_until_ = std::chrono::steady_clock::now() + 
                           std::chrono::seconds(RATE_LIMIT_WINDOW_SECONDS);
        Logger::get_instance().warn("Rate limit reached: " + 
                                    std::to_string(max_requests_per_minute_) + 
                                    " requests per minute");
        return false;
    }
    
    return true;
}

void RateLimiter::record_request() {
    std::lock_guard<std::mutex> lock(rate_limit_mutex_);
    check_rate_limit_window();
    requests_in_window_++;
}

void RateLimiter::handle_rate_limit_response(const std::string& response) {
    std::lock_guard<std::mutex> lock(rate_limit_mutex_);
    
    if (response.find("\"ok\":false") != std::string::npos &&
        response.find("rate_limited") != std::string::npos) {
        rate_limited_ = true;
        parse_retry_after(response);
        
        auto now = std::chrono::steady_clock::now();
        rate_limit_until_ = now + std::chrono::seconds(retry_after_seconds_);
        
        Logger::get_instance().warn("Rate limited by Slack API. Retry after: " + 
                                    std::to_string(retry_after_seconds_) + " seconds");
    }
}

int RateLimiter::get_retry_after_seconds() {
    std::lock_guard<std::mutex> lock(rate_limit_mutex_);
    if (rate_limited_) {
        auto now = std::chrono::steady_clock::now();
        auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
            rate_limit_until_ - now).count();
        return std::max(static_cast<long long>(remaining), 0LL);
    }
    return retry_after_seconds_;
}

bool RateLimiter::is_rate_limited() {
    std::lock_guard<std::mutex> lock(rate_limit_mutex_);
    if (rate_limited_) {
        auto now = std::chrono::steady_clock::now();
        return now < rate_limit_until_;
    }
    return false;
}

void RateLimiter::reset() {
    std::lock_guard<std::mutex> lock(rate_limit_mutex_);
    requests_in_window_ = 0;
    window_start_ = std::chrono::steady_clock::now();
    rate_limited_ = false;
    retry_after_seconds_ = 0;
}

bool RateLimiter::check_rate_limit_window() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        now - window_start_).count();
    
    if (elapsed >= RATE_LIMIT_WINDOW_SECONDS) {
        requests_in_window_ = 0;
        window_start_ = now;
        return true;
    }
    return false;
}

void RateLimiter::parse_retry_after(const std::string& response) {
    std::regex retry_regex(R"("retry_after":\s*(\d+))");
    std::smatch match;
    
    if (std::regex_search(response, match, retry_regex)) {
        retry_after_seconds_ = std::stoi(match[1].str());
    } else {
        retry_after_seconds_ = RATE_LIMIT_WINDOW_SECONDS;
    }
}
