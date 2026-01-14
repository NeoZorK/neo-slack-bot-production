#ifndef MOCK_WEBSOCKET_CLIENT_HPP
#define MOCK_WEBSOCKET_CLIENT_HPP

#include <string>
#include <functional>
#include <vector>
#include "../../src/interfaces/i_websocket_client.hpp"

class MockWebSocketClient : public IWebSocketClient {
public:
    MockWebSocketClient() : connected_(false), auto_connect_(false), 
                           connect_result_(false), send_result_(true) {}
    
    bool connect() override {
        if (auto_connect_) {
            connected_ = true;
            return true;
        }
        connected_ = connect_result_;
        return connect_result_;
    }
    
    void disconnect() override {
        connected_ = false;
    }
    
    void run() override {
        // Mock implementation - does nothing
    }
    
    bool is_connected() const override {
        return connected_;
    }
    
    void set_message_handler(std::function<void(const std::string&)> handler) override {
        message_handler_ = handler;
    }
    
    void set_error_handler(std::function<void(const std::string&)> handler) override {
        error_handler_ = handler;
    }
    
    bool send_message(const std::string& message) override {
        messages_sent_.push_back(message);
        return send_result_;
    }
    
    void set_connect_result(bool result) { 
        connect_result_ = result; 
        auto_connect_ = result;
    }
    
    void set_send_result(bool result) { send_result_ = result; }
    
    void simulate_message(const std::string& message) {
        if (message_handler_) {
            message_handler_(message);
        }
    }
    
    void simulate_error(const std::string& error) {
        if (error_handler_) {
            error_handler_(error);
        }
    }
    
    const std::vector<std::string>& get_messages_sent() const {
        return messages_sent_;
    }
    
    void reset() {
        connected_ = false;
        messages_sent_.clear();
        connect_result_ = false;
        send_result_ = true;
        auto_connect_ = false;
        message_handler_ = nullptr;
        error_handler_ = nullptr;
    }
    
private:
    bool connected_;
    bool auto_connect_;
    bool connect_result_;
    bool send_result_;
    std::vector<std::string> messages_sent_;
    std::function<void(const std::string&)> message_handler_;
    std::function<void(const std::string&)> error_handler_;
};

#endif // MOCK_WEBSOCKET_CLIENT_HPP
