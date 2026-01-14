#ifndef WEBSOCKET_FRAME_HPP
#define WEBSOCKET_FRAME_HPP

#include <string>
#include <vector>

/**
 * @brief WebSocket frame encoding/decoding utilities
 * 
 * Handles WebSocket protocol frame construction and parsing
 * according to RFC 6455.
 */
namespace websocket_frame {
    /**
     * @brief Encode message as WebSocket frame
     * @param message Message to encode
     * @return Encoded frame bytes
     */
    std::vector<unsigned char> encode_text_frame(const std::string& message);
    
    /**
     * @brief Decode WebSocket frame to message
     * @param data Raw frame data
     * @param length Data length
     * @return Decoded message or empty if invalid
     */
    std::string decode_frame(const unsigned char* data, int length);
    
    /**
     * @brief Create ping frame
     * @return Ping frame bytes
     */
    std::vector<unsigned char> create_ping_frame();
    
    /**
     * @brief Create pong frame
     * @return Pong frame bytes
     */
    std::vector<unsigned char> create_pong_frame();
    
    /**
     * @brief Create close frame
     * @return Close frame bytes
     */
    std::vector<unsigned char> create_close_frame();
}

#endif // WEBSOCKET_FRAME_HPP
