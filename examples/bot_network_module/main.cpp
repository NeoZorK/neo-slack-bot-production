#include "bot_network.hpp"
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>

using namespace std::chrono;

int main(int argc, char* argv[]) {
    const char* channel = std::getenv("SLACK_TEST_CHANNEL");
    if (!channel) {
        std::cerr << "Error: SLACK_TEST_CHANNEL not set" << std::endl;
        std::cerr << "Usage: export SLACK_TEST_CHANNEL=C12345678" << std::endl;
        return 1;
    }
    
    int bot_count = 100;
    if (argc > 1) {
        bot_count = std::atoi(argv[1]);
    }
    
    std::cout << "Bot Network Example" << std::endl;
    std::cout << "==================" << std::endl;
    std::cout << "Bots: " << bot_count << std::endl;
    std::cout << "Channel: " << channel << std::endl;
    std::cout << "Duration: 60 seconds" << std::endl;
    std::cout << "\n⚠️  Note: Slack rate limits are ~1 message/second per channel." << std::endl;
    std::cout << "   With " << bot_count << " bots, expect rate limiting." << std::endl;
    std::cout << "\nStarting in 3 seconds..." << std::endl;
    std::this_thread::sleep_for(seconds(3));
    
    try {
        bot_network::BotNetwork network(bot_count, channel);
        network.start();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
