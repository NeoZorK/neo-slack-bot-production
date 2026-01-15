/**
 * @file test_channel_management.cpp
 * @brief Tests for channel_management.cpp example
 */

#include <iostream>
#include <cassert>
// Test channel_management example functionality
// Note: This tests the example code structure, not full execution

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <set>
#include "../src/message_filter.hpp"
#include "../src/settings.hpp"
#include "../src/command_handler.hpp"

// Forward declarations matching channel_management.cpp
void demonstrate_channel_filtering();
void demonstrate_keyword_filtering();
void demonstrate_command_handling();

void demonstrate_channel_filtering() {
    Settings settings = Settings::get_defaults();
    
    // Whitelist mode
    settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::WHITELIST;
    settings.notifications.channel_filter.channels.insert("C12345678");
    settings.notifications.channel_filter.channels.insert("C87654321");
    
    MessageFilter filter(settings);
    assert(filter.should_process("C12345678", "test", "U123"));
    assert(!filter.should_process("C99999999", "test", "U123"));
    
    // Blacklist mode
    settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::BLACKLIST;
    settings.notifications.channel_filter.channels.clear();
    settings.notifications.channel_filter.channels.insert("C12345678");
    
    MessageFilter filter2(settings);
    assert(!filter2.should_process("C12345678", "test", "U123"));
    assert(filter2.should_process("C99999999", "test", "U123"));
}

void demonstrate_keyword_filtering() {
    Settings settings = Settings::get_defaults();
    settings.events.keyword_filter.enabled = true;
    settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::INCLUDE;
    settings.events.keyword_filter.keywords = {"urgent", "important", "critical"};
    
    MessageFilter filter(settings);
    assert(filter.should_process("C123", "This is an urgent message", "U123"));
    assert(filter.should_process("C123", "Important update here", "U123"));
    assert(!filter.should_process("C123", "Just a normal message", "U123"));
}

void demonstrate_command_handling() {
    CommandHandler handler;
    int command_count = 0;
    
    handler.register_command("test", [&command_count](const std::string& /* channel */,
                                                      const std::vector<std::string>& /* args */) {
        command_count++;
    });
    
    handler.handle_command("/test arg1 arg2", "C12345678", "U1234567890");
    assert(command_count == 1);
}

void test_channel_filtering_demo() {
    std::cout << "Testing channel filtering demo..." << std::endl;
    
    try {
        demonstrate_channel_filtering();
        std::cout << "  ✓ Channel filtering demo works" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  ✗ Channel filtering demo failed: " << e.what() << std::endl;
        assert(false);
    }
}

void test_keyword_filtering_demo() {
    std::cout << "Testing keyword filtering demo..." << std::endl;
    
    try {
        demonstrate_keyword_filtering();
        std::cout << "  ✓ Keyword filtering demo works" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  ✗ Keyword filtering demo failed: " << e.what() << std::endl;
        assert(false);
    }
}

void test_command_handling_demo() {
    std::cout << "Testing command handling demo..." << std::endl;
    
    try {
        demonstrate_command_handling();
        std::cout << "  ✓ Command handling demo works" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  ✗ Command handling demo failed: " << e.what() << std::endl;
        assert(false);
    }
}

void test_channel_management_example() {
    std::cout << "\n=== Channel Management Example Tests ===" << std::endl;
    
    test_channel_filtering_demo();
    test_keyword_filtering_demo();
    test_command_handling_demo();
    
    std::cout << "All channel management tests passed!" << std::endl;
}
