/**
 * @file basic_bot.cpp
 * @brief Basic example of using Neo Slack Bot
 * 
 * This example demonstrates how to:
 * - Initialize WebSocket client
 * - Connect to Slack Socket Mode
 * - Handle incoming messages
 * - Display notifications
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include "../src/websocket_client.hpp"
#include "../src/slack_api.hpp"
#include "../src/notifications_factory.hpp"

int main() {
    // Get tokens from environment
    const char* bot_token = std::getenv("SLACK_BOT_TOKEN");
    const char* app_token = std::getenv("SLACK_APP_TOKEN");
    
    if (!bot_token) {
        std::cerr << "Error: SLACK_BOT_TOKEN environment variable not set" << std::endl;
        std::cerr << "Usage: export SLACK_BOT_TOKEN=xoxb-BOT_TOKEN_PLACEHOLDER" << std::endl;
        return 1;
    }
    
    if (!app_token) {
        std::cerr << "Warning: SLACK_APP_TOKEN not set, using fallback URL" << std::endl;
        std::cerr << "For better reliability, set SLACK_APP_TOKEN=xapp-APP_TOKEN_PLACEHOLDER" << std::endl;
    }

    std::cout << "Initializing Neo Slack Bot..." << std::endl;

    // Initialize components
    auto notifications = NotificationsFactory::create();
    WebSocketClient ws_client(bot_token);
    
    // Get WebSocket URL from Slack API if app_token is available
    if (app_token) {
        SlackAPI slack_api(app_token);
        std::string websocket_url;
        if (slack_api.get_socket_mode_url(websocket_url)) {
            ws_client.set_websocket_url(websocket_url);
            std::cout << "Got WebSocket URL from Slack API" << std::endl;
        } else {
            std::cerr << "Warning: Failed to get WebSocket URL, using fallback" << std::endl;
        }
    }

    // Set up message handler
    ws_client.set_message_handler([&notifications](const std::string& message) {
        std::cout << "Received message: " << message << std::endl;
        
        // Display notification
        if (notifications) {
            notifications->show_notification(
                "Slack Message",
                message
            );
        }
    });

    // Set up error handler
    ws_client.set_error_handler([](const std::string& error) {
        std::cerr << "Error: " << error << std::endl;
    });

    // Connect to Slack
    std::cout << "Connecting to Slack..." << std::endl;
    if (!ws_client.connect()) {
        std::cerr << "Failed to connect to Slack" << std::endl;
        return 1;
    }

    std::cout << "Connected! Bot is running. Press Ctrl+C to stop." << std::endl;

    // Run event loop
    try {
        ws_client.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
