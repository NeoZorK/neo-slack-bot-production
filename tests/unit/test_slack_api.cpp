#include <iostream>
#include <cassert>
#include <memory>
#include "../src/slack_api.hpp"
#include "../mocks/mock_http_client.hpp"

// Helper to create SlackAPI with mock client
std::pair<std::unique_ptr<SlackAPI>, MockHttpClient*> create_api_with_mock() {
    auto mock = std::make_unique<MockHttpClient>();
    auto* mock_ptr = mock.get();
    auto api = std::make_unique<SlackAPI>("xoxb-test-token", std::move(mock));
    return {std::move(api), mock_ptr};
}

void test_slack_api_creation() {
    auto [api, mock] = create_api_with_mock();
    assert(api != nullptr);
    
    // Check if Authorization header is set correctly
    const auto& headers = mock->get_headers();
    assert(headers.count("Authorization") > 0);
    assert(headers.at("Authorization") == "Bearer xoxb-test-token");
    assert(headers.count("Content-Type") > 0);
    assert(headers.at("Content-Type") == "application/json");
    
    std::cout << "test_slack_api_creation passed" << std::endl;
}

void test_slack_api_send_message() {
    auto [api, mock] = create_api_with_mock();
    
    // Success case
    mock->set_response("{\"ok\":true, \"ts\":\"1234567890.123456\"}");
    bool result = api->send_message("C12345678", "Hello world");
    assert(result == true);
    
    // Failure case (API error)
    mock->set_response("{\"ok\":false, \"error\":\"channel_not_found\"}");
    std::string error_response;
    result = api->send_message("C12345678", "Hello world", error_response);
    assert(result == false);
    assert(error_response.find("channel_not_found") != std::string::npos);
    
    // Failure case (Network error)
    mock->set_should_succeed(false);
    result = api->send_message("C12345678", "Hello world");
    assert(result == false);
    
    std::cout << "test_slack_api_send_message passed" << std::endl;
}

void test_slack_api_get_channel_info() {
    auto [api, mock] = create_api_with_mock();
    
    // Success case
    mock->set_response("{\"ok\":true, \"channel\":{\"id\":\"C12345678\",\"name\":\"general\"}}");
    std::string info;
    bool result = api->get_channel_info("C12345678", info);
    assert(result == true);
    assert(info.find("\"name\":\"general\"") != std::string::npos);
    
    // Failure case
    mock->set_response("{\"ok\":false, \"error\":\"channel_not_found\"}");
    result = api->get_channel_info("C12345678", info);
    assert(result == false);
    
    std::cout << "test_slack_api_get_channel_info passed" << std::endl;
}

void test_slack_api_get_user_info() {
    auto [api, mock] = create_api_with_mock();
    
    // Success case
    mock->set_response("{\"ok\":true, \"user\":{\"id\":\"U12345678\",\"name\":\"user\"}}");
    std::string info;
    bool result = api->get_user_info("U12345678", info);
    assert(result == true);
    assert(info.find("\"name\":\"user\"") != std::string::npos);
    
    // Invalid user ID format check (client-side validation)
    result = api->get_user_info("invalid-id", info);
    assert(result == false);
    
    std::cout << "test_slack_api_get_user_info passed" << std::endl;
}

void test_slack_api_socket_mode() {
    auto [api, mock] = create_api_with_mock();
    
    // apps.connections.open requires specific handling
    // It changes Content-Type to form-urlencoded temporarily
    
    mock->set_response("{\"ok\":true, \"url\":\"wss://wss-primary.slack.com/link\"}");
    std::string url;
    bool result = api->get_socket_mode_url(url);
    
    assert(result == true);
    assert(url == "wss://wss-primary.slack.com/link");
    
    // Verify Content-Type was restored (though mock doesn't track history, 
    // current state should be json)
    const auto& headers = mock->get_headers();
    assert(headers.at("Content-Type") == "application/json");
    
    std::cout << "test_slack_api_socket_mode passed" << std::endl;
}

// Export for test_main.cpp
void test_slack_api() {
    test_slack_api_creation();
    test_slack_api_send_message();
    test_slack_api_get_channel_info();
    test_slack_api_get_user_info();
    test_slack_api_socket_mode();
}
