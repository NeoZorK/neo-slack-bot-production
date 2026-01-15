#ifndef WEBSOCKET_CLIENT_HPP
#define WEBSOCKET_CLIENT_HPP

#include <string>
#include <functional>
#include "settings.hpp"

/**
 * @brief WebSocket client for Slack Socket Mode connection
 * 
 * Thread Safety: NOT thread-safe. This class is designed for single-threaded use.
 * Methods should be called from the same thread that created the instance.
 * External synchronization required if accessed from multiple threads.
 */
class WebSocketClient {
public:
    /**
     * @brief Construct WebSocket client with bot token
     * @param token Slack bot token (xoxb-...)
     */
    explicit WebSocketClient(const std::string& token);
    
    /**
     * @brief Construct WebSocket client with bot token and settings
     * @param token Slack bot token (xoxb-...)
     * @param settings Application settings for connection configuration
     */
    WebSocketClient(const std::string& token, const Settings& settings);
    
    /**
     * @brief Destructor - closes connection if still open
     */
    ~WebSocketClient();
    
    /**
     * @brief Connect to Slack Socket Mode
     * 
     * Establishes WebSocket connection to Slack's Socket Mode endpoint.
     * Authenticates using bot token and waits for hello event.
     * 
     * @return true if connection successful, false otherwise
     * 
     * @note Connection is asynchronous, check is_connected() after calling
     * 
     * @see disconnect()
     * @see is_connected()
     */
    bool connect();
    
    /**
     * @brief Disconnect from Slack Socket Mode
     * 
     * Closes WebSocket connection gracefully. Safe to call even if not connected.
     */
    void disconnect();
    
    /**
     * @brief Run event loop
     * 
     * Starts the main event loop for receiving and processing messages.
     * Blocks until disconnect() is called or connection is lost.
     * 
     * @note Should be called after connect()
     */
    void run();
    
    /**
     * @brief Check if currently connected
     * @return true if connected, false otherwise
     */
    bool is_connected() const;
    
    /**
     * @brief Set message handler callback
     * @param handler Function to call when message is received
     */
    void set_message_handler(std::function<void(const std::string&)> handler);
    
    /**
     * @brief Set error handler callback
     * @param handler Function to call when error occurs
     */
    void set_error_handler(std::function<void(const std::string&)> handler);
    
    /**
     * @brief Send message through WebSocket
     * @param message Message to send (JSON string)
     * @return true if message sent successfully, false otherwise
     */
    bool send_message(const std::string& message);
    
    /**
     * @brief Apply new settings to connection
     * @param settings Settings to apply
     */
    void apply_settings(const Settings& settings);
    
    /**
     * @brief Set custom WebSocket URL
     * 
     * Sets the WebSocket URL obtained from apps.connections.open API call.
     * This URL is dynamic and changes for each connection session.
     * 
     * @param url WebSocket URL from Slack API
     */
    void set_websocket_url(const std::string& url);
    
    /**
     * @brief Set should_stop flag for graceful shutdown
     * 
     * Thread-safe method that can be called from signal handler.
     * When set to true, run() will exit gracefully.
     * 
     * @param stop true to stop, false to continue
     */
    void set_should_stop(bool stop);

private:
    std::string token_;
    std::string websocket_url_; // Dynamic URL from apps.connections.open
    bool connected_;
    bool should_stop_;
    bool auto_reconnect_;
    int reconnect_attempts_;
    int max_reconnect_attempts_;
    int ping_interval_sec_;
    int retry_delay_sec_;
    void* ssl_ctx_;
    void* ssl_;
    int socket_fd_;
    
    std::function<void(const std::string&)> message_handler_;
    std::function<void(const std::string&)> error_handler_;
    
    bool establish_tcp_connection();
    bool establish_tls_connection();
    bool perform_websocket_handshake();
    std::string generate_websocket_key(); // Keep for backward compatibility
    bool has_data_available() const;
    bool receive_message(std::string& message);
    std::string receive_message();
    void send_ping();
    void send_pong();
    void handle_message(const std::string& message);
    void handle_error(const std::string& error);
    bool attempt_reconnect();
};

#endif // WEBSOCKET_CLIENT_HPP
