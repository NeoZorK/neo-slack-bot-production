#include "bot_network.hpp"
#include "bot_network_rate_limiter.hpp"
#include "system_utils.hpp"
#include "../../src/logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <cstdlib>

using namespace std::chrono;

namespace bot_network {

BotNetwork::BotNetwork(int bot_count, const std::string& channel_id)
    : bot_count_(bot_count), channel_id_(channel_id), 
      running_(true), messages_sent_(0), messages_received_(0), rate_limit_errors_(0) {
    
    const char* bot_token = std::getenv("SLACK_BOT_TOKEN");
    if (!bot_token) {
        throw std::runtime_error("SLACK_BOT_TOKEN not set");
    }
    
    bot_token_ = std::string(bot_token);
    
    // Validate channel ID format
    if (channel_id_.empty() || (channel_id_[0] != 'C' && channel_id_[0] != 'D' && channel_id_[0] != 'G')) {
        std::cerr << "Warning: Channel ID '" << channel_id_ 
                  << "' may be invalid. Expected format: C1234567890 (channel), D1234567890 (DM), or G1234567890 (group)" << std::endl;
    }
    
    // Try to verify channel exists
    SlackAPI test_api(bot_token_);
    std::string channel_info;
    if (!test_api.get_channel_info(channel_id_, channel_info)) {
        std::cerr << "Warning: Could not verify channel '" << channel_id_ 
                  << "'. Make sure:" << std::endl;
        std::cerr << "  1. Channel exists in your Slack workspace" << std::endl;
        std::cerr << "  2. Bot is invited to the channel" << std::endl;
        std::cerr << "  3. Bot has 'chat:write' scope" << std::endl;
        std::cerr << "  4. Channel ID is correct (not a name, but ID like C1234567890)" << std::endl;
    }
}

void BotNetwork::start() {
    std::cout << "Starting bot network with " << bot_count_ << " bots..." << std::endl;
    std::cout << "Bots will send messages via HTTP API and receive via WebSocket." << std::endl;
    std::cout << "Note: Slack does NOT send bot's own messages back via WebSocket." << std::endl;
    std::cout << "      Bots will only receive messages from OTHER users/bots." << std::endl;
    std::cout << "      To test receiving, send a message manually to channel: " << channel_id_ << std::endl;
    
    // Create bot instances (for sending messages)
    for (int i = 0; i < bot_count_; ++i) {
        bots_.emplace_back(std::make_unique<SlackAPI>(bot_token_));
    }
    
    // Create WebSocket clients and get unique URL for each (Socket Mode v2 requirement)
    // Each WebSocket connection MUST have its own unique URL from apps.connections.open
    const char* app_token = std::getenv("SLACK_APP_TOKEN");
    if (!app_token) {
        std::cerr << "Warning: SLACK_APP_TOKEN not set. WebSocket clients will use fallback URL." << std::endl;
        std::cerr << "         For Socket Mode v2, each connection needs a unique URL." << std::endl;
    }
    
    int successful_connections = 0;
    for (int i = 0; i < bot_count_; ++i) {
        ws_clients_.emplace_back(std::make_unique<WebSocketClient>(bot_token_));
        
        // Get unique WebSocket URL for each bot (Socket Mode v2 requirement)
        if (app_token) {
            SlackAPI slack_api(app_token);
            std::string websocket_url;
            if (slack_api.get_socket_mode_url(websocket_url)) {
                ws_clients_[i]->set_websocket_url(websocket_url);
                successful_connections++;
                if (i < 3) {
                    std::cout << "[Setup] Bot " << i << " got unique WebSocket URL" << std::endl;
                }
            } else {
                std::cerr << "[Bot " << i << "] Failed to get WebSocket URL from apps.connections.open" << std::endl;
                std::cerr << "         This bot may not receive messages properly." << std::endl;
            }
            
            // Rate limiting: delay between API calls to avoid hitting Slack limits
            // apps.connections.open is rate-limited, so we add a small delay
            if (i < bot_count_ - 1) {
                std::this_thread::sleep_for(milliseconds(100));
            }
        }
    }
    
    if (app_token && successful_connections > 0) {
        std::cout << "[Setup] Successfully obtained WebSocket URLs for " << successful_connections 
                  << " out of " << bot_count_ << " bots" << std::endl;
    }
    
    // Setup message handlers for WebSocket clients BEFORE connecting
    for (int i = 0; i < bot_count_; ++i) {
        setup_websocket_handler(i);
        if (i < 3) {
            std::cout << "[Setup] Bot " << i << " handler configured" << std::endl;
        }
    }
    
    // Small delay to ensure handlers are set
    std::this_thread::sleep_for(milliseconds(100));
    
    // Start monitoring thread
    std::thread monitor(&BotNetwork::monitor_system, this);
    monitor.detach();
    
    // Start WebSocket receiver threads (for receiving messages)
    std::vector<std::thread> ws_threads;
    for (int i = 0; i < bot_count_; ++i) {
        ws_threads.emplace_back(&BotNetwork::websocket_receiver, this, i);
        // Stagger WebSocket connections: 100ms delay to avoid connection storm
        if (i < bot_count_ - 1) {
            std::this_thread::sleep_for(milliseconds(100));
        }
    }
    
    // Wait a bit for connections to establish and hello events to arrive
    std::this_thread::sleep_for(seconds(3));
    if (bot_count_ > 0) {
        std::cout << "[Setup] Connections established. If no 'Handler CALLED' messages above, handlers are not being invoked!" << std::endl;
    }
    
    // Start bot worker threads with staggered start to avoid thundering herd
    std::vector<std::thread> threads;
    for (int i = 0; i < bot_count_; ++i) {
        threads.emplace_back(&BotNetwork::bot_worker, this, i);
        // Stagger thread starts: 10ms delay between each bot to spread initial load
        if (i < bot_count_ - 1) {
            std::this_thread::sleep_for(milliseconds(10));
        }
    }
    
    // Run for specified duration
    const char* duration_env = std::getenv("BOT_NETWORK_DURATION");
    const int duration_sec = duration_env ? std::atoi(duration_env) : 60;
    std::this_thread::sleep_for(seconds(duration_sec));
    
    // Stop all bots
    running_.store(false);
    BotNetworkRateLimiter::get_instance().shutdown();
    
    // Signal all WebSocket clients to stop (thread-safe)
    for (auto& ws_client : ws_clients_) {
        ws_client->set_should_stop(true);
    }
    
    // Wait for bot worker threads first
    for (auto& t : threads) {
        t.join();
    }
    
    // Wait for WebSocket receiver threads to finish their run() loops
    // This ensures run() calls disconnect() internally before we destroy objects
    for (auto& t : ws_threads) {
        t.join();
    }
    
    // Now all WebSocket run() loops have finished and called disconnect() internally
    // Destructors will be safe - disconnect() checks if already disconnected
    
    // Reset rate limiter for next run
    BotNetworkRateLimiter::get_instance().reset();
    
    // Print final statistics
    print_statistics();
}

void BotNetwork::monitor_system() {
    auto start_time = steady_clock::now();
    auto last_check = start_time;
    
    while (running_.load()) {
        std::this_thread::sleep_for(seconds(10));
        
        auto now = steady_clock::now();
        auto elapsed = duration_cast<seconds>(now - start_time).count();
        
        size_t memory = get_process_memory();
        double cpu = get_cpu_usage();
        
        std::cout << "\n[Monitor] T+" << elapsed << "s | "
                  << "CPU: " << std::fixed << std::setprecision(2) << cpu << "% | "
                  << "Memory: " << (memory / 1024 / 1024) << " MB | "
                  << "Messages sent: " << messages_sent_.load() << " | "
                  << "Rate limit errors: " << rate_limit_errors_.load() << " | "
                  << "Messages received: " << messages_received_.load() << std::endl;
        
        last_check = now;
    }
}

void BotNetwork::print_statistics() {
    std::cout << "\n=== Bot Network Statistics ===" << std::endl;
    std::cout << "Bots: " << bot_count_ << std::endl;
    std::cout << "Total messages sent: " << messages_sent_.load() << std::endl;
    std::cout << "Rate limit errors: " << rate_limit_errors_.load() << std::endl;
    std::cout << "Total messages received: " << messages_received_.load() << std::endl;
    if (bot_count_ > 0) {
        std::cout << "Average messages per bot: " 
                  << (messages_sent_.load() / bot_count_) << std::endl;
    }
    
    size_t final_memory = get_process_memory();
    std::cout << "Final memory usage: " << (final_memory / 1024 / 1024) << " MB" << std::endl;
    
    if (rate_limit_errors_.load() > 0) {
        std::cout << "\n⚠️  Note: Rate limit errors occurred. Consider:" << std::endl;
        std::cout << "  - Reducing number of bots" << std::endl;
        std::cout << "  - Increasing delays between messages" << std::endl;
        std::cout << "  - Using fewer concurrent bots" << std::endl;
    }
}

} // namespace bot_network
