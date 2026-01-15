/**
 * @file main.cpp
 * @brief Main application entry point
 * 
 * Application initialization and main event loop.
 * Initialization logic is split into main_init.cpp
 */

#include "main.hpp"
#include "websocket_client.hpp"
#include "slack_api.hpp"
#include "event_parser.hpp"
#include "message_filter.hpp"
#include "command_handler.hpp"
#include "notifications_factory.hpp"
#include "interfaces/i_notification_service.hpp"
#include "metrics.hpp"
#include "logger.hpp"
#include <iostream>
#include <csignal>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>

#define NEO_SLACK_MAIN_APP
std::atomic<bool> g_running(true);
WebSocketClient* g_ws_client = nullptr;

void signal_handler(int signal) {
    Logger::get_instance().info("Received signal " + std::to_string(signal) + ", shutting down...");
    g_running.store(false);
    // Set should_stop_ flag if ws_client is available for faster shutdown
    if (g_ws_client) {
        g_ws_client->set_should_stop(true);
    }
    // Don't call disconnect() here - it's unsafe from signal handler
    // The run() loop will check g_running and exit gracefully
}

int main(int /* argc */, char* /* argv */[]) {
    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Initialize metrics
    Metrics& metrics = Metrics::get_instance();
    metrics.reset();
    
    // Initialize application
    ConfigLoader config_loader;
    Settings settings;
    if (!initialize_application(config_loader, settings)) {
        std::cerr << "Failed to initialize application" << std::endl;
        return 1;
    }
    
    // Load tokens
    if (settings.bot_token.empty()) {
        std::cerr << "Error: SLACK_BOT_TOKEN is required" << std::endl;
        return 1;
    }
    
    if (settings.app_token.empty()) {
        std::cerr << "Error: SLACK_APP_TOKEN is required" << std::endl;
        std::cerr << "Without SLACK_APP_TOKEN, the application cannot connect to Slack Socket Mode" << std::endl;
        return 1;
    }
    
    // Initialize Slack API
    SlackAPI slack_api(settings.app_token);
    std::string websocket_url;
    if (!slack_api.get_socket_mode_url(websocket_url)) {
        std::cerr << "Failed to get WebSocket URL from Slack" << std::endl;
        return 1;
    }
    
    // Initialize WebSocket client
    WebSocketClient ws_client(settings.bot_token);
    ws_client.set_websocket_url(websocket_url);
    g_ws_client = &ws_client;
    
    // Setup message filter
    MessageFilter message_filter(settings);
    
    // Setup notifications using factory (platform-specific implementation)
    auto notifications = NotificationsFactory::create(settings);
    
    // Setup command handler
    CommandHandler command_handler;
    command_handler.register_command("status", [&](const std::string& channel, 
                                                    const std::vector<std::string>& /* args */) {
        std::string status = "Bot is running. Metrics: " + metrics.get_summary();
        slack_api.send_message(channel, status);
    });
    
    command_handler.register_command("metrics", [&](const std::string& channel,
                                                     const std::vector<std::string>& /* args */) {
        std::string metrics_summary = metrics.get_summary();
        slack_api.send_message(channel, metrics_summary);
    });
    
    // Setup message handler
    ws_client.set_message_handler([&](const std::string& message) {
        metrics.increment_event_count("message_received");
        metrics.update_memory_usage();
        
        auto event_type = event_parser::parse_type(message);
        
        if (event_type == event_parser::event_type::hello) {
            Logger::get_instance().info("Received hello event");
            metrics.increment_event_count("hello");
            return;
        }
        
        if (event_type == event_parser::event_type::events_api) {
            auto envelope_id = event_parser::extract_envelope_id(message);
            if (envelope_id) {
                ws_client.send_message("{\"envelope_id\":\"" + *envelope_id + "\"}");
            }
            
            if (event_parser::is_message_event(message)) {
                auto text = event_parser::extract_message_text(message);
                auto channel = event_parser::extract_channel_id(message);
                auto user = event_parser::extract_user_id(message);
                
                if (text && channel && user) {
                    if (message_filter.should_process(*channel, *text, *user)) {
                        notifications->show_notification("Slack Message", *text);
                        metrics.increment_event_count("notification_displayed");
                    }
                    
                    // Check for commands
                    if (text->find("/") == 0 || text->find("status") == 0 || 
                        text->find("metrics") == 0) {
                        command_handler.handle_command(*text, *channel, *user);
                    }
                }
            }
        }
        
        if (event_type == event_parser::event_type::disconnect) {
            Logger::get_instance().warn("Received disconnect event");
            metrics.increment_event_count("disconnect");
        }
    });
    
    // Start config watcher for hot reload
    config_loader.start_watching([&](const Settings& new_settings) {
        Logger::get_instance().info("Configuration changed, reloading settings...");
        
        // Update components
        message_filter.apply_settings(new_settings);
        if (notifications) {
            notifications->apply_settings(new_settings);
        }
        
        Logger::get_instance().info("Settings reloaded successfully");
    });
    
    // Run WebSocket client - it will check g_running internally
    ws_client.run();
    
    // Cleanup - disconnect() already called in run(), but safe to call again
    // However, to avoid double cleanup, we skip it here since run() already did it
    // ws_client.disconnect(); // Already called in run()
    
    Logger::get_instance().info("Application shutdown complete");
    return 0;
}
