#include <iostream>

// Forward declarations from stress test modules
void test_stress_connection();
void test_stress_components();
void test_stress_system();
void test_stress_advanced();


// Export for test_main.cpp
void test_stress() {
    std::cout << "\n=== Stress Tests ===" << std::endl;
    
    std::cout << "\n--- Connection Stress Tests ---" << std::endl;
    test_stress_connection();
    
    std::cout << "\n--- Component Stress Tests ---" << std::endl;
    test_stress_components();
    
    std::cout << "\n--- System Stress Tests ---" << std::endl;
    test_stress_system();
    
    std::cout << "\n--- Advanced Stress Tests ---" << std::endl;
    test_stress_advanced();
    
    std::cout << "\nAll stress tests passed!" << std::endl;
}
