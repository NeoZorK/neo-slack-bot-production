#include <iostream>
#include <chrono>
#include <cassert>
#include "../src/settings.hpp"
#include "../src/config_loader.hpp"
#include "../src/json_parser.hpp"
#include "../src/message_filter.hpp"

using namespace std::chrono;
using namespace std;

void stress_test_settings_creation(int iterations) {
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        Settings settings = Settings::get_defaults();
        (void)settings.is_dnd_active();
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    
    std::cout << "Settings creation stress test:" << std::endl;
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "  Duration: " << duration << " ms" << std::endl;
    
    assert(duration >= 0);
}

void stress_test_config_loader(int iterations) {
    ConfigLoader loader;
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        Settings settings = loader.load_settings();
        (void)settings;
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    
    std::cout << "ConfigLoader stress test:" << std::endl;
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "  Duration: " << duration << " ms" << std::endl;
    
    assert(duration >= 0);
}

void stress_test_json_parsing(int iterations) {
    std::string json = R"({"notifications":{"enabled":true,"sound_enabled":false},"connection":{"max_retries":10,"timeout_seconds":30},"events":{"keyword_filter":{"enabled":true,"mode":"include","keywords":["urgent","important"]}}})";
    
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        std::string notifications = json_parser::extract_string(json, "notifications");
        json_parser::extract_bool(notifications, "enabled", false);
        std::string connection = json_parser::extract_string(json, "connection");
        json_parser::extract_int(connection, "max_retries", 0);
        std::string events = json_parser::extract_string(json, "events");
        std::string keyword_filter = json_parser::extract_string(events, "keyword_filter");
        json_parser::extract_string_array(keyword_filter, "keywords");
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    long duration_long = static_cast<long>(duration);
    
    std::cout << "JSON parsing stress test:" << std::endl;
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "  Duration: " << duration << " ms" << std::endl;
    if (duration_long > 0) {
        std::cout << "  Rate: " << (iterations * 1000.0 / duration_long) 
                  << " operations/sec" << std::endl;
    } else {
        std::cout << "  Rate: > 1M operations/sec (extremely fast)" << std::endl;
    }
    
    assert(duration >= 0);
}

void stress_test_message_filter(int iterations) {
    Settings settings = Settings::get_defaults();
    settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::WHITELIST;
    settings.notifications.channel_filter.channels.insert("C123");
    settings.events.user_filter.mode = Settings::EventSettings::UserFilter::BLACKLIST;
    settings.events.user_filter.users.insert("U999");
    
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        MessageFilter filter(settings);
        filter.should_process("C123", "test message", "U123");
        filter.should_process("C456", "test message", "U123");
        filter.should_process("C123", "urgent message", "U999");
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    
    std::cout << "MessageFilter stress test:" << std::endl;
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "  Duration: " << duration << " ms" << std::endl;
    
    assert(duration >= 0);
}

void test_stress_components() {
    stress_test_settings_creation(10000);
    stress_test_config_loader(1000);
    stress_test_json_parsing(10000);
    stress_test_message_filter(1000);
}
