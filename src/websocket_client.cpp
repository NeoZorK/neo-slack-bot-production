/**
 * @file websocket_client.cpp
 * @brief WebSocket client implementation
 * 
 * Main WebSocket client class implementation.
 * Connection logic is in websocket_client_connection.cpp
 * Message handling is in websocket_client_messaging.cpp
 */

#include "websocket_client.hpp"
#include "websocket_connection.hpp"
#include "logger.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/select.h>
#endif

using namespace std::chrono;

// External reference to global running flag
extern std::atomic<bool> g_running;

WebSocketClient::WebSocketClient(const std::string& token)
    : token_(token), websocket_url_(), connected_(false), should_stop_(false),
      auto_reconnect_(true), reconnect_attempts_(0), max_reconnect_attempts_(10),
      ping_interval_sec_(30), retry_delay_sec_(1), ssl_ctx_(nullptr), ssl_(nullptr),
      socket_fd_(-1) {
}

WebSocketClient::~WebSocketClient() {
    // Safe cleanup - disconnect() checks if already disconnected
    disconnect();
}

void WebSocketClient::run() {
    if (!connect()) {
        Logger::get_instance().error("Initial connection failed, entering reconnection loop");
    }
    
    // After connection, immediately try to read any pending data (like hello events)
    // This ensures we don't miss events that arrive right after connection
    int immediate_reads = 0;
    const int MAX_IMMEDIATE_READS = 10;
    
    // Check both should_stop_ and g_running for graceful shutdown
    while (!should_stop_ && g_running.load()) {
        // Check shutdown flags before any blocking operations
        if (should_stop_ || !g_running.load()) {
            break;
        }
        
        if (!connected_) {
            if (!auto_reconnect_) {
                break;
            }
            
            // Check shutdown flags before reconnection attempt
            if (should_stop_ || !g_running.load()) {
                break;
            }
            
            if (!attempt_reconnect()) {
                // Check again after reconnection attempt
                if (should_stop_ || !g_running.load()) {
                    break;
                }
                continue;
            }
            // After reconnection, reset immediate read counter
            immediate_reads = 0;
        }
        
        // Check shutdown flags before receiving message
        if (should_stop_ || !g_running.load()) {
            break;
        }
        
        // Try to receive message - always attempt to read
        // SSL_read() will return SSL_ERROR_WANT_READ if no data is available (non-blocking)
        std::string message;
        bool received = receive_message(message);
        
        if (received && !message.empty()) {
            if (message_handler_) {
                message_handler_(message);
            } else {
                // Handler not set - this should not happen in normal operation
                Logger::get_instance().warn("Message received but handler not set");
            }
            // Reset immediate read counter when we receive data
            immediate_reads = 0;
        } else if (!connected_) {
            // Connection lost, will reconnect in next iteration
            continue;
        } else {
            // No data available (SSL_ERROR_WANT_READ) - this is normal in non-blocking mode
            // Immediately after connection, try reading multiple times without sleep
            // to catch hello events that arrive right after connection
            if (immediate_reads < MAX_IMMEDIATE_READS) {
                immediate_reads++;
                // Don't sleep - try reading again immediately
                continue;
            }
            
            // After initial reads, sleep briefly to avoid busy-waiting
            if (should_stop_ || !g_running.load()) {
                break;
            }
            std::this_thread::sleep_for(milliseconds(10));
        }
    }
    
    // Disconnect before exiting run() - safe to call multiple times
    disconnect();
}

bool WebSocketClient::is_connected() const {
    return connected_ && socket_fd_ >= 0;
}

void WebSocketClient::set_message_handler(std::function<void(const std::string&)> handler) {
    message_handler_ = handler;
}

void WebSocketClient::set_websocket_url(const std::string& url) {
    websocket_url_ = url;
}

WebSocketClient::WebSocketClient(const std::string& token, const Settings& settings)
    : WebSocketClient(token) {
    apply_settings(settings);
}

void WebSocketClient::set_error_handler(std::function<void(const std::string&)> handler) {
    error_handler_ = handler;
}

void WebSocketClient::apply_settings(const Settings& settings) {
    max_reconnect_attempts_ = settings.connection.max_retries;
    ping_interval_sec_ = settings.connection.ping_interval_seconds;
    retry_delay_sec_ = settings.connection.retry_delay_seconds;
}

void WebSocketClient::handle_message(const std::string& message) {
    if (message_handler_) {
        message_handler_(message);
    }
}

void WebSocketClient::handle_error(const std::string& error) {
    Logger::get_instance().error(error);
    if (error_handler_) {
        error_handler_(error);
    }
}

bool WebSocketClient::has_data_available() const {
    if (!connected_ || socket_fd_ < 0) {
        return false;
    }
    
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(socket_fd_, &read_fds);
    
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    
    return select(socket_fd_ + 1, &read_fds, nullptr, nullptr, &timeout) > 0;
}

std::string WebSocketClient::generate_websocket_key() {
    return websocket_connection::generate_websocket_key();
}

void WebSocketClient::set_should_stop(bool stop) {
    should_stop_ = stop;
}
