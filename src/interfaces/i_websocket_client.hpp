#ifndef I_WEBSOCKET_CLIENT_HPP
#define I_WEBSOCKET_CLIENT_HPP

#include <string>
#include <functional>

/**
 * @brief WebSocket client interface
 * 
 * Provides abstraction for WebSocket client implementations.
 * Used for dependency inversion and testability.
 * 
 * Thread Safety: Implementation-dependent. Each implementation
 * should document its thread safety guarantees.
 */
class IWebSocketClient {
public:
    virtual ~IWebSocketClient() = default;
    
    /**
     * @brief Connect to WebSocket server
     * @return true if connection successful, false otherwise
     */
    virtual bool connect() = 0;
    
    /**
     * @brief Disconnect from WebSocket server
     */
    virtual void disconnect() = 0;
    
    /**
     * @brief Run event loop
     * 
     * Starts the main event loop for receiving and processing messages.
     * Blocks until disconnect() is called or connection is lost.
     */
    virtual void run() = 0;
    
    /**
     * @brief Check if currently connected
     * @return true if connected, false otherwise
     */
    virtual bool is_connected() const = 0;
    
    /**
     * @brief Set message handler callback
     * @param handler Function to call when message is received
     */
    virtual void set_message_handler(std::function<void(const std::string&)> handler) = 0;
    
    /**
     * @brief Set error handler callback
     * @param handler Function to call when error occurs
     */
    virtual void set_error_handler(std::function<void(const std::string&)> handler) = 0;
    
    /**
     * @brief Send message through WebSocket
     * @param message Message to send (JSON string)
     * @return true if message sent successfully, false otherwise
     */
    virtual bool send_message(const std::string& message) = 0;
};

#endif // I_WEBSOCKET_CLIENT_HPP
