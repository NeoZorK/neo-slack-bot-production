/**
 * @file test_security_input.cpp
 * @brief Security tests for input validation - main coordinator
 * 
 * Coordinates all input validation security tests.
 */

#include <iostream>

// Forward declarations
void test_security_input_attacks();
void test_security_input_format();
void test_security_input_sanitization();

// Export for test_security.cpp
void test_security_input_validation() {
    test_security_input_attacks();
    test_security_input_format();
    test_security_input_sanitization();
}
