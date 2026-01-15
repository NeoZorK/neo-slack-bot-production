#include <iostream>
#include <cassert>
#include "../src/message_filter.hpp"
#include "../src/settings.hpp"

void test_message_filter_user_whitelist() {
    MessageFilter filter;
    filter.add_user_whitelist("U123");
    filter.add_user_whitelist("U456");
    
    assert(filter.should_process("C123", "test message", "U123"));
    assert(filter.should_process("C123", "test message", "U456"));
    assert(!filter.should_process("C123", "test message", "U789"));
}

void test_message_filter_user_blacklist() {
    MessageFilter filter;
    filter.add_user_blacklist("U123");
    
    assert(!filter.should_process("C123", "test message", "U123"));
    assert(filter.should_process("C123", "test message", "U456"));
}

void test_message_filter_with_settings() {
    Settings settings = Settings::get_defaults();
    
    settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::WHITELIST;
    settings.notifications.channel_filter.channels.insert("C123");
    settings.notifications.channel_filter.channels.insert("C456");
    
    settings.events.keyword_filter.enabled = true;
    settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::INCLUDE;
    settings.events.keyword_filter.keywords.push_back("urgent");
    
    settings.events.user_filter.mode = Settings::EventSettings::UserFilter::BLACKLIST;
    settings.events.user_filter.users.insert("U999");
    
    MessageFilter filter(settings);
    
    assert(filter.should_process("C123", "urgent message", "U123"));
    assert(!filter.should_process("C789", "urgent message", "U123"));
    assert(!filter.should_process("C123", "normal message", "U123"));
    assert(!filter.should_process("C123", "urgent message", "U999"));
}

void test_message_filter_keyword_include_mode() {
    Settings settings = Settings::get_defaults();
    settings.events.keyword_filter.enabled = true;
    settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::INCLUDE;
    settings.events.keyword_filter.keywords.push_back("important");
    
    MessageFilter filter(settings);
    
    assert(filter.should_process("C123", "This is important", "U123"));
    assert(!filter.should_process("C123", "This is normal", "U123"));
}

void test_message_filter_keyword_exclude_mode() {
    Settings settings = Settings::get_defaults();
    settings.events.keyword_filter.enabled = true;
    settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::EXCLUDE;
    settings.events.keyword_filter.keywords.push_back("spam");
    
    MessageFilter filter(settings);
    
    assert(filter.should_process("C123", "normal message", "U123"));
    assert(!filter.should_process("C123", "spam message", "U123"));
}

void test_message_filter_combined_filters() {
    Settings settings = Settings::get_defaults();
    
    settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::BLACKLIST;
    settings.notifications.channel_filter.channels.insert("C999");
    
    settings.events.user_filter.mode = Settings::EventSettings::UserFilter::WHITELIST;
    settings.events.user_filter.users.insert("U123");
    
    settings.events.keyword_filter.enabled = true;
    settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::INCLUDE;
    settings.events.keyword_filter.keywords.push_back("urgent");
    
    MessageFilter filter(settings);
    
    assert(filter.should_process("C123", "urgent message", "U123"));
    assert(!filter.should_process("C999", "urgent message", "U123"));
    assert(!filter.should_process("C123", "urgent message", "U456"));
    assert(!filter.should_process("C123", "normal message", "U123"));
}

