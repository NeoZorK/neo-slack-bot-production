/**
 * @file test_slack_e2e_api.cpp
 * @brief E2E tests for Slack API - real API calls
 * 
 * Tests actual Slack API integration (requires tokens).
 */

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <atomic>
#include <cstdlib>
#include <chrono>
#include "../../src/slack_api.hpp"
#include "../../src/input_validator.hpp"

extern std::atomic<int> g_messages_sent;
extern std::vector<std::string> g_sent_messages;

void test_slack_message_sending() {
    std::cout << "\n--- Slack Message Sending Test ---" << std::endl;
    
    const char* bot_token = std::getenv("SLACK_BOT_TOKEN");
    if (!bot_token) {
        std::cout << "  ⚠ SKIPPED: SLACK_BOT_TOKEN not set (set it to run E2E tests)" << std::endl;
        return;
    }
    
    if (!InputValidator::is_valid_bot_token(bot_token)) {
        std::cout << "  ⚠ SKIPPED: Invalid bot token format" << std::endl;
        return;
    }
    
    const char* test_channel = std::getenv("SLACK_TEST_CHANNEL");
    if (!test_channel) {
        std::cout << "  ⚠ SKIPPED: SLACK_TEST_CHANNEL not set" << std::endl;
        std::cout << "  Set SLACK_TEST_CHANNEL=C12345678 to run this test" << std::endl;
        return;
    }
    
    if (!InputValidator::is_valid_channel_id(test_channel)) {
        std::cout << "  ⚠ SKIPPED: Invalid channel ID format" << std::endl;
        return;
    }
    
    SlackAPI api(bot_token);
    
    std::string test_message = "E2E Test Message " + 
        std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    
    std::cout << "  Sending test message to channel: " << test_channel << std::endl;
    bool sent = api.send_message(test_channel, test_message);
    
    if (sent) {
        g_messages_sent++;
        g_sent_messages.push_back(test_message);
        std::cout << "  ✓ Message sent successfully" << std::endl;
    } else {
        std::cout << "  ✗ Failed to send message" << std::endl;
    }
    
    assert(sent);
    
    std::string special_message = "Test: \"quotes\", 'apostrophes', & symbols! @here #channel";
    sent = api.send_message(test_channel, special_message);
    assert(sent);
    std::cout << "  ✓ Message with special characters sent" << std::endl;
    
    std::string long_message(3900, 'A');
    sent = api.send_message(test_channel, long_message);
    assert(sent);
    std::cout << "  ✓ Long message sent" << std::endl;
    
    sent = api.send_message(test_channel, "");
    assert(!sent);
    std::cout << "  ✓ Empty message correctly rejected" << std::endl;
}
