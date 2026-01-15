#include "websocket_frame.hpp"
#include <cstring>
#include <algorithm>
#include <random>
#include <chrono>

std::vector<unsigned char> websocket_frame::encode_text_frame(
    const std::string& message) {
    std::vector<unsigned char> frame;
    size_t payload_len = message.length();
    
    // Generate random masking key (RFC 6455 requires client to mask all frames)
    unsigned char masking_key[4];
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    for (int i = 0; i < 4; ++i) {
        masking_key[i] = static_cast<unsigned char>(dis(gen));
    }
    
    // Frame header: FIN=1, opcode=0x1 (text)
    frame.push_back(0x81);
    
    // Payload length with MASK bit set (0x80)
    if (payload_len < 126) {
        frame.push_back(static_cast<unsigned char>(payload_len | 0x80));
    } else if (payload_len < 65536) {
        frame.push_back(126 | 0x80);
        frame.push_back(static_cast<unsigned char>((payload_len >> 8) & 0xFF));
        frame.push_back(static_cast<unsigned char>(payload_len & 0xFF));
    } else {
        frame.push_back(127 | 0x80);
        for (int i = 7; i >= 0; --i) {
            frame.push_back(static_cast<unsigned char>((payload_len >> (i * 8)) & 0xFF));
        }
    }
    
    // Add masking key
    frame.insert(frame.end(), masking_key, masking_key + 4);
    
    // Add masked payload
    for (size_t i = 0; i < payload_len; ++i) {
        frame.push_back(message[i] ^ masking_key[i % 4]);
    }
    
    return frame;
}

std::string websocket_frame::decode_frame(const unsigned char* data, int length) {
    if (length < 2) {
        return "";
    }
    
    // bool fin = (data[0] & 0x80) != 0; // Reserved for future use
    int opcode = data[0] & 0x0F;
    bool masked = (data[1] & 0x80) != 0;
    uint64_t payload_len = data[1] & 0x7F;
    
    int offset = 2;
    if (payload_len == 126) {
        if (length < 4) return "";
        payload_len = (static_cast<uint64_t>(data[2]) << 8) | data[3];
        offset = 4;
    } else if (payload_len == 127) {
        if (length < 10) return "";
        payload_len = 0;
        for (int i = 0; i < 8; ++i) {
            payload_len = (payload_len << 8) | data[2 + i];
        }
        offset = 10;
    }
    
    if (opcode == 0x8) {
        return "";
    }
    
    if (opcode == 0x9 || opcode == 0xA) {
        return "";
    }
    
    if (masked && length >= offset + 4) {
        unsigned char masking_key[4];
        std::memcpy(masking_key, data + offset, 4);
        offset += 4;
        
        if (length >= offset + static_cast<int>(payload_len)) {
            std::string message;
            message.resize(payload_len);
            for (uint64_t i = 0; i < payload_len; ++i) {
                message[i] = data[offset + i] ^ masking_key[i % 4];
            }
            return message;
        }
    } else if (!masked && length >= offset + static_cast<int>(payload_len)) {
        return std::string(reinterpret_cast<const char*>(data + offset), 
                          payload_len);
    }
    
    return "";
}

std::vector<unsigned char> websocket_frame::create_ping_frame() {
    std::vector<unsigned char> frame;
    
    // Generate random masking key
    unsigned char masking_key[4];
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    for (int i = 0; i < 4; ++i) {
        masking_key[i] = static_cast<unsigned char>(dis(gen));
    }
    
    // Frame header: FIN=1, opcode=0x9 (ping), MASK=1, payload_len=0
    frame.push_back(0x89);
    frame.push_back(0x80); // MASK bit set, payload length = 0
    
    // Add masking key (even for empty payload, RFC 6455 requires it)
    frame.insert(frame.end(), masking_key, masking_key + 4);
    
    return frame;
}

std::vector<unsigned char> websocket_frame::create_pong_frame() {
    std::vector<unsigned char> frame;
    
    // Generate random masking key
    unsigned char masking_key[4];
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    for (int i = 0; i < 4; ++i) {
        masking_key[i] = static_cast<unsigned char>(dis(gen));
    }
    
    // Frame header: FIN=1, opcode=0xA (pong), MASK=1, payload_len=0
    frame.push_back(0x8A);
    frame.push_back(0x80); // MASK bit set, payload length = 0
    
    // Add masking key
    frame.insert(frame.end(), masking_key, masking_key + 4);
    
    return frame;
}

std::vector<unsigned char> websocket_frame::create_close_frame() {
    std::vector<unsigned char> frame;
    
    // Generate random masking key
    unsigned char masking_key[4];
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    for (int i = 0; i < 4; ++i) {
        masking_key[i] = static_cast<unsigned char>(dis(gen));
    }
    
    // Frame header: FIN=1, opcode=0x8 (close), MASK=1, payload_len=0
    // Note: We can optionally include close code, but empty close frame is also valid
    frame.push_back(0x88);
    frame.push_back(0x80); // MASK bit set, payload length = 0
    
    // Add masking key
    frame.insert(frame.end(), masking_key, masking_key + 4);
    
    return frame;
}
