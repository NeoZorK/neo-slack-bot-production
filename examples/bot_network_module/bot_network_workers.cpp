#include "bot_network.hpp"
#include "bot_network_rate_limiter.hpp"
#include "../../src/event_parser.hpp"
#include "../../src/logger.hpp"
#include <iostream>
#include <random>
#include <sstream>
#include <thread>
#include <chrono>

using namespace std::chrono;

namespace bot_network {

void BotNetwork::setup_websocket_handler(int bot_id) {
    auto& ws_client = *ws_clients_[bot_id];
    
    // Verify handler is set (for debugging)
    if (bot_id < 3) {
        std::cout << "[Setup] Bot " << bot_id << " setting message handler..." << std::endl;
    }
    
    // Set up message handler
    ws_client.set_message_handler([this, bot_id](const std::string& message) {
        // Debug: Always print that handler was called (for first few bots)
        static std::atomic<int> total_events{0};
        int event_num = total_events.fetch_add(1);
        
        // ALWAYS print for first 5 bots to see if handler is called at all
        if (bot_id < 5) {
            std::cout << "[Bot " << bot_id << "] *** Handler CALLED! Event #" << event_num 
                      << ", msg length: " << message.length() << std::endl;
            if (message.length() < 300) {
                std::cout << "[Bot " << bot_id << "] Raw message: " << message << std::endl;
            } else {
                std::cout << "[Bot " << bot_id << "] Raw message (first 300): " 
                          << message.substr(0, 300) << "..." << std::endl;
            }
        }
        
        auto event_type = event_parser::parse_type(message);
        
        // Print hello events for all bots (they should arrive first)
        if (event_type == event_parser::event_type::hello) {
            std::cout << "[Bot " << bot_id << "] *** HELLO EVENT RECEIVED! ***" << std::endl;
        }
        
        // Debug: print all events for first few bots to understand what's happening
        if (bot_id < 5) {
            std::cout << "[Bot " << bot_id << "] Parsed event type: ";
            switch (event_type) {
                case event_parser::event_type::hello:
                    std::cout << "hello";
                    break;
                case event_parser::event_type::events_api:
                    std::cout << "events_api";
                    break;
                case event_parser::event_type::interactive:
                    std::cout << "interactive";
                    break;
                case event_parser::event_type::slash_commands:
                    std::cout << "slash_commands";
                    break;
                case event_parser::event_type::disconnect:
                    std::cout << "disconnect";
                    break;
                default:
                    std::cout << "unknown";
                    break;
            }
            std::cout << std::endl;
        }
        
        // Send acknowledgment for events with envelope_id
        if (event_type == event_parser::event_type::events_api ||
            event_type == event_parser::event_type::interactive ||
            event_type == event_parser::event_type::slash_commands) {
            auto envelope_id = event_parser::extract_envelope_id(message);
            if (envelope_id && ws_clients_[bot_id]->is_connected()) {
                std::string ack = "{\"envelope_id\":\"" + *envelope_id + "\"}";
                ws_clients_[bot_id]->send_message(ack);
            }
        }
        
        // Count messages from the target channel
        if (event_type == event_parser::event_type::events_api) {
            if (bot_id < 5) {
                std::cout << "[Bot " << bot_id << "] Processing events_api..." << std::endl;
            }
            
            bool is_message = event_parser::is_message_event(message);
            if (bot_id < 5) {
                std::cout << "[Bot " << bot_id << "] is_message_event: " << (is_message ? "YES" : "NO") << std::endl;
            }
            
            if (is_message) {
                auto channel = event_parser::extract_channel_id(message);
                auto text = event_parser::extract_message_text(message);
                
                // Print received message info with bot ID
                std::cout << "[Bot " << bot_id << "] *** MESSAGE RECEIVED! *** channel: " 
                          << (channel ? *channel : "N/A") 
                          << ", text: " << (text ? text->substr(0, 100) : "N/A")
                          << (text && text->length() > 100 ? "..." : "") << std::endl;
                
                // Count messages matching target channel
                if (channel) {
                    std::cout << "[Bot " << bot_id << "] Comparing channels: '" << *channel 
                              << "' == '" << channel_id_ << "' ? " 
                              << (*channel == channel_id_ ? "YES" : "NO") << std::endl;
                    
                    if (*channel == channel_id_) {
                        messages_received_++;
                        std::cout << "[Bot " << bot_id << "] ✓✓✓ Message COUNTED! (channel match: " 
                                  << *channel << ", total: " << messages_received_.load() << ")" << std::endl;
                    } else {
                        std::cout << "[Bot " << bot_id << "] ✗ Message ignored (channel mismatch: " 
                                  << *channel << " != " << channel_id_ << ")" << std::endl;
                    }
                } else {
                    std::cout << "[Bot " << bot_id << "] ✗ Message ignored (no channel ID extracted)" << std::endl;
                }
            } else {
                // Debug: show what type of events_api event it is
                if (bot_id < 5) {
                    auto payload = event_parser::extract_payload(message);
                    if (payload) {
                        std::cout << "[Bot " << bot_id << "] Payload extracted, length: " << payload->length() << std::endl;
                        // Try to extract event type from payload
                        size_t type_pos = payload->find("\"type\":");
                        if (type_pos != std::string::npos) {
                            size_t start = payload->find("\"", type_pos + 7) + 1;
                            size_t end = payload->find("\"", start);
                            if (end != std::string::npos && start < end) {
                                std::string event_type_str = payload->substr(start, end - start);
                                std::cout << "[Bot " << bot_id << "] events_api event type: " << event_type_str << std::endl;
                            }
                        } else {
                            std::cout << "[Bot " << bot_id << "] No 'type' field found in payload" << std::endl;
                        }
                    } else {
                        std::cout << "[Bot " << bot_id << "] Failed to extract payload" << std::endl;
                    }
                }
            }
        } else if (bot_id < 5) {
            std::cout << "[Bot " << bot_id << "] Event type is not events_api, skipping message processing" << std::endl;
        }
    });
    
    // Set up error handler
    ws_client.set_error_handler([bot_id](const std::string& error) {
        // Silently handle errors to avoid spam in console
        (void)bot_id;
        (void)error;
    });
}

void BotNetwork::websocket_receiver(int bot_id) {
    auto& ws_client = *ws_clients_[bot_id];
    
    // Verify handler is set before connecting
    if (bot_id < 3) {
        std::cout << "[Bot " << bot_id << "] Verifying handler is set before connect..." << std::endl;
    }
    
    // Connect to Slack
    if (!ws_client.connect()) {
        std::cerr << "[Bot " << bot_id << "] Failed to connect WebSocket" << std::endl;
        return; // Connection failed, thread exits
    }
    
    if (bot_id < 3) {
        std::cout << "[Bot " << bot_id << "] WebSocket connected successfully, starting event loop" << std::endl;
        std::cout << "[Bot " << bot_id << "] Waiting for events (hello should arrive first)..." << std::endl;
        std::cout << "[Bot " << bot_id << "] Handler is " << (ws_client.is_connected() ? "ready" : "NOT ready") << std::endl;
    }
    
    // Run WebSocket event loop (blocks until disconnect or stop)
    // This will call receive_message() in a loop and invoke message_handler_ when data arrives
    try {
        ws_client.run();
    } catch (const std::exception& e) {
        std::cerr << "[Bot " << bot_id << "] Exception in WebSocket run(): " << e.what() << std::endl;
    }
    
    if (bot_id < 3) {
        std::cout << "[Bot " << bot_id << "] WebSocket event loop ended" << std::endl;
    }
}

void BotNetwork::bot_worker(int bot_id) {
    auto& api = *bots_[bot_id];
    auto& rate_limiter = BotNetworkRateLimiter::get_instance();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    // Base delay between message attempts (will be coordinated by global rate limiter)
    std::uniform_int_distribution<> dis(500, 2000); // 0.5-2 seconds base delay
    
    int message_count = 0;
    const char* max_msg_env = std::getenv("BOT_NETWORK_MAX_MESSAGES");
    const int max_messages = max_msg_env ? std::atoi(max_msg_env) : 100; // Limit per bot
    
    while (running_.load() && message_count < max_messages) {
        // Wait for permission from global rate limiter
        if (!rate_limiter.wait_for_permission(channel_id_)) {
            break; // Shutdown requested
        }
        
        std::ostringstream msg;
        msg << "Bot " << bot_id << " message #" << message_count 
            << " at " << duration_cast<milliseconds>(
                system_clock::now().time_since_epoch()).count();
        
        std::string error_response;
        if (api.send_message(channel_id_, msg.str(), error_response)) {
            rate_limiter.record_sent(channel_id_);
            messages_sent_++;
            message_count++;
        } else {
            // Check if it's a rate limit error
            if (error_response.find("ratelimited") != std::string::npos ||
                error_response.find("rate_limited") != std::string::npos) {
                rate_limit_errors_++;
                
                // Extract retry_after from response if available
                int retry_after = 60; // Default 60 seconds
                size_t retry_pos = error_response.find("\"retry_after\":");
                if (retry_pos != std::string::npos) {
                    size_t start = error_response.find_first_of("0123456789", retry_pos);
                    if (start != std::string::npos) {
                        size_t end = error_response.find_first_not_of("0123456789", start);
                        if (end != std::string::npos) {
                            retry_after = std::stoi(error_response.substr(start, end - start));
                        }
                    }
                }
                
                // Notify global rate limiter about the rate limit
                rate_limiter.handle_rate_limit(channel_id_, retry_after);
            } else {
                // For other errors, wait a bit before retry
                std::this_thread::sleep_for(milliseconds(dis(gen) * 2));
            }
        }
        
        // Small random delay to spread out requests
        std::this_thread::sleep_for(milliseconds(dis(gen)));
    }
}

} // namespace bot_network
