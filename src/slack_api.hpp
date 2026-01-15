#ifndef SLACK_API_HPP
#define SLACK_API_HPP

#include <string>
#include <memory>
#include "interfaces/i_http_client.hpp"

class RateLimiter;
class ErrorRecovery;

/**
 * @brief Slack API wrapper
 * 
 * Thread Safety: Thread-safe for read operations (get_* methods).
 * send_message() uses internal rate limiter and error recovery which are
 * thread-safe. Multiple threads can call API methods concurrently.
 */
class SlackAPI {
public:
    /**
     * @brief Construct Slack API wrapper
     * @param token Slack bot token (xoxb-...)
     */
    explicit SlackAPI(const std::string& token);
    
    /**
     * @brief Construct Slack API wrapper with custom HTTP client (dependency injection)
     * @param token Slack bot token
     * @param http_client Custom HTTP client (ownership transferred)
     */
    SlackAPI(const std::string& token, std::unique_ptr<IHttpClient> http_client);
    
    /**
     * @brief Destructor
     */
    ~SlackAPI();
    
    /**
     * @brief Send message to Slack channel
     * @param channel Channel ID (e.g., "C1234567890")
     * @param text Message text to send
     * @return true if message sent successfully, false otherwise
     */
    bool send_message(const std::string& channel, const std::string& text);
    
    /**
     * @brief Send message to Slack channel with error details
     * @param channel Channel ID (e.g., "C1234567890")
     * @param text Message text to send
     * @param error_response Output parameter for error response if failed
     * @return true if message sent successfully, false otherwise
     */
    bool send_message(const std::string& channel, const std::string& text, std::string& error_response);
    
    /**
     * @brief Get channel information
     * @param channel_id Channel ID to query
     * @param info Output parameter for channel info (JSON string)
     * @return true if request successful, false otherwise
     */
    bool get_channel_info(const std::string& channel_id, std::string& info);
    
    /**
     * @brief Get user information
     * @param user_id User ID to query
     * @param info Output parameter for user info (JSON string)
     * @return true if request successful, false otherwise
     */
    bool get_user_info(const std::string& user_id, std::string& info);
    
    /**
     * @brief Get Socket Mode WebSocket URL
     * 
     * Calls apps.connections.open API to get dynamic WebSocket URL.
     * 
     * @param websocket_url Output parameter for WebSocket URL
     * @return true if request successful, false otherwise
     */
    bool get_socket_mode_url(std::string& websocket_url);
    
    /**
     * @brief Join Slack channel
     * @param channel_id Channel ID to join
     * @return true if join successful, false otherwise
     */
    bool join_channel(const std::string& channel_id);
    
    /**
     * @brief Get bot user ID
     * 
     * Retrieves the bot's own user ID from auth.test API.
     * 
     * @param bot_user_id Output parameter for bot user ID
     * @return true if request successful, false otherwise
     */
    bool get_bot_user_id(std::string& bot_user_id);

private:
    std::string token_;
    std::unique_ptr<IHttpClient> http_client_;
    std::string api_base_url_;
    std::unique_ptr<RateLimiter> rate_limiter_;
    std::unique_ptr<ErrorRecovery> error_recovery_;
    
    std::string build_api_url(const std::string& endpoint);
    bool make_api_request(const std::string& url, const std::string& data, 
                         std::string& response, bool is_post = true);
    static std::string escape_json_string(const std::string& str);
};

#endif // SLACK_API_HPP
