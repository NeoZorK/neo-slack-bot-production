/**
 * @file add_bot_to_channel.cpp
 * @brief Utility to add bot to a Slack channel
 * 
 * This utility helps add your bot to a Slack channel programmatically.
 * 
 * Usage:
 *   export SLACK_BOT_TOKEN=xoxb-your-token
 *   make add_bot_to_channel
 *   ./bin/add_bot_to_channel C1234567890
 */

#include <iostream>
#include <cstdlib>
#include "../src/slack_api.hpp"
#include "../src/logger.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <channel_id>" << std::endl;
        std::cerr << "Example: " << argv[0] << " C1234567890" << std::endl;
        std::cerr << "\nTo find channel ID:" << std::endl;
        std::cerr << "  1. Open channel in Slack" << std::endl;
        std::cerr << "  2. Click channel name at top" << std::endl;
        std::cerr << "  3. Scroll down to 'About' section" << std::endl;
        std::cerr << "  4. Channel ID will be shown there" << std::endl;
        return 1;
    }
    
    const char* bot_token = std::getenv("SLACK_BOT_TOKEN");
    if (!bot_token) {
        std::cerr << "Error: SLACK_BOT_TOKEN environment variable not set" << std::endl;
        std::cerr << "Usage: export SLACK_BOT_TOKEN=xoxb-your-token" << std::endl;
        return 1;
    }
    
    std::string channel_id = argv[1];
    
    std::cout << "Adding bot to channel: " << channel_id << std::endl;
    
    SlackAPI api(bot_token);
    
    // Get bot user ID first
    std::string bot_user_id;
    if (!api.get_bot_user_id(bot_user_id)) {
        std::cerr << "Error: Failed to get bot user ID. Check your token." << std::endl;
        return 1;
    }
    std::cout << "Bot user ID: " << bot_user_id << std::endl;
    
    // Try to join channel
    if (api.join_channel(channel_id)) {
        std::cout << "Success! Bot has been added to channel " << channel_id << std::endl;
        return 0;
    } else {
        std::cerr << "\nFailed to add bot to channel. Possible reasons:" << std::endl;
        std::cerr << "  1. Bot doesn't have 'channels:join' or 'groups:write' scope" << std::endl;
        std::cerr << "  2. Channel is private and bot needs manual invitation" << std::endl;
        std::cerr << "  3. Channel doesn't exist or ID is incorrect" << std::endl;
        std::cerr << "\nTry manually:" << std::endl;
        std::cerr << "  1. Open channel #nz in Slack" << std::endl;
        std::cerr << "  2. Type: /invite @YourBotName" << std::endl;
        std::cerr << "  3. Or go to channel settings > Integrations > Add apps" << std::endl;
        return 1;
    }
}
