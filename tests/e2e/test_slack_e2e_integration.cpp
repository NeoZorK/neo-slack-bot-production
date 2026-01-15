/**
 * @file test_slack_e2e_integration.cpp
 * @brief E2E tests for Slack - integration tests
 * 
 * Tests multiple bots simulation and notification flow.
 */

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include "../../src/event_parser.hpp"
#include "../../src/message_filter.hpp"
#include "../../src/settings.hpp"
#include "../../src/command_handler.hpp"
#include "../../src/logger.hpp"

using namespace std::chrono;

extern std::atomic<int> g_notifications_triggered;

void mock_notification_handler(const std::string& title, const std::string& message) {
    g_notifications_triggered++;
    Logger::get_instance().info("Notification: " + title + " - " + message);
}

void test_slack_multiple_bots_simulation() {
    std::cout << "\n--- Multiple Bots Simulation Test ---" << std::endl;
    
    const int bot_count = 3;
    const int messages_per_bot = 10;
    
    std::vector<std::thread> bot_threads;
    std::atomic<int> total_processed{0};
    
    auto bot_worker = [&](int bot_id) {
        Settings settings = Settings::get_defaults();
        MessageFilter filter(settings);
        CommandHandler handler;
        
        for (int i = 0; i < messages_per_bot; ++i) {
            std::string json = R"({
                "type": "events_api",
                "envelope_id": "bot)" + std::to_string(bot_id) + 
                "-msg" + std::to_string(i) + R"(",
                "payload": {
                    "type": "event_callback",
                    "event": {
                        "type": "message",
                        "text": "Message from bot )" + std::to_string(bot_id) + 
                        " message " + std::to_string(i) + R"(",
                        "channel": "C12345678",
                        "user": "U1234567890"
                    }
                }
            })";
            
            auto channel = event_parser::extract_channel_id(json);
            auto text = event_parser::extract_message_text(json);
            
            if (channel && text) {
                if (filter.should_process(*channel, *text)) {
                    total_processed++;
                }
            }
        }
    };
    
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < bot_count; ++i) {
        bot_threads.emplace_back(bot_worker, i);
    }
    
    for (auto& t : bot_threads) {
        t.join();
    }
    
    auto end = high_resolution_clock::now();
    auto duration_ms = duration_cast<milliseconds>(end - start).count();
    
    std::cout << "  Bots: " << bot_count << std::endl;
    std::cout << "  Messages per bot: " << messages_per_bot << std::endl;
    std::cout << "  Total processed: " << total_processed.load() << std::endl;
    std::cout << "  Duration: " << duration_ms << " ms" << std::endl;
    
    assert(total_processed.load() == bot_count * messages_per_bot);
    std::cout << "  ✓ Multiple bots simulation successful" << std::endl;
}

void test_slack_notification_flow() {
    std::cout << "\n--- Notification Flow Test ---" << std::endl;
    
    Settings settings = Settings::get_defaults();
    settings.notifications.enabled = true;
    settings.events.keyword_filter.enabled = true;
    settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::INCLUDE;
    settings.events.keyword_filter.keywords = {"urgent", "important"};
    
    MessageFilter filter(settings);
    
    std::vector<std::string> test_messages = {
        "This is an urgent message",
        "Important update here",
        "Just a normal message",
        "Another normal message",
        "Critical: urgent action needed"
    };
    
    int notifications_triggered = 0;
    
    for (const auto& message : test_messages) {
        if (filter.should_process("C12345678", message, "U1234567890")) {
            notifications_triggered++;
            mock_notification_handler("Slack Message", message);
        }
    }
    
    std::cout << "  Messages tested: " << test_messages.size() << std::endl;
    std::cout << "  Notifications triggered: " << notifications_triggered << std::endl;
    
    assert(notifications_triggered == 3);
    std::cout << "  ✓ Notification flow works correctly" << std::endl;
}
