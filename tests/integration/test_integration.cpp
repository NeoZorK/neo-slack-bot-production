#include <iostream>
#include <cassert>
#include "../mocks/mock_http_client.hpp"
#include "../mocks/mock_websocket_client.hpp"
#include "../src/slack_api.hpp"
#include "../src/event_parser.hpp"

void test_slack_api_with_mock() {
    MockHttpClient mock_client;
    mock_client.set_response("{\"ok\":true,\"channel\":\"C123\",\"ts\":\"1234567890.123456\"}");
    
    SlackAPI api("test-token");
    // Note: SlackAPI uses HttpClient internally, so this test demonstrates
    // the concept. Full integration would require refactoring SlackAPI to use IHttpClient.
    
    std::cout << "  ✓ SlackAPI mock integration test structure created" << std::endl;
}

void test_integration_websocket_message_flow() {
    MockWebSocketClient mock_ws;
    mock_ws.set_connect_result(true);
    
    bool message_received = false;
    mock_ws.set_message_handler([&message_received](const std::string& msg) {
        message_received = true;
        assert(msg.find("hello") != std::string::npos);
    });
    
    assert(mock_ws.connect());
    assert(mock_ws.is_connected());
    
    mock_ws.simulate_message("{\"type\":\"hello\"}");
    assert(message_received);
    
    std::cout << "  ✓ WebSocket message flow test passed" << std::endl;
}

void test_integration_websocket_error_handling() {
    MockWebSocketClient mock_ws;
    
    bool error_received = false;
    mock_ws.set_error_handler([&error_received](const std::string& error) {
        error_received = true;
        assert(error.find("test error") != std::string::npos);
    });
    
    mock_ws.simulate_error("test error occurred");
    assert(error_received);
    
    std::cout << "  ✓ WebSocket error handling test passed" << std::endl;
}

void test_integration_websocket_send_message() {
    MockWebSocketClient mock_ws;
    mock_ws.set_connect_result(true);
    
    assert(mock_ws.connect());
    assert(mock_ws.send_message("test message"));
    
    const auto& messages = mock_ws.get_messages_sent();
    assert(messages.size() == 1);
    assert(messages[0] == "test message");
    
    std::cout << "  ✓ WebSocket send message test passed" << std::endl;
}

void test_http_client_mock() {
    MockHttpClient mock_client;
    
    std::string response;
    assert(mock_client.get("https://api.slack.com/test", response));
    assert(response == "{\"ok\":true}");
    
    mock_client.set_response("{\"ok\":false,\"error\":\"invalid_auth\"}");
    assert(mock_client.post("https://api.slack.com/test", "{}", response));
    assert(response.find("invalid_auth") != std::string::npos);
    
    std::cout << "  ✓ HTTP client mock test passed" << std::endl;
}

void test_http_client_custom_handler() {
    MockHttpClient mock_client;
    
    mock_client.set_custom_handler([](const std::string& url, std::string& response) {
        if (url.find("error") != std::string::npos) {
            response = "{\"ok\":false}";
            return false;
        }
        response = "{\"ok\":true}";
        return true;
    });
    
    std::string response;
    assert(mock_client.get("https://api.slack.com/success", response));
    assert(!mock_client.get("https://api.slack.com/error", response));
    
    std::cout << "  ✓ HTTP client custom handler test passed" << std::endl;
}

void test_integration() {
    std::cout << "\n=== Integration Tests ===" << std::endl;
    
    test_http_client_mock();
    test_http_client_custom_handler();
    test_integration_websocket_message_flow();
    test_integration_websocket_error_handling();
    test_integration_websocket_send_message();
    test_slack_api_with_mock();
    
    std::cout << "All integration tests passed!" << std::endl;
}
