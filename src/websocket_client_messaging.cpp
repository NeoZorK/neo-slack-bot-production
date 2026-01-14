/**
 * @file websocket_client_messaging.cpp
 * @brief WebSocket client message handling
 * 
 * Handles sending and receiving WebSocket messages, ping/pong frames.
 */

#include "websocket_client.hpp"
#include "websocket_frame.hpp"
#include "logger.hpp"
#include <openssl/ssl.h>
#include <openssl/err.h>

bool WebSocketClient::send_message(const std::string& message) {
    if (!connected_ || socket_fd_ < 0 || !ssl_) {
        Logger::get_instance().error("Cannot send message: not connected");
        return false;
    }
    
    auto frame_vec = websocket_frame::encode_text_frame(message);
    const unsigned char* frame_data = frame_vec.data();
    int frame_len = static_cast<int>(frame_vec.size());
    
    int total_sent = 0;
    while (total_sent < frame_len) {
        int sent = SSL_write(static_cast<SSL*>(ssl_), frame_data + total_sent, 
                             frame_len - total_sent);
        
        if (sent <= 0) {
            int err = SSL_get_error(static_cast<SSL*>(ssl_), sent);
            if (err == SSL_ERROR_WANT_WRITE || err == SSL_ERROR_WANT_READ) {
                continue;
            }
            
            Logger::get_instance().error("Failed to send WebSocket message: SSL error " + 
                                        std::to_string(err));
            connected_ = false;
            return false;
        }
        
        total_sent += sent;
    }
    
    return true;
}

bool WebSocketClient::receive_message(std::string& message) {
    if (!connected_ || socket_fd_ < 0 || !ssl_) {
        return false;
    }
    
    // Check if SSL has pending data first (most reliable for SSL)
    int ssl_pending = SSL_pending(static_cast<SSL*>(ssl_));
    
    // If no SSL pending data, check socket readiness with select()
    // This ensures we don't call SSL_read() unnecessarily
    if (ssl_pending == 0) {
        if (!has_data_available()) {
            return false; // No data available
        }
    }
    
    // Data is available - attempt to read
    // SSL_read() will return SSL_ERROR_WANT_READ if data not ready yet (shouldn't happen after select)
    unsigned char buffer[4096];
    int bytes_read = SSL_read(static_cast<SSL*>(ssl_), buffer, sizeof(buffer));
    
    if (bytes_read <= 0) {
        int err = SSL_get_error(static_cast<SSL*>(ssl_), bytes_read);
        
        if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
            // Normal - no data available yet in non-blocking mode
            return false;
        }
        
        if (err == SSL_ERROR_ZERO_RETURN) {
            Logger::get_instance().info("WebSocket connection closed by peer");
            connected_ = false;
            return false;
        }
        
        Logger::get_instance().error("Failed to receive WebSocket message: SSL error " + 
                                     std::to_string(err));
        connected_ = false;
        return false;
    }
    
    // Parse WebSocket frame
    std::string decoded = websocket_frame::decode_frame(buffer, bytes_read);
    
    if (!decoded.empty()) {
        message = decoded;
        return true;
    }
    
    // Check frame type by examining first byte
    if (bytes_read >= 1) {
        int opcode = buffer[0] & 0x0F;
        
        if (opcode == 0x8) { // Close frame
            Logger::get_instance().info("Received WebSocket close frame");
            
            // Send close frame response
            auto close_frame = websocket_frame::create_close_frame();
            SSL_write(static_cast<SSL*>(ssl_), close_frame.data(), static_cast<int>(close_frame.size()));
            
            connected_ = false;
            return false;
        } else if (opcode == 0x9) { // Ping frame
            auto pong_frame = websocket_frame::create_pong_frame();
            SSL_write(static_cast<SSL*>(ssl_), pong_frame.data(), static_cast<int>(pong_frame.size()));
            return false;
        } else if (opcode == 0xA) { // Pong frame
            return false;
        }
    }
    
    return false;
}

std::string WebSocketClient::receive_message() {
    std::string message;
    if (receive_message(message)) {
        return message;
    }
    return "";
}

void WebSocketClient::send_ping() {
    if (!connected_ || socket_fd_ < 0 || !ssl_) {
        return;
    }
    
    auto ping_frame = websocket_frame::create_ping_frame();
    SSL_write(static_cast<SSL*>(ssl_), ping_frame.data(), static_cast<int>(ping_frame.size()));
}

void WebSocketClient::send_pong() {
    if (!connected_ || socket_fd_ < 0 || !ssl_) {
        return;
    }
    
    auto pong_frame = websocket_frame::create_pong_frame();
    SSL_write(static_cast<SSL*>(ssl_), pong_frame.data(), static_cast<int>(pong_frame.size()));
}
