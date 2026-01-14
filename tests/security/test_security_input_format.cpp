/**
 * @file test_security_input_format.cpp
 * @brief Security tests for input validation - format validation
 * 
 * Tests format validation for tokens, channel IDs, user IDs, and message length.
 */

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "../../src/input_validator.hpp"

void test_token_format_validation_security() {
    std::vector<std::string> malicious_tokens = {
        "",
        "xoxb-",
        "xoxb-" + std::string(300, 'A'),
        "xoxb-1234567890-1234567890123-abc<script>",
        "xoxb-1234567890-1234567890123-abc; rm -rf",
        "xoxb-1234567890-1234567890123-abc' OR '1'='1",
        "xoxb-1234567890-1234567890123-abc" + std::string(1, '\0') + "null",
        "xoxb-1234567890-1234567890123-abc/../../etc/passwd",
        "invalid-prefix-1234567890",
        "XOXB-1234567890-1234567890123-ABC",
    };
    
    for (const auto& token : malicious_tokens) {
        assert(!InputValidator::is_valid_bot_token(token));
        assert(!InputValidator::is_valid_app_token(token));
    }
    
    std::string valid_token = "xoxb-your-bot-token-here";
    assert(InputValidator::is_valid_bot_token(valid_token));
    
    std::cout << "  ✓ Token format validation security test passed" << std::endl;
}

void test_channel_id_validation_security() {
    std::vector<std::string> malicious_channel_ids = {
        "",
        "C",
        "C123456789012345",
        "C12345678<script>",
        "C12345678; rm -rf",
        "C12345678' OR '1'='1",
        "C12345678" + std::string(1, '\0') + "null",
        "C12345678/../../etc/passwd",
        "X12345678",
        "c12345678",
        "C1234567-",
        "C1234567@",
    };
    
    for (const auto& channel_id : malicious_channel_ids) {
        assert(!InputValidator::is_valid_channel_id(channel_id));
    }
    
    assert(InputValidator::is_valid_channel_id("C12345678"));
    assert(InputValidator::is_valid_channel_id("D12345678"));
    assert(InputValidator::is_valid_channel_id("G12345678"));
    
    std::cout << "  ✓ Channel ID validation security test passed" << std::endl;
}

void test_user_id_validation_security() {
    std::vector<std::string> malicious_user_ids = {
        "",
        "U",
        "U123456789012345",
        "U1234567890<script>",
        "U1234567890; rm -rf",
        "U1234567890' OR '1'='1",
        "U1234567890" + std::string(1, '\0') + "null",
        "U1234567890/../../etc/passwd",
        "X1234567890",
        "u1234567890",
        "U123456789-",
        "U123456789@",
    };
    
    for (const auto& user_id : malicious_user_ids) {
        assert(!InputValidator::is_valid_user_id(user_id));
    }
    
    assert(InputValidator::is_valid_user_id("U1234567890"));
    
    std::cout << "  ✓ User ID validation security test passed" << std::endl;
}

void test_message_text_length_limit_security() {
    std::string normal_message(1000, 'A');
    assert(InputValidator::is_valid_message_text(normal_message));
    
    std::string max_message(4000, 'A');
    assert(InputValidator::is_valid_message_text(max_message));
    
    std::string oversized_message(4001, 'A');
    assert(!InputValidator::is_valid_message_text(oversized_message));
    
    std::string huge_message(100000, 'A');
    assert(!InputValidator::is_valid_message_text(huge_message));
    
    std::cout << "  ✓ Message text length limit security test passed" << std::endl;
}

void test_security_input_format() {
    test_token_format_validation_security();
    test_channel_id_validation_security();
    test_user_id_validation_security();
    test_message_text_length_limit_security();
}
