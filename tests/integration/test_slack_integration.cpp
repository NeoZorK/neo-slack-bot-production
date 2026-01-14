#include <iostream>
#include <cassert>
#include <string>
#include <cstdlib>
#include "../src/slack_api.hpp"
#include "../src/websocket_client.hpp"
#include "../src/event_parser.hpp"
#include "../src/command_handler.hpp"
#include "../src/message_filter.hpp"
#include "../src/settings.hpp"
#include "../src/logger.hpp"

// Integration tests for Slack bot functionality
// These tests verify end-to-end message flow and command handling

void test_slack_message_parsing() {
    std::string message_json = R"({
        "type": "events_api",
        "envelope_id": "test-envelope-123",
        "payload": {
            "type": "event_callback",
            "event": {
                "type": "message",
                "text": "Hello, bot!",
                "channel": "C1234567890",
                "user": "U0987654321",
                "ts": "1234567890.123456"
            }
        }
    })";
    
    auto event_type = event_parser::parse_type(message_json);
    assert(event_type == event_parser::event_type::events_api);
    
    auto envelope_id = event_parser::extract_envelope_id(message_json);
    assert(envelope_id.has_value());
    assert(*envelope_id == "test-envelope-123");
    
    assert(event_parser::is_message_event(message_json));
    
    auto text = event_parser::extract_message_text(message_json);
    assert(text.has_value());
    assert(*text == "Hello, bot!");
    
    auto channel = event_parser::extract_channel_id(message_json);
    assert(channel.has_value());
    assert(*channel == "C1234567890");
    
    auto user_id = event_parser::extract_user_id(message_json);
    assert(user_id.has_value());
    assert(*user_id == "U0987654321");
    
    std::cout << "  ✓ Slack message parsing test passed" << std::endl;
}

void test_slack_app_mention_parsing() {
    std::string mention_json = R"({
        "type": "events_api",
        "envelope_id": "test-envelope-456",
        "payload": {
            "type": "event_callback",
            "event": {
                "type": "app_mention",
                "text": "<@U123456> hello",
                "channel": "C1234567890",
                "user": "U0987654321",
                "ts": "1234567890.123456"
            }
        }
    })";
    
    assert(event_parser::is_app_mention(mention_json));
    
    auto text = event_parser::extract_message_text(mention_json);
    assert(text.has_value());
    assert(text->find("hello") != std::string::npos);
    
    std::cout << "  ✓ Slack app mention parsing test passed" << std::endl;
}

void test_slack_command_parsing() {
    std::string command_json = R"({
        "type": "slash_commands",
        "envelope_id": "test-envelope-789",
        "payload": {
            "command": "/status",
            "text": "",
            "channel_id": "C1234567890",
            "user_id": "U0987654321"
        }
    })";
    
    auto event_type = event_parser::parse_type(command_json);
    assert(event_type == event_parser::event_type::slash_commands);
    
    auto envelope_id = event_parser::extract_envelope_id(command_json);
    assert(envelope_id.has_value());
    
    auto command_text = event_parser::extract_command_text(command_json);
    assert(command_text.has_value());
    assert(*command_text == "/status");
    
    auto channel_id = event_parser::extract_channel_id(command_json);
    assert(channel_id.has_value());
    assert(*channel_id == "C1234567890");
    
    std::cout << "  ✓ Slack command parsing test passed" << std::endl;
}

void test_command_handler_integration() {
    CommandHandler handler;
    bool status_called = false;
    bool help_called = false;
    
    handler.register_command("status", [&status_called](const std::string& channel,
                                                         const std::vector<std::string>& /*args*/) {
        status_called = true;
        assert(!channel.empty());
    });
    
    handler.register_command("help", [&help_called](const std::string& channel,
                                                    const std::vector<std::string>& /*args*/) {
        help_called = true;
        assert(!channel.empty());
    });
    
    // Simulate command from Slack
    std::string command_json = R"({
        "type": "slash_commands",
        "envelope_id": "test-envelope",
        "payload": {
            "command": "/status",
            "text": "",
            "channel_id": "C1234567890",
            "user_id": "U0987654321"
        }
    })";
    
    auto command_text = event_parser::extract_command_text(command_json);
    auto channel_id = event_parser::extract_channel_id(command_json);
    auto user_id = event_parser::extract_user_id(command_json);
    
    if (command_text && channel_id) {
        handler.handle_command(*command_text, *channel_id, user_id.value_or(""));
        assert(status_called);
        assert(!help_called);
    }
    
    std::cout << "  ✓ Command handler integration test passed" << std::endl;
}

void test_message_filter_integration() {
    Settings settings;
    MessageFilter filter(settings);
    
    // Test channel whitelist
    filter.add_channel_whitelist("C1234567890");
    
    assert(filter.should_process("C1234567890", "Hello"));
    assert(!filter.should_process("C9999999999", "Hello"));
    
    // Test keyword filter
    filter.clear_filters();
    filter.add_keyword_filter("urgent");
    
    assert(filter.should_process("C1234567890", "This is urgent!"));
    assert(!filter.should_process("C1234567890", "Regular message"));
    
    std::cout << "  ✓ Message filter integration test passed" << std::endl;
}

void test_websocket_envelope_ack() {
    std::string message_json = R"({
        "type": "events_api",
        "envelope_id": "test-envelope-ack",
        "payload": {
            "type": "event_callback",
            "event": {
                "type": "message",
                "text": "Test",
                "channel": "C1234567890"
            }
        }
    })";
    
    auto envelope_id = event_parser::extract_envelope_id(message_json);
    assert(envelope_id.has_value());
    
    // Simulate ACK response
    std::string ack = "{\"envelope_id\":\"" + *envelope_id + "\"}";
    assert(ack.find("test-envelope-ack") != std::string::npos);
    
    std::cout << "  ✓ WebSocket envelope ACK test passed" << std::endl;
}

void test_slack_hello_event() {
    std::string hello_json = R"({
        "type": "hello"
    })";
    
    auto event_type = event_parser::parse_type(hello_json);
    assert(event_type == event_parser::event_type::hello);
    
    std::cout << "  ✓ Slack hello event test passed" << std::endl;
}

void test_slack_disconnect_event() {
    std::string disconnect_json = R"({
        "type": "disconnect"
    })";
    
    auto event_type = event_parser::parse_type(disconnect_json);
    assert(event_type == event_parser::event_type::disconnect);
    
    std::cout << "  ✓ Slack disconnect event test passed" << std::endl;
}

// Export for test_main.cpp
void test_slack_integration() {
    std::cout << "\n=== Slack Integration Tests ===" << std::endl;
    
    test_slack_message_parsing();
    test_slack_app_mention_parsing();
    test_slack_command_parsing();
    test_command_handler_integration();
    test_message_filter_integration();
    test_websocket_envelope_ack();
    test_slack_hello_event();
    test_slack_disconnect_event();
    
    std::cout << "All Slack integration tests passed!" << std::endl;
}
