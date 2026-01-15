#include <iostream>
#include <cassert>
#include "../src/event_parser.hpp"

void test_event_parser_type_detection() {
    std::string hello_json = R"({"type":"hello","connection_info":{"app_id":"A123"}})";
    assert(event_parser::parse_type(hello_json) == event_parser::event_type::hello);
    
    std::string events_api_json = R"({"type":"events_api","envelope_id":"123"})";
    assert(event_parser::parse_type(events_api_json) == event_parser::event_type::events_api);
    
    std::string disconnect_json = R"({"type":"disconnect"})";
    assert(event_parser::parse_type(disconnect_json) == event_parser::event_type::disconnect);
    
    std::string unknown_json = R"({"type":"unknown"})";
    assert(event_parser::parse_type(unknown_json) == event_parser::event_type::unknown);
}

void test_event_parser_envelope_id() {
    std::string json = R"({"type":"events_api","envelope_id":"E123456"})";
    auto id = event_parser::extract_envelope_id(json);
    assert(id.has_value());
    assert(*id == "E123456");
}

void test_event_parser_message_text() {
    std::string json = R"({"type":"events_api","payload":{"event":{"type":"message","text":"Hello"}}})";
    auto text = event_parser::extract_message_text(json);
    assert(text.has_value());
    assert(*text == "Hello");
}

void test_event_parser_channel_id() {
    std::string json = R"({"type":"events_api","payload":{"event":{"channel":"C123456"}}})";
    auto channel = event_parser::extract_channel_id(json);
    assert(channel.has_value());
    assert(*channel == "C123456");
}

void test_event_parser_is_message() {
    std::string message_json = R"({"type":"events_api","payload":{"event":{"type":"message"}}})";
    assert(event_parser::is_message_event(message_json));
    
    std::string non_message_json = R"({"type":"events_api","payload":{"event":{"type":"other"}}})";
    assert(!event_parser::is_message_event(non_message_json));
}

void test_event_parser_is_app_mention() {
    std::string mention_json = R"({"type":"events_api","payload":{"event":{"type":"app_mention"}}})";
    assert(event_parser::is_app_mention(mention_json));
    
    std::string non_mention_json = R"({"type":"events_api","payload":{"event":{"type":"message"}}})";
    assert(!event_parser::is_app_mention(non_mention_json));
}

void test_event_parser() {
    test_event_parser_type_detection();
    test_event_parser_envelope_id();
    test_event_parser_message_text();
    test_event_parser_channel_id();
    test_event_parser_is_message();
    test_event_parser_is_app_mention();
    std::cout << "All event parser tests passed!" << std::endl;
}
