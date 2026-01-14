/**
 * @file http_api_example.cpp
 * @brief Example using HTTP API to send messages
 * 
 * This example demonstrates:
 * - Using Slack API HTTP client
 * - Sending messages programmatically
 * - Error handling
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include "../src/slack_api.hpp"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <channel-id> <message>" << std::endl;
        std::cerr << "Example: " << argv[0] << " C1234567890 \"Hello from bot!\"" << std::endl;
        return 1;
    }

    const char* token = std::getenv("SLACK_BOT_TOKEN");
    if (!token) {
        std::cerr << "Error: SLACK_BOT_TOKEN not set" << std::endl;
        return 1;
    }

    std::string channel = argv[1];
    std::string message = argv[2];
    
    std::cout << "Initializing Slack API..." << std::endl;
    std::cout << "Channel ID: " << channel << std::endl;
    std::cout << "Message: \"" << message << "\"" << std::endl;
    
    SlackAPI api(token);
    
    // Check channel info first
    std::string channel_info;
    std::cout << "\nChecking channel info..." << std::endl;
    if (api.get_channel_info(channel, channel_info)) {
        std::cout << "✅ Channel info retrieved successfully" << std::endl;
    } else {
        std::cerr << "⚠️  Warning: Could not retrieve channel info. Channel may not exist or bot may not have access." << std::endl;
    }
    
    // Try to send message with detailed error reporting
    std::cout << "\nSending message..." << std::endl;
    std::string error_response;
    if (api.send_message(channel, message, error_response)) {
        std::cout << "✅ Message sent successfully!" << std::endl;
        return 0;
    } else {
        std::cerr << "\n❌ Failed to send message" << std::endl;
        if (!error_response.empty()) {
            std::cerr << "\nError response from Slack API:" << std::endl;
            std::cerr << error_response << std::endl;
        }
        std::cerr << "\nPossible reasons:" << std::endl;
        std::cerr << "  1. Bot doesn't have 'chat:write' scope" << std::endl;
        std::cerr << "  2. Bot is not in the channel (try: /invite @YourBotName in Slack)" << std::endl;
        std::cerr << "  3. Channel ID is incorrect" << std::endl;
        std::cerr << "  4. Bot token is invalid or expired" << std::endl;
        std::cerr << "  5. Channel is private and bot needs explicit invitation" << std::endl;
        std::cerr << "\nTo fix:" << std::endl;
        std::cerr << "  1. Check bot scopes at https://api.slack.com/apps" << std::endl;
        std::cerr << "  2. Invite bot to channel: /invite @YourBotName" << std::endl;
        std::cerr << "  3. Verify channel ID is correct (format: C1234567890)" << std::endl;
        return 1;
    }
}
