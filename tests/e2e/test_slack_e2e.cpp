/**
 * @file test_slack_e2e.cpp
 * @brief E2E tests for Slack - main coordinator
 * 
 * Coordinates all E2E tests for Slack integration.
 */

#include <iostream>
#include <atomic>
#include <vector>
#include <string>

// Global state for E2E tests
std::atomic<int> g_messages_received{0};
std::atomic<int> g_messages_sent{0};
std::atomic<int> g_notifications_triggered{0};
std::vector<std::string> g_received_messages;
std::vector<std::string> g_sent_messages;

// Forward declarations
void test_slack_e2e_message_parsing();
void test_slack_channel_filtering();
void test_slack_keyword_filtering();
void test_slack_command_handling();
void test_slack_multiple_bots_simulation();
void test_slack_notification_flow();
void test_slack_message_sending();

// Export for test_main.cpp
void test_slack_e2e() {
    std::cout << "\n=== Slack End-to-End Tests ===" << std::endl;
    
    test_slack_e2e_message_parsing();
    test_slack_channel_filtering();
    test_slack_keyword_filtering();
    test_slack_command_handling();
    test_slack_multiple_bots_simulation();
    test_slack_notification_flow();
    
    test_slack_message_sending();
    
    std::cout << "\nE2E Test Summary:" << std::endl;
    std::cout << "  Messages sent: " << g_messages_sent.load() << std::endl;
    std::cout << "  Messages received: " << g_received_messages.size() << std::endl;
    std::cout << "  Notifications triggered: " << g_notifications_triggered.load() << std::endl;
    
    std::cout << "All Slack E2E tests passed!" << std::endl;
}
