#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <thread>
#include <vector>
#include "../src/logger.hpp"

void test_logger_levels() {
    Logger& logger = Logger::get_instance();
    
    logger.set_level(LogLevel::DEBUG);
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warn("Warn message");
    logger.error("Error message");
    
    logger.set_level(LogLevel::INFO);
    logger.debug("This debug should not appear");
    logger.info("This info should appear");
    
    logger.set_level(LogLevel::WARN);
    logger.info("This info should not appear");
    logger.warn("This warn should appear");
    
    logger.set_level(LogLevel::ERR);
    logger.warn("This warn should not appear");
    logger.error("This error should appear");
    
    std::cout << "  ✓ Logger levels test passed" << std::endl;
}

void test_logger_file_output() {
    std::string test_log_file = "/tmp/test_neo_slack_bot.log";
    
    if (std::filesystem::exists(test_log_file)) {
        std::filesystem::remove(test_log_file);
    }
    
    Logger& logger = Logger::get_instance();
    logger.set_file_path(test_log_file);
    logger.set_level(LogLevel::DEBUG);
    logger.set_console_output(false);
    
    logger.info("Test log message");
    logger.error("Test error message");
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    assert(std::filesystem::exists(test_log_file));
    
    std::ifstream file(test_log_file);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    assert(content.find("Test log message") != std::string::npos);
    assert(content.find("Test error message") != std::string::npos);
    assert(content.find("[INFO ]") != std::string::npos);
    assert(content.find("[ERROR]") != std::string::npos);
    
    file.close();
    logger.set_file_path(""); // Close file handle
    
    // Give OS time to release file handle (especially on Windows/Wine)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    try {
        if (std::filesystem::exists(test_log_file)) {
            std::filesystem::remove(test_log_file);
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Warning: Failed to remove log file: " << e.what() << std::endl;
    }
    
    logger.set_console_output(true);
    
    std::cout << "  ✓ Logger file output test passed" << std::endl;
}

void test_logger_json_format() {
    std::string test_log_file = "/tmp/test_neo_slack_bot_json.log";

    try {
        if (std::filesystem::exists(test_log_file)) {
            std::filesystem::remove(test_log_file);
        }
    } catch (const std::filesystem::filesystem_error& e) {
        // Ignore removal error at start of test
    }

    Logger& logger = Logger::get_instance();
    logger.set_file_path(test_log_file);
    logger.set_level(LogLevel::INFO);
    logger.set_console_output(false);
    logger.set_format(LogFormat::JSON);

    std::string message = "Json test \"quotes\" and newline\nline";
    logger.info(message);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    assert(std::filesystem::exists(test_log_file));

    std::ifstream file(test_log_file);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    assert(content.find("{") != std::string::npos);
    assert(content.find("\"ts\"") != std::string::npos);
    assert(content.find("\"level\"") != std::string::npos);
    assert(content.find("\"message\"") != std::string::npos);
    assert(content.find("[INFO ]") == std::string::npos);

    file.close();
    logger.set_file_path(""); // Close file handle
    logger.set_format(LogFormat::PLAIN);
    logger.set_console_output(true);
    std::filesystem::remove(test_log_file);
    
    std::cout << "  ✓ Logger JSON format test passed" << std::endl;
}

void test_logger_thread_safety() {
    Logger& logger = Logger::get_instance();
    logger.set_level(LogLevel::INFO);
    
    std::vector<std::thread> threads;
    const int thread_count = 10;
    const int messages_per_thread = 100;
    
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back([&logger, i]() {
            for (int j = 0; j < messages_per_thread; ++j) {
                logger.info("Thread " + std::to_string(i) + " message " + std::to_string(j));
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "  ✓ Logger thread safety test passed" << std::endl;
}

void test_logger_formatting() {
    Logger& logger = Logger::get_instance();
    logger.set_level(LogLevel::INFO);
    
    logger.info("Simple message");
    logger.info("Message with special chars: !@#$%^&*()");
    logger.info("Message with numbers: 1234567890");
    logger.info("Message with unicode: тест");
    
    std::cout << "  ✓ Logger formatting test passed" << std::endl;
}

void test_logger() {
    std::cout << "Testing Logger..." << std::endl;
    
    test_logger_levels();
    test_logger_file_output();
    test_logger_thread_safety();
    test_logger_formatting();
    test_logger_json_format();
    
    std::cout << "Logger tests passed!" << std::endl;
}
