#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>
#include "../src/error_recovery.hpp"

void test_error_recovery_categorization() {
    ErrorRecovery recovery;
    
    recovery.handle_error(ErrorType::NETWORK_ERROR, "Network timeout");
    recovery.handle_error(ErrorType::AUTHENTICATION_ERROR, "Invalid token");
    recovery.handle_error(ErrorType::API_ERROR, "API request failed");
    recovery.handle_error(ErrorType::PARSING_ERROR, "JSON parse error");
    recovery.handle_error(ErrorType::RATE_LIMIT_ERROR, "Rate limit exceeded");
    
    assert(recovery.get_error_count(ErrorType::NETWORK_ERROR) == 1);
    assert(recovery.get_error_count(ErrorType::AUTHENTICATION_ERROR) == 1);
    assert(recovery.get_error_count(ErrorType::API_ERROR) == 1);
    assert(recovery.get_error_count(ErrorType::PARSING_ERROR) == 1);
    assert(recovery.get_error_count(ErrorType::RATE_LIMIT_ERROR) == 1);
    
    std::cout << "  ✓ Error recovery categorization test passed" << std::endl;
}

void test_error_recovery_strategies() {
    ErrorRecovery recovery;
    
    recovery.set_recovery_strategy(ErrorType::NETWORK_ERROR, RecoveryStrategy::RETRY);
    recovery.set_recovery_strategy(ErrorType::AUTHENTICATION_ERROR, RecoveryStrategy::FAIL_FAST);
    recovery.set_recovery_strategy(ErrorType::PARSING_ERROR, RecoveryStrategy::IGNORE);
    
    recovery.handle_error(ErrorType::NETWORK_ERROR, "Test");
    assert(recovery.should_retry(ErrorType::NETWORK_ERROR));
    
    recovery.handle_error(ErrorType::AUTHENTICATION_ERROR, "Test");
    assert(!recovery.should_retry(ErrorType::AUTHENTICATION_ERROR));
    
    recovery.handle_error(ErrorType::PARSING_ERROR, "Test");
    assert(recovery.should_retry(ErrorType::PARSING_ERROR));
    
    std::cout << "  ✓ Error recovery strategies test passed" << std::endl;
}

void test_error_recovery_retry_delay() {
    ErrorRecovery recovery;
    recovery.set_initial_retry_delay(1);
    
    // Test exponential backoff (default)
    int delay1 = recovery.get_retry_delay(ErrorType::API_ERROR, 0);
    int delay2 = recovery.get_retry_delay(ErrorType::API_ERROR, 1);
    int delay3 = recovery.get_retry_delay(ErrorType::API_ERROR, 2);
    
    assert(delay1 == 1);
    assert(delay2 == 2);
    assert(delay3 == 4);
    
    // Test linear retry (RETRY strategy)
    recovery.set_recovery_strategy(ErrorType::NETWORK_ERROR, RecoveryStrategy::RETRY);
    int delay_linear1 = recovery.get_retry_delay(ErrorType::NETWORK_ERROR, 0);
    int delay_linear2 = recovery.get_retry_delay(ErrorType::NETWORK_ERROR, 5);
    
    assert(delay_linear1 == 1);
    assert(delay_linear2 == 1); // Should be constant
    
    std::cout << "  ✓ Error recovery retry delay test passed" << std::endl;
}

void test_error_recovery_max_retries() {
    ErrorRecovery recovery;
    recovery.set_max_retries(3);
    
    for (int i = 0; i < 3; ++i) {
        recovery.handle_error(ErrorType::API_ERROR, "Test error");
        assert(recovery.should_retry(ErrorType::API_ERROR));
    }
    
    recovery.handle_error(ErrorType::API_ERROR, "Test error");
    assert(!recovery.should_retry(ErrorType::API_ERROR));
    
    std::cout << "  ✓ Error recovery max retries test passed" << std::endl;
}

void test_error_recovery_reset() {
    ErrorRecovery recovery;
    
    recovery.handle_error(ErrorType::NETWORK_ERROR, "Test");
    recovery.handle_error(ErrorType::NETWORK_ERROR, "Test");
    assert(recovery.get_error_count(ErrorType::NETWORK_ERROR) == 2);
    
    recovery.reset_error_count(ErrorType::NETWORK_ERROR);
    assert(recovery.get_error_count(ErrorType::NETWORK_ERROR) == 0);
    
    std::cout << "  ✓ Error recovery reset test passed" << std::endl;
}

void test_error_recovery_thread_safety() {
    ErrorRecovery recovery;
    const int num_threads = 4;
    const int operations_per_thread = 100;
    std::vector<std::thread> threads;
    
    // Test concurrent error handling
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&recovery]() {
            for (int j = 0; j < operations_per_thread; ++j) {
                ErrorType type = static_cast<ErrorType>(j % 7);
                recovery.handle_error(type, "Test error");
                recovery.get_error_count(type);
                recovery.should_retry(type);
            }
        });
    }
    
    // Test concurrent configuration
    std::thread config_thread([&recovery]() {
        for (int i = 0; i < 50; ++i) {
            recovery.set_max_retries(i % 10 + 1);
            recovery.set_initial_retry_delay(i % 5 + 1);
            recovery.set_recovery_strategy(ErrorType::NETWORK_ERROR, 
                                          static_cast<RecoveryStrategy>(i % 4));
        }
    });
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    config_thread.join();
    
    // Verify final state is consistent
    int total_errors = 0;
    for (int i = 0; i < 7; ++i) {
        ErrorType type = static_cast<ErrorType>(i);
        int count = recovery.get_error_count(type);
        total_errors += count;
        assert(count >= 0);
    }
    
    // Should have handled errors from all threads
    assert(total_errors > 0);
    
    std::cout << "  ✓ Error recovery thread safety test passed" << std::endl;
}

void test_error_recovery() {
    std::cout << "Testing ErrorRecovery..." << std::endl;
    
    test_error_recovery_categorization();
    test_error_recovery_strategies();
    test_error_recovery_retry_delay();
    test_error_recovery_max_retries();
    test_error_recovery_reset();
    test_error_recovery_thread_safety();
    
    std::cout << "ErrorRecovery tests passed!" << std::endl;
}
