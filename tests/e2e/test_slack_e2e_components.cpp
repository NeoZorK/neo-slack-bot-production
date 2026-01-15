/**
 * @file test_slack_e2e_components.cpp
 * @brief E2E tests for Slack - component tests
 * 
 * Tests message parsing, filtering, and command handling.
 */

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <atomic>
#include "../../src/event_parser.hpp"
#include "../../src/message_filter.hpp"
#include "../../src/settings.hpp"
#include "../../src/command_handler.hpp"
#include "../../src/logger.hpp"

extern std::vector<std::string> g_received_messages;

void test_slack_e2e_message_parsing() {
    std::cout << "\n--- Slack Message Parsing Test ---" << std::endl;
    
    std::vector<std::string> test_messages = {
        R"({"type":"events_api","envelope_id":"test-123","payload":{"type":"event_callback","event":{"type":"message","text":"Hello world","channel":"C12345678","user":"U1234567890"}}})",
        R"({"type":"events_api","envelope_id":"test-456","payload":{"type":"event_callback","event":{"type":"message","text":"Test with \"quotes\"","channel":"C12345678","user":"U1234567890"}}})",
        R"({"type":"events_api","envelope_id":"test-789","payload":{"type":"event_callback","event":{"type":"app_mention","text":"<@U123> hello bot","channel":"C12345678","user":"U1234567890"}}})",
        R"({"type":"events_api","envelope_id":"test-abc","payload":{"type":"event_callback","event":{"type":"message","text":"Multi\nline\nmessage","channel":"C12345678","user":"U1234567890"}}})"
    };
    
    int parsed_count = 0;
    for (const auto& json : test_messages) {
        auto type = event_parser::parse_type(json);
        if (type == event_parser::event_type::events_api) {
            auto channel = event_parser::extract_channel_id(json);
            auto text = event_parser::extract_message_text(json);
            auto user = event_parser::extract_user_id(json);
            
            if (channel && text && user) {
                parsed_count++;
                g_received_messages.push_back(*text);
            }
        }
    }
    
    std::cout << "  Parsed " << parsed_count << " messages successfully" << std::endl;
    assert(parsed_count == static_cast<int>(test_messages.size()));
    std::cout << "  ✓ All messages parsed correctly" << std::endl;
}

void test_slack_channel_filtering() {
    std::cout << "\n--- Slack Channel Filtering Test ---" << std::endl;
    
    Settings settings = Settings::get_defaults();
    
    settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::WHITELIST;
    settings.notifications.channel_filter.channels.insert("C12345678");
    settings.notifications.channel_filter.channels.insert("C87654321");
    
    MessageFilter filter(settings);
    
    assert(filter.should_process("C12345678", "test message", "U123"));
    assert(filter.should_process("C87654321", "test message", "U123"));
    assert(!filter.should_process("C99999999", "test message", "U123"));
    
    std::cout << "  ✓ Whitelist filtering works correctly" << std::endl;
    
    settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::BLACKLIST;
    settings.notifications.channel_filter.channels.clear();
    settings.notifications.channel_filter.channels.insert("C12345678");
    
    filter.apply_settings(settings);
    
    assert(!filter.should_process("C12345678", "test message", "U123"));
    assert(filter.should_process("C99999999", "test message", "U123"));
    
    std::cout << "  ✓ Blacklist filtering works correctly" << std::endl;
}

void test_slack_keyword_filtering() {
    std::cout << "\n--- Slack Keyword Filtering Test ---" << std::endl;
    
    Settings settings = Settings::get_defaults();
    settings.events.keyword_filter.enabled = true;
    settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::INCLUDE;
    settings.events.keyword_filter.keywords = {"urgent", "important", "critical"};
    
    MessageFilter filter(settings);
    
    assert(filter.should_process("C123", "This is an urgent message", "U123"));
    assert(filter.should_process("C123", "Important update here", "U123"));
    assert(filter.should_process("C123", "Critical issue detected", "U123"));
    assert(!filter.should_process("C123", "Just a normal message", "U123"));
    assert(!filter.should_process("C123", "Hello world", "U123"));
    
    std::cout << "  ✓ Keyword filtering (include mode) works correctly" << std::endl;
    
    settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::EXCLUDE;
    filter.apply_settings(settings);
    
    assert(!filter.should_process("C123", "This is an urgent message", "U123"));
    assert(filter.should_process("C123", "Just a normal message", "U123"));
    
    std::cout << "  ✓ Keyword filtering (exclude mode) works correctly" << std::endl;
}

void test_slack_command_handling() {
    std::cout << "\n--- Slack Command Handling Test ---" << std::endl;
    
    CommandHandler handler;
    std::atomic<int> command_count{0};
    
    handler.register_command("test", [&command_count](const std::string& channel, 
                                                      const std::vector<std::string>& /* args */) {
        command_count++;
        Logger::get_instance().info("Command executed in channel: " + channel);
    });
    
    bool handled = handler.handle_command("/test arg1 arg2", "C12345678", "U1234567890");
    assert(handled);
    assert(command_count.load() == 1);
    
    handled = handler.handle_command("test arg1", "C12345678", "U1234567890");
    assert(handled);
    assert(command_count.load() == 2);
    
    handled = handler.handle_command("/unknown", "C12345678", "U1234567890");
    assert(!handled);
    
    std::cout << "  ✓ Command handling works correctly" << std::endl;
}
