#ifndef RATE_LIMITER_HPP
#define RATE_LIMITER_HPP

#include <chrono>
#include <mutex>
#include <string>

/**
 * @brief Rate limiter for Slack API requests
 * 
 * Thread Safety: Thread-safe. All public methods can be called concurrently
 * from multiple threads. Uses mutex for synchronization.
 */
class RateLimiter {
public:
    /**
     * @brief Construct rate limiter
     */
    RateLimiter();
    
    /**
     * @brief Check if request can be made
     * @return true if request allowed, false if rate limited
     */
    bool can_make_request();
    
    /**
     * @brief Record that a request was made
     */
    void record_request();
    
    /**
     * @brief Handle rate limit response from API
     * @param response API response containing rate limit information
     */
    void handle_rate_limit_response(const std::string& response);
    
    /**
     * @brief Get retry-after seconds from rate limit
     * @return Seconds to wait before retrying
     */
    int get_retry_after_seconds();
    
    /**
     * @brief Check if currently rate limited
     * @return true if rate limited, false otherwise
     */
    bool is_rate_limited();
    
    /**
     * @brief Reset rate limiter state
     */
    void reset();

private:
    void parse_retry_after(const std::string& response);
    bool check_rate_limit_window();
    
    int requests_in_window_;
    int max_requests_per_minute_;
    std::chrono::steady_clock::time_point window_start_;
    std::chrono::steady_clock::time_point rate_limit_until_;
    std::mutex rate_limit_mutex_;
    bool rate_limited_;
    int retry_after_seconds_;
};

#endif // RATE_LIMITER_HPP
