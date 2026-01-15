#include <iostream>
#include <cassert>
#include "../src/input_validator.hpp"

void test_token_validation() {
    std::string valid_token = "xoxb-BOT_TOKEN_PLACEHOLDER";
    assert(InputValidator::is_valid_bot_token(valid_token));
    assert(!InputValidator::is_valid_bot_token("invalid-token"));
    assert(!InputValidator::is_valid_bot_token("xoxb-"));
    assert(!InputValidator::is_valid_bot_token(""));
    assert(!InputValidator::is_valid_bot_token("xoxa-1234567890"));
    
    std::cout << "  ✓ Token validation test passed" << std::endl;
}

void test_channel_id_validation() {
    assert(InputValidator::is_valid_channel_id("C12345678"));
    assert(InputValidator::is_valid_channel_id("D12345678"));
    assert(InputValidator::is_valid_channel_id("G12345678"));
    assert(!InputValidator::is_valid_channel_id("invalid"));
    assert(!InputValidator::is_valid_channel_id("C123"));
    assert(!InputValidator::is_valid_channel_id(""));
    
    std::cout << "  ✓ Channel ID validation test passed" << std::endl;
}

void test_user_id_validation() {
    assert(InputValidator::is_valid_user_id("U1234567890"));
    assert(!InputValidator::is_valid_user_id("invalid"));
    assert(!InputValidator::is_valid_user_id("U123"));
    assert(!InputValidator::is_valid_user_id(""));
    
    std::cout << "  ✓ User ID validation test passed" << std::endl;
}

void test_message_text_validation() {
    assert(InputValidator::is_valid_message_text("Hello"));
    assert(InputValidator::is_valid_message_text(""));
    assert(!InputValidator::is_valid_message_text(std::string(5000, 'a')));
    assert(InputValidator::is_valid_message_text("A"));
    assert(InputValidator::is_valid_message_text(std::string(4000, 'a')));
    
    std::cout << "  ✓ Message text validation test passed" << std::endl;
}

void test_sanitization() {
    auto channel = InputValidator::sanitize_channel_id("C12345678");
    assert(channel.has_value() && channel.value() == "C12345678");
    
    auto user = InputValidator::sanitize_user_id("U1234567890");
    assert(user.has_value() && user.value() == "U1234567890");
    
    auto text = InputValidator::sanitize_message_text("Test message");
    assert(text.has_value());
    
    auto invalid_channel = InputValidator::sanitize_channel_id("invalid");
    assert(!invalid_channel.has_value());
    
    std::cout << "  ✓ Sanitization test passed" << std::endl;
}

void test_input_validator() {
    std::cout << "Testing InputValidator..." << std::endl;
    
    test_token_validation();
    test_channel_id_validation();
    test_user_id_validation();
    test_message_text_validation();
    test_sanitization();
    
    std::cout << "InputValidator tests passed!" << std::endl;
}
