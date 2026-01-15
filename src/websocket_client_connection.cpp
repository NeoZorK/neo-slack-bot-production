/**
 * @file websocket_client_connection.cpp
 * @brief WebSocket client connection management
 * 
 * Handles connection establishment, disconnection, and reconnection logic.
 */

#include "websocket_client.hpp"
#include "websocket_connection.hpp"
#include "logger.hpp"
#include <unistd.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <atomic>

using namespace std::chrono;

// External reference to global running flag
extern std::atomic<bool> g_running;

namespace {
    constexpr int MAX_RECONNECT_ATTEMPTS = 10;
    constexpr int INITIAL_RECONNECT_DELAY_MS = 1000;
    constexpr int MAX_RECONNECT_DELAY_MS = 60000;
}

namespace websocket_client_connection {
    /**
     * @brief Parse WebSocket URL into components
     * @param url WebSocket URL (wss://host:port/path)
     * @param host Output host name
     * @param port Output port number
     * @param path Output path
     * @return true if parsing successful
     */
    bool parse_url(const std::string& url, std::string& host, int& port, std::string& path) {
        if (url.find("wss://") != 0) {
            Logger::get_instance().error("Invalid WebSocket URL format: " + url);
            return false;
        }
        
        size_t host_start = 6; // Skip "wss://"
        size_t path_start = url.find('/', host_start);
        size_t port_start = url.find(':', host_start);
        
        if (port_start != std::string::npos && (path_start == std::string::npos || port_start < path_start)) {
            host = url.substr(host_start, port_start - host_start);
            size_t port_end = (path_start != std::string::npos) ? path_start : url.length();
            port = std::stoi(url.substr(port_start + 1, port_end - port_start - 1));
            path = (path_start != std::string::npos) ? url.substr(path_start) : "/";
        } else {
            if (path_start != std::string::npos) {
                host = url.substr(host_start, path_start - host_start);
                path = url.substr(path_start);
            } else {
                host = url.substr(host_start);
                path = "/";
            }
            port = 443;
        }
        
        return true;
    }
}

bool WebSocketClient::connect() {
    if (connected_) {
        Logger::get_instance().warn("Already connected");
        return true;
    }
    
    std::string url = websocket_url_.empty() ? 
        "wss://wss-primary.slack.com/link/?ticket=" + token_ : 
        websocket_url_;
    
    Logger::get_instance().info("Connecting to: " + url);
    
    std::string host, path;
    int port = 443;
    
    if (!websocket_client_connection::parse_url(url, host, port, path)) {
        return false;
    }
    
    // Establish TCP connection
    if (!websocket_connection::establish_tcp_connection(socket_fd_, host.c_str(), port)) {
        Logger::get_instance().error("Failed to establish TCP connection");
        return false;
    }
    
    // Establish TLS connection
    if (!websocket_connection::establish_tls_connection(ssl_ctx_, ssl_, socket_fd_)) {
        Logger::get_instance().error("Failed to establish TLS connection");
        if (socket_fd_ >= 0) {
            close(socket_fd_);
            socket_fd_ = -1;
        }
        return false;
    }
    
    // Perform WebSocket handshake
    websocket_connection::ConnectionParams params;
    params.host = host.c_str();
    params.path = path.c_str();
    params.port = port;
    params.token = ""; // Dynamic URL uses ticket in URL, not Authorization header
    
    if (!websocket_connection::perform_websocket_handshake(ssl_, params)) {
        Logger::get_instance().error("Failed to perform WebSocket handshake");
        websocket_connection::cleanup_connection(ssl_ctx_, ssl_, socket_fd_);
        return false;
    }
    
    connected_ = true;
    reconnect_attempts_ = 0;
    
    Logger::get_instance().info("Successfully connected to Slack Socket Mode");
    return true;
}

void WebSocketClient::disconnect() {
    // Prevent double cleanup
    if (!connected_ && socket_fd_ < 0 && !ssl_ && !ssl_ctx_) {
        return; // Already disconnected
    }
    
    should_stop_ = true;
    auto_reconnect_ = false;
    
    websocket_connection::cleanup_connection(ssl_ctx_, ssl_, socket_fd_);
    
    connected_ = false;
    Logger::get_instance().info("Disconnected from Slack Socket Mode");
}

bool WebSocketClient::attempt_reconnect() {
    if (should_stop_) {
        return false;
    }
    
    reconnect_attempts_++;
    
    if (reconnect_attempts_ > MAX_RECONNECT_ATTEMPTS) {
        reconnect_attempts_ = 0; // Reset and try again
    }
    
    int delay_ms = std::min(
        INITIAL_RECONNECT_DELAY_MS * (1 << (reconnect_attempts_ - 1)),
        MAX_RECONNECT_DELAY_MS
    );
    
    Logger::get_instance().info("Reconnecting in " + std::to_string(delay_ms) + "ms (attempt " + 
                                 std::to_string(reconnect_attempts_) + ")");
    
    auto start = steady_clock::now();
    while (duration_cast<milliseconds>(steady_clock::now() - start).count() < delay_ms) {
        if (should_stop_ || !g_running.load()) {
            return false;
        }
        // Use shorter sleep intervals for faster shutdown response
        std::this_thread::sleep_for(milliseconds(50));
    }
    
    return connect();
}

bool WebSocketClient::establish_tcp_connection() {
    std::string url = websocket_url_.empty() ? 
        "wss://wss-primary.slack.com/link/?ticket=" + token_ : 
        websocket_url_;
    
    std::string host;
    int port = 443;
    std::string path;
    
    if (!websocket_client_connection::parse_url(url, host, port, path)) {
        return false;
    }
    
    return websocket_connection::establish_tcp_connection(socket_fd_, host.c_str(), port);
}

bool WebSocketClient::establish_tls_connection() {
    return websocket_connection::establish_tls_connection(ssl_ctx_, ssl_, socket_fd_);
}

bool WebSocketClient::perform_websocket_handshake() {
    std::string url = websocket_url_.empty() ? 
        "wss://wss-primary.slack.com/link/?ticket=" + token_ : 
        websocket_url_;
    
    std::string host, path;
    int port = 443;
    
    if (!websocket_client_connection::parse_url(url, host, port, path)) {
        return false;
    }
    
    websocket_connection::ConnectionParams params;
    params.host = host.c_str();
    params.path = path.c_str();
    params.port = port;
    params.token = "";
    
    return websocket_connection::perform_websocket_handshake(ssl_, params);
}
