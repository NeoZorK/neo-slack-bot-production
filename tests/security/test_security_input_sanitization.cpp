/**
 * @file test_security_input_sanitization.cpp
 * @brief Security tests for input validation - sanitization and memory safety
 * 
 * Tests sanitization functions and memory safety.
 */

#include <iostream>
#include <cassert>
#include <string>
#include "../../src/input_validator.hpp"

void test_sanitization_security() {
    std::string channel_with_invalid = "C1234567<script>";
    auto sanitized_channel = InputValidator::sanitize_channel_id(channel_with_invalid);
    assert(!sanitized_channel.has_value());
    
    std::string user_with_invalid = "U123456789<script>";
    auto sanitized_user = InputValidator::sanitize_user_id(user_with_invalid);
    assert(!sanitized_user.has_value());
    
    std::string message_with_null = "Hello";
    message_with_null += '\0';
    message_with_null += "World";
    message_with_null += '\0';
    message_with_null += "Test";
    auto sanitized_message = InputValidator::sanitize_message_text(message_with_null);
    assert(sanitized_message.has_value());
    assert(sanitized_message->find('\0') == std::string::npos);
    assert(sanitized_message->find("Hello") != std::string::npos);
    
    std::cout << "  ✓ Sanitization security test passed" << std::endl;
}

void test_memory_safety_security() {
    std::string valid_channel = "C12345678";
    std::string valid_user = "U1234567890";
    
    assert(InputValidator::is_valid_channel_id(valid_channel));
    assert(InputValidator::is_valid_user_id(valid_user));
    
    for (size_t len = 0; len < 20; ++len) {
        std::string test_str(len, 'A');
        (void)InputValidator::is_valid_channel_id(test_str);
        (void)InputValidator::is_valid_user_id(test_str);
        (void)InputValidator::is_valid_bot_token(test_str);
    }
    
    std::cout << "  ✓ Memory safety security test passed" << std::endl;
}

void test_security_input_sanitization() {
    test_sanitization_security();
    test_memory_safety_security();
}
