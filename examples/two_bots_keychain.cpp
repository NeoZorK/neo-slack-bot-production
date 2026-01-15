/**
 * @file two_bots_keychain.cpp
 * @brief Example of two bots communicating using channel ID from Keychain
 */

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <mutex>
#include "../src/websocket_client.hpp"
#include "../src/slack_api.hpp"
#include "../src/event_parser.hpp"
#include "../src/platform/macos/macos_keychain_impl.hpp"

std::mutex g_console_mutex;

void log_console(const std::string& prefix, const std::string& msg) {
    std::lock_guard<std::mutex> lock(g_console_mutex);
    std::cout << "[" << prefix << "] " << msg << std::endl;
}

// Wrapper to run client in background
void run_client_background(WebSocketClient* client) {
    try {
        client->run();
    } catch (...) {}
}

int main() {
    const char* env_bot_token = std::getenv("SLACK_BOT_TOKEN");
    const char* env_app_token = std::getenv("SLACK_APP_TOKEN");

    if (!env_bot_token || !env_app_token) {
        std::cerr << "Please set SLACK_BOT_TOKEN and SLACK_APP_TOKEN env vars." << std::endl;
        return 1;
    }

    std::string service = "neo-slack-bot";
    std::string account = "slack-test-channel";
    std::string channel_id;

    auto secret = MacOSKeychainImpl::get_password(service, account);
    if (secret) {
        channel_id = *secret;
        std::cout << "Loaded Channel ID from Keychain: " << channel_id << std::endl;
    } else {
        const char* env_channel = std::getenv("SLACK_TEST_CHANNEL");
        if (!env_channel) {
            std::cerr << "Channel ID not found in Keychain and SLACK_TEST_CHANNEL is not set." << std::endl;
            std::cerr << "Set SLACK_TEST_CHANNEL to a valid Slack channel ID." << std::endl;
            return 1;
        }
        channel_id = env_channel;
        std::cout << "Using Channel ID from SLACK_TEST_CHANNEL: " << channel_id << std::endl;
        if (MacOSKeychainImpl::set_password(service, account, channel_id)) {
            std::cout << "Saved " << channel_id << " to Keychain for future use." << std::endl;
        } else {
            std::cerr << "Failed to save to Keychain." << std::endl;
        }
    }

    WebSocketClient bot1(env_bot_token);
    WebSocketClient bot2(env_bot_token);
    
    SlackAPI api(env_app_token);
    std::string wss_url_bot1;
    std::string wss_url_bot2;
    if (!api.get_socket_mode_url(wss_url_bot1)) {
        std::cerr << "Failed to get WSS URL" << std::endl;
        return 1;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (!api.get_socket_mode_url(wss_url_bot2)) {
        std::cerr << "Failed to get WSS URL for second bot" << std::endl;
        return 1;
    }
    bot1.set_websocket_url(wss_url_bot1);
    bot2.set_websocket_url(wss_url_bot2);

    SlackAPI sender_api(env_bot_token);
    
    bot1.set_message_handler([&sender_api, channel_id](const std::string& msg) {
        std::string preview = msg.size() > 500 ? msg.substr(0, 500) : msg;
        log_console("BOT_1", "Raw message: " + preview);
        
        if (!event_parser::is_message_event(msg)) {
            return;
        }
        auto ch = event_parser::extract_channel_id(msg);
        auto text = event_parser::extract_message_text(msg);
        if (!ch || !text || *ch != channel_id) {
            return;
        }
        
        if (text->rfind("BOT_2:", 0) == 0) {
            std::string reply = "BOT_1: ack -> " + *text;
            log_console("BOT_1", "Replying: " + reply);
            sender_api.send_message(channel_id, reply);
        }
    });
    
    bot2.set_message_handler([&sender_api, channel_id](const std::string& msg) {
        std::string preview = msg.size() > 500 ? msg.substr(0, 500) : msg;
        log_console("BOT_2", "Raw message: " + preview);
        
        if (!event_parser::is_message_event(msg)) {
            return;
        }
        auto ch = event_parser::extract_channel_id(msg);
        auto text = event_parser::extract_message_text(msg);
        if (!ch || !text || *ch != channel_id) {
            return;
        }
        
        if (text->rfind("BOT_1:", 0) == 0) {
            std::string reply = "BOT_2: ack -> " + *text;
            log_console("BOT_2", "Replying: " + reply);
            sender_api.send_message(channel_id, reply);
        }
    });

    std::cout << "Starting bots..." << std::endl;

    // Run bots in background threads
    std::thread t1(run_client_background, &bot1);
    std::thread t2(run_client_background, &bot2);

    std::cout << "Bots running. Sending messages..." << std::endl;

    for (int i = 0; i < 3; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::string msg = "BOT_1: hello (msg " + std::to_string(i) + ")";
        log_console("BOT_1", "Sending: " + msg);
        sender_api.send_message(channel_id, msg);
    }

    std::cout << "Finished sending. Waiting 5s..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    bot1.set_should_stop(true);
    bot2.set_should_stop(true);

    if (t1.joinable()) {
        t1.join();
    }
    if (t2.joinable()) {
        t2.join();
    }

    return 0;
}
