/**
 * @file custom_handler.cpp
 * @brief Example with custom message handling
 * 
 * This example demonstrates:
 * - Custom message filtering
 * - Message parsing
 * - Conditional notification display
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include "../src/websocket_client.hpp"
#include "../src/slack_api.hpp"
#include "../src/notifications_factory.hpp"

// Simple message filter
bool should_notify(const std::string& message) {
    // Only notify for messages containing "urgent" or "@here"
    return message.find("urgent") != std::string::npos ||
           message.find("@here") != std::string::npos;
}

int main() {
    const char* bot_token = std::getenv("SLACK_BOT_TOKEN");
    const char* app_token = std::getenv("SLACK_APP_TOKEN");
    
    if (!bot_token) {
        std::cerr << "Error: SLACK_BOT_TOKEN not set" << std::endl;
        return 1;
    }
    
    if (!app_token) {
        std::cerr << "Warning: SLACK_APP_TOKEN not set, using fallback URL" << std::endl;
    }

    auto notifications = NotificationsFactory::create();
    WebSocketClient ws_client(bot_token);
    
    // Get WebSocket URL from Slack API if app_token is available
    if (app_token) {
        SlackAPI slack_api(app_token);
        std::string websocket_url;
        if (slack_api.get_socket_mode_url(websocket_url)) {
            ws_client.set_websocket_url(websocket_url);
        }
    }

    // Custom message handler with filtering
    ws_client.set_message_handler([&notifications](const std::string& message) {
        std::cout << "Processing message: " << message << std::endl;

        // Filter messages
        if (should_notify(message)) {
            notifications->show_notification(
                "Important Slack Message",
                message
            );
            std::cout << "Notification sent for important message" << std::endl;
        } else {
            std::cout << "Message filtered out (not urgent)" << std::endl;
        }
    });

    if (!ws_client.connect()) {
        std::cerr << "Failed to connect" << std::endl;
        return 1;
    }

    std::cout << "Custom handler bot running..." << std::endl;
    ws_client.run();

    return 0;
}
