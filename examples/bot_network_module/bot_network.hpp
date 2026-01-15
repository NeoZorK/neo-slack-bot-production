#pragma once

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include "../../src/websocket_client.hpp"
#include "../../src/slack_api.hpp"

namespace bot_network {

class BotNetwork {
public:
    BotNetwork(int bot_count, const std::string& channel_id);
    
    void start();
    
private:
    void setup_websocket_handler(int bot_id);
    void websocket_receiver(int bot_id);
    void bot_worker(int bot_id);
    void monitor_system();
    void print_statistics();
    
    int bot_count_;
    std::string channel_id_;
    std::string bot_token_;
    std::atomic<bool> running_;
    std::atomic<int> messages_sent_;
    std::atomic<int> messages_received_;
    std::atomic<int> rate_limit_errors_;
    std::vector<std::unique_ptr<SlackAPI>> bots_;
    std::vector<std::unique_ptr<WebSocketClient>> ws_clients_;
};

} // namespace bot_network