void test_message_filter_all_filters_active() {
    Settings settings = Settings::get_defaults();
    
    // Channel Whitelist: C1, C2
    settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::WHITELIST;
    settings.notifications.channel_filter.channels.insert("C1");
    settings.notifications.channel_filter.channels.insert("C2");
    
    // User Whitelist: U1, U2
    settings.events.user_filter.mode = Settings::EventSettings::UserFilter::WHITELIST;
    settings.events.user_filter.users.insert("U1");
    settings.events.user_filter.users.insert("U2");
    
    // Keyword Include: "urgent"
    settings.events.keyword_filter.enabled = true;
    settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::INCLUDE;
    settings.events.keyword_filter.keywords.push_back("urgent");
    
    MessageFilter filter(settings);
    
    // Pass: C1, U1, "urgent"
    assert(filter.should_process("C1", "This is urgent", "U1"));
    
    // Fail: Wrong Channel (C3)
    assert(!filter.should_process("C3", "This is urgent", "U1"));
    
    // Fail: Wrong User (U3)
    assert(!filter.should_process("C1", "This is urgent", "U3"));
    
    // Fail: Wrong Keyword ("normal")
    assert(!filter.should_process("C1", "This is normal", "U1"));
    
    // Fail: Wrong Channel, Correct User/Keyword
    assert(!filter.should_process("C3", "This is urgent", "U1"));
    
    std::cout << "  ✓ Message filter all-active complex test passed" << std::endl;
}

void test_message_filter_conflicting_lists() {
    // Test what happens if user is manually added to both whitelist and blacklist (via API)
    MessageFilter filter;
    filter.add_user_blacklist("U1");
    filter.add_user_whitelist("U1");
    
    // Blacklist should take precedence
    assert(!filter.should_process("C1", "msg", "U1"));
    
    std::cout << "  ✓ Message filter conflicting lists test passed" << std::endl;
}

void test_message_filter_case_insensitive() {
    MessageFilter filter;
    filter.add_keyword_filter("Urgent");
    // Enable keyword filtering
    Settings settings = Settings::get_defaults();
    settings.events.keyword_filter.enabled = true;
    settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::INCLUDE;
    filter.apply_settings(settings);
    filter.add_keyword_filter("Urgent"); // Re-add after apply_settings clears it
    
    // Should match "urgent", "URGENT", "UrGeNt"
    assert(filter.should_process("C1", "This is urgent", "U1"));
    assert(filter.should_process("C1", "This is URGENT", "U1"));
    assert(filter.should_process("C1", "This is UrGeNt", "U1"));
    
    std::cout << "  ✓ Message filter case sensitivity test passed" << std::endl;
}

void test_message_filter_channel_conflicts() {
    MessageFilter filter;
    filter.add_channel_blacklist("C1");
    filter.add_channel_whitelist("C1");
    
    // Blacklist should take precedence (based on implementation order: blacklist check first)
    assert(!filter.should_process("C1", "msg", "U1"));
    
    std::cout << "  ✓ Message filter channel conflicts test passed" << std::endl;
}

void test_message_filter_empty_inputs() {
    MessageFilter filter;
    filter.add_keyword_filter("urgent");
    // Enable keyword filtering
    Settings settings = Settings::get_defaults();
    settings.events.keyword_filter.enabled = true;
    settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::INCLUDE;
    filter.apply_settings(settings);
    filter.add_keyword_filter("urgent");
    
    // Empty message should not match "urgent"
    assert(!filter.should_process("C1", "", "U1"));
    
    // Empty channel/user should still respect filters if they are not in lists
    // Since lists are empty (except keyword), and "urgent" matches, it should process
    assert(filter.should_process("", "urgent", "")); 
    
    // Test with actual whitelist - empty string shouldn't match
    filter.add_channel_whitelist("C1");
    assert(!filter.should_process("", "urgent", "")); // Empty channel not in whitelist
    
    std::cout << "  ✓ Message filter empty inputs test passed" << std::endl;
}

void test_message_filter_extended() {
    test_message_filter_user_whitelist();
    test_message_filter_user_blacklist();
    test_message_filter_with_settings();
    test_message_filter_keyword_include_mode();
    test_message_filter_keyword_exclude_mode();
    test_message_filter_combined_filters();
    test_message_filter_all_filters_active();
    test_message_filter_conflicting_lists();
    test_message_filter_case_insensitive();
    test_message_filter_channel_conflicts();
    test_message_filter_empty_inputs();
    std::cout << "All extended message filter tests passed!" << std::endl;
}
