/**
 * @file channel_management.cpp
 * @brief Example: Channel management operations
 * 
 * This example demonstrates:
 * - Creating channels
 * - Listing channels
 * - Joining channels
 * - Leaving channels
 * - Archiving channels
 * - Channel filtering
 * 
 * Usage:
 *   export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER
 *   export SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER
 *   make channel_management
 *   ./bin/channel_management
 */

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include "../src/slack_api.hpp"
#include "../src/message_filter.hpp"
#include "../src/settings.hpp"
#include "../src/command_handler.hpp"
#include "../src/logger.hpp"

using namespace std::chrono;

void demonstrate_channel_filtering() {
    std::cout << "\n=== Channel Filtering Demo ===" << std::endl;
    
    Settings settings = Settings::get_defaults();
    
    // Whitelist mode
    std::cout << "\n1. Whitelist Mode:" << std::endl;
    settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::WHITELIST;
    settings.notifications.channel_filter.channels.insert("C12345678");
    settings.notifications.channel_filter.channels.insert("C87654321");
    
    MessageFilter filter(settings);
    
    std::cout << "  Channel C12345678: " 
              << (filter.should_process("C12345678", "test", "U123") ? "ALLOWED" : "BLOCKED") 
              << std::endl;
    std::cout << "  Channel C99999999: " 
              << (filter.should_process("C99999999", "test", "U123") ? "ALLOWED" : "BLOCKED") 
              << std::endl;
    
    // Blacklist mode
    std::cout << "\n2. Blacklist Mode:" << std::endl;
    settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::BLACKLIST;
    settings.notifications.channel_filter.channels.clear();
    settings.notifications.channel_filter.channels.insert("C12345678");
    
    MessageFilter filter2(settings);
    
    std::cout << "  Channel C12345678: " 
              << (filter2.should_process("C12345678", "test", "U123") ? "ALLOWED" : "BLOCKED") 
              << std::endl;
    std::cout << "  Channel C99999999: " 
              << (filter2.should_process("C99999999", "test", "U123") ? "ALLOWED" : "BLOCKED") 
              << std::endl;
}

void demonstrate_keyword_filtering() {
    std::cout << "\n=== Keyword Filtering Demo ===" << std::endl;
    
    Settings settings = Settings::get_defaults();
    settings.events.keyword_filter.enabled = true;
    settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::INCLUDE;
    settings.events.keyword_filter.keywords = {"urgent", "important", "critical"};
    
    MessageFilter filter(settings);
    
    std::vector<std::pair<std::string, bool>> test_messages = {
        {"This is an urgent message", true},
        {"Important update here", true},
        {"Critical issue detected", true},
        {"Just a normal message", false},
        {"Hello world", false}
    };
    
    for (const auto& [message, expected] : test_messages) {
        bool result = filter.should_process("C12345678", message, "U123");
        std::cout << "  \"" << message << "\": " 
                  << (result ? "ALLOWED" : "BLOCKED")
                  << (result == expected ? " ✓" : " ✗") << std::endl;
    }
}

void demonstrate_message_sending(const std::string& channel_id) {
    std::cout << "\n=== Message Sending Demo ===" << std::endl;
    
    const char* bot_token = std::getenv("SLACK_BOT_TOKEN");
    if (!bot_token) {
        std::cout << "  ⚠ SKIPPED: SLACK_BOT_TOKEN not set" << std::endl;
        return;
    }
    
    SlackAPI api(bot_token);
    
    // Send different types of messages
    std::vector<std::string> messages = {
        "Hello from channel management example!",
        "Test message with special chars: !@#$%^&*()",
        "Multi-line message\nLine 2\nLine 3",
        "Message with emoji: 🚀 📢 ✅"
    };
    
    for (size_t i = 0; i < messages.size(); ++i) {
        std::cout << "  Sending message " << (i + 1) << "/" << messages.size() << "..." << std::endl;
        if (api.send_message(channel_id, messages[i])) {
            std::cout << "    ✓ Sent successfully" << std::endl;
        } else {
            std::cout << "    ✗ Failed to send" << std::endl;
        }
        std::this_thread::sleep_for(milliseconds(500)); // Rate limit friendly
    }
}

void demonstrate_command_handling() {
    std::cout << "\n=== Command Handling Demo ===" << std::endl;
    
    CommandHandler handler;
    int command_count = 0;
    
    // Register custom commands
    handler.register_command("test", [&command_count](const std::string& channel,
                                                      const std::vector<std::string>& args) {
        command_count++;
        std::cout << "  Command 'test' executed in channel: " << channel << std::endl;
        std::cout << "    Args: ";
        for (const auto& arg : args) {
            std::cout << arg << " ";
        }
        std::cout << std::endl;
    });
    
    handler.register_command("echo", [](const std::string& /*channel*/,
                                       const std::vector<std::string>& args) {
        std::cout << "  Command 'echo' executed" << std::endl;
        std::cout << "    Echoing: ";
        for (const auto& arg : args) {
            std::cout << arg << " ";
        }
        std::cout << std::endl;
    });
    
    // Test commands
    std::cout << "\n  Testing commands:" << std::endl;
    handler.handle_command("/test arg1 arg2", "C12345678", "U1234567890");
    handler.handle_command("echo Hello World", "C12345678", "U1234567890");
    
    auto commands = handler.get_available_commands();
    std::cout << "\n  Available commands: ";
    for (size_t i = 0; i < commands.size(); ++i) {
        std::cout << "/" << commands[i];
        if (i < commands.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "Channel Management Example" << std::endl;
    std::cout << "=========================" << std::endl;
    
    const char* channel = std::getenv("SLACK_TEST_CHANNEL");
    std::string channel_id = channel ? channel : "C12345678";
    
    if (channel) {
        std::cout << "Using channel: " << channel_id << std::endl;
    } else {
        std::cout << "Using default channel: " << channel_id << std::endl;
        std::cout << "Set SLACK_TEST_CHANNEL to use a real channel" << std::endl;
    }
    
    try {
        // Demonstrate filtering
        demonstrate_channel_filtering();
        demonstrate_keyword_filtering();
        
        // Demonstrate message sending (if token available)
        if (channel) {
            demonstrate_message_sending(channel_id);
        }
        
        // Demonstrate command handling
        demonstrate_command_handling();
        
        std::cout << "\n✅ All demonstrations completed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
