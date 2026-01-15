#include <iostream>
#include <cassert>
#include <string>
#include <atomic>
#include "../src/websocket_client.hpp"
#include "../mocks/mock_websocket_client.hpp"

void test_websocket_creation() {
    WebSocketClient client("test-token");
    assert(!client.is_connected());
    std::cout << "  ✓ WebSocket creation test passed" << std::endl;
}

void test_websocket_setters_and_state() {
    WebSocketClient client("test-token");
    client.set_websocket_url("wss://wss-primary.slack.com/link");
    client.set_should_stop(true);
    assert(!client.is_connected());
    std::cout << "  ✓ WebSocket setters/state test passed" << std::endl;
}

void test_mock_websocket_message_flow() {
    MockWebSocketClient mock_ws;
    mock_ws.set_connect_result(true);
    
    std::atomic<bool> message_received{false};
    mock_ws.set_message_handler([&message_received](const std::string& msg) {
        message_received.store(true);
        assert(msg.find("hello") != std::string::npos);
    });
    
    assert(mock_ws.connect());
    assert(mock_ws.is_connected());
    
    mock_ws.simulate_message("{\"type\":\"hello\"}");
    assert(message_received.load());
    
    std::cout << "  ✓ Mock WebSocket message flow test passed" << std::endl;
}

void test_mock_websocket_error_flow() {
    MockWebSocketClient mock_ws;
    
    std::atomic<bool> error_received{false};
    mock_ws.set_error_handler([&error_received](const std::string& error) {
        error_received.store(true);
        assert(error.find("error") != std::string::npos);
    });
    
    mock_ws.simulate_error("test error");
    assert(error_received.load());
    
    std::cout << "  ✓ Mock WebSocket error flow test passed" << std::endl;
}

void test_mock_websocket_send_message() {
    MockWebSocketClient mock_ws;
    mock_ws.set_connect_result(true);
    
    assert(mock_ws.connect());
    assert(mock_ws.is_connected());
    assert(mock_ws.send_message("test message"));
    
    const auto& messages = mock_ws.get_messages_sent();
    assert(messages.size() == 1);
    assert(messages[0] == "test message");
    
    std::cout << "  ✓ Mock WebSocket send message test passed" << std::endl;
}

// Export for test_main.cpp
void test_websocket_client() {
    std::cout << "Testing WebSocket client..." << std::endl;
    test_websocket_creation();
    test_websocket_setters_and_state();
    test_mock_websocket_message_flow();
    test_mock_websocket_error_flow();
    test_mock_websocket_send_message();
    std::cout << "WebSocket client tests passed!" << std::endl;
}
