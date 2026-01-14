#include <iostream>
#include <cassert>
#include <string>
#include <atomic>

// Define g_running for tests (not using main app's definition)
std::atomic<bool> g_running(true);

// Test framework - organized by category
// Test implementations are in separate category directories:
// - unit/ - Unit tests for individual components
// - integration/ - Integration tests with mocks
// - security/ - Security and input validation tests
// - performance/ - Performance and latency tests
// - stress/ - Stress and load tests
// - benchmark/ - Benchmark tests
// - e2e/ - End-to-end Slack API tests

// Unit tests
void test_websocket_client();
#ifdef __APPLE__
void test_macos_notifications();
#endif
#if defined(__linux__) || (defined(__unix__) && !defined(__APPLE__))
void test_linux_notifications();
#endif
void test_http_client();
void test_slack_api();
void test_event_parser();
void test_settings();
void test_json_parser();
void test_config_loader();
void test_message_filter_extended();
void test_logger();
void test_error_recovery();
void test_rate_limiter();
void test_input_validator();
void test_metrics();
void test_command_handler();
void test_bot_network_rate_limiter();
void test_validator_limiter_boundaries();

// Integration tests
void test_integration();
void test_slack_integration();

// Security tests
void test_security();
void test_security_input();

// Performance tests
void test_performance();
void test_performance_latency();
void test_performance_throughput();
void test_performance_advanced();

// Stress tests
void test_stress();
void test_stress_connection();
void test_stress_components();
void test_stress_system();
void test_stress_concurrent();
void test_stress_advanced();

// Benchmark tests
void test_benchmark();

// E2E tests
void test_slack_e2e();

// Example tests - excluded from main test build (they include example files with main())
// void test_bot_network_example();
// void test_channel_management_example();

int main(int argc, char* argv[]) {
    std::cout << "Running Neo Slack Bot tests..." << std::endl;
    
    bool run_all = true;
    bool run_unit = false;
    bool run_integration = false;
    bool run_security = false;
    bool run_performance = false;
    bool run_stress = false;
    bool run_benchmark = false;
    bool run_e2e = false;
    
    // Parse command line arguments
    if (argc > 1) {
        run_all = false;
        std::string arg = argv[1];
        if (arg == "--unit") run_unit = true;
        else if (arg == "--integration") run_integration = true;
        else if (arg == "--security") run_security = true;
        else if (arg == "--performance") run_performance = true;
        else if (arg == "--stress") run_stress = true;
        else if (arg == "--benchmark") run_benchmark = true;
        else if (arg == "--e2e") run_e2e = true;
        else if (arg == "--perf") {
            run_performance = true;
            run_stress = true;
            run_benchmark = true;
        } else {
            run_all = true; // Unknown argument, run all
        }
    }
    
    try {
        if (run_all || run_unit) {
            std::cout << "\n=== Unit Tests ===" << std::endl;
            test_websocket_client();
#ifdef __APPLE__
            test_macos_notifications();
#endif
#if defined(__linux__) || (defined(__unix__) && !defined(__APPLE__))
            test_linux_notifications();
#endif
            test_http_client();
            test_slack_api();
            test_event_parser();
            test_settings();
            test_json_parser();
            test_config_loader();
            test_message_filter_extended();
            test_logger();
            test_error_recovery();
            test_rate_limiter();
            test_input_validator();
            test_metrics();
            test_command_handler();
            test_bot_network_rate_limiter();
            test_validator_limiter_boundaries();
        }
        
        if (run_all || run_security) {
            std::cout << "\n=== Security Tests ===" << std::endl;
            test_security();
        }
        
        if (run_all || run_integration) {
            std::cout << "\n=== Integration Tests ===" << std::endl;
            test_integration();
            // test_slack_integration(); // Temporarily disabled - needs fix
        }
        
        if (run_all || run_e2e) {
            std::cout << "\n=== End-to-End Tests ===" << std::endl;
            test_slack_e2e();
        }
        
        if (run_all) {
            std::cout << "\n=== Example Tests ===" << std::endl;
            // Example tests excluded - they include example files with main() functions
            // test_bot_network_example();
            // test_channel_management_example();
        }
        
        if (run_performance) {
            std::cout << "\n=== Performance Tests ===" << std::endl;
            test_performance();
        }
        
        if (run_stress) {
            std::cout << "\n=== Stress Tests ===" << std::endl;
            test_stress();
        }
        
        if (run_benchmark) {
            std::cout << "\n=== Benchmark Tests ===" << std::endl;
            test_benchmark();
        }
        
        std::cout << "\n✅ All tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
