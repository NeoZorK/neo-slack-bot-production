#ifndef MOCK_HTTP_CLIENT_HPP
#define MOCK_HTTP_CLIENT_HPP

#include <string>
#include <map>
#include <functional>
#include "../../src/interfaces/i_http_client.hpp"

class MockHttpClient : public IHttpClient {
public:
    MockHttpClient() : should_succeed_(true), response_("{\"ok\":true}") {}
    
    void set_should_succeed(bool value) { should_succeed_ = value; }
    void set_response(const std::string& response) { response_ = response; }
    void set_custom_handler(std::function<bool(const std::string&, std::string&)> handler) {
        custom_handler_ = handler;
    }
    
    bool get(const std::string& url, std::string& response) override {
        if (custom_handler_) {
            return custom_handler_(url, response);
        }
        response = response_;
        return should_succeed_;
    }
    
    bool post(const std::string& url, const std::string& /* data */, 
              std::string& response) override {
        if (custom_handler_) {
            return custom_handler_(url, response);
        }
        response = response_;
        return should_succeed_;
    }
    
    void set_header(const std::string& key, const std::string& value) override {
        headers_[key] = value;
    }
    
    void set_timeout(int seconds) override {
        timeout_seconds_ = seconds;
    }
    
    void reset() {
        should_succeed_ = true;
        response_ = "{\"ok\":true}";
        custom_handler_ = nullptr;
        headers_.clear();
        timeout_seconds_ = 30;
    }
    
    const std::map<std::string, std::string>& get_headers() const {
        return headers_;
    }
    
private:
    bool should_succeed_;
    std::string response_;
    std::function<bool(const std::string&, std::string&)> custom_handler_;
    std::map<std::string, std::string> headers_;
    int timeout_seconds_ = 30;
};

#endif // MOCK_HTTP_CLIENT_HPP
