#include <iostream>
#include <cassert>
#include <vector>
#include "../src/command_handler.hpp"

void test_command_handler_creation() {
    CommandHandler handler;
    auto commands = handler.get_available_commands();
    assert(commands.empty());
    std::cout << "  ✓ CommandHandler creation test passed" << std::endl;
}

void test_command_handler_register_command() {
    CommandHandler handler;
    bool callback_called = false;
    
    handler.register_command("test", [&callback_called](const std::string& channel, 
                                                       const std::vector<std::string>& /* args */) {
        callback_called = true;
        assert(!channel.empty());
    });
    
    auto commands = handler.get_available_commands();
    assert(commands.size() == 1);
    assert(commands[0] == "test");
    
    std::cout << "  ✓ CommandHandler register_command test passed" << std::endl;
}

void test_command_handler_handle_command() {
    CommandHandler handler;
    bool callback_called = false;
    std::string received_channel;
    std::vector<std::string> received_args;
    
    handler.register_command("echo", [&callback_called, &received_channel, &received_args](
        const std::string& channel, const std::vector<std::string>& args) {
        callback_called = true;
        received_channel = channel;
        received_args = args;
    });
    
    assert(handler.handle_command("echo hello world", "C123", "U456"));
    assert(callback_called);
    assert(received_channel == "C123");
    assert(received_args.size() == 2);
    assert(received_args[0] == "hello");
    assert(received_args[1] == "world");
    
    std::cout << "  ✓ CommandHandler handle_command test passed" << std::endl;
}

void test_command_handler_handle_command_with_slash() {
    CommandHandler handler;
    bool callback_called = false;
    
    handler.register_command("status", [&callback_called](const std::string&, 
                                                         const std::vector<std::string>&) {
        callback_called = true;
    });
    
    assert(handler.handle_command("/status", "C123", "U456"));
    assert(callback_called);
    
    std::cout << "  ✓ CommandHandler handle_command with slash test passed" << std::endl;
}

void test_command_handler_handle_unknown_command() {
    CommandHandler handler;
    
    assert(!handler.handle_command("unknown", "C123", "U456"));
    assert(!handler.handle_command("", "C123", "U456"));
    
    std::cout << "  ✓ CommandHandler handle_unknown_command test passed" << std::endl;
}

void test_command_handler_multiple_commands() {
    CommandHandler handler;
    int callback1_count = 0;
    int callback2_count = 0;
    
    handler.register_command("cmd1", [&callback1_count](const std::string&, 
                                                        const std::vector<std::string>&) {
        callback1_count++;
    });
    
    handler.register_command("cmd2", [&callback2_count](const std::string&, 
                                                        const std::vector<std::string>&) {
        callback2_count++;
    });
    
    auto commands = handler.get_available_commands();
    assert(commands.size() == 2);
    
    assert(handler.handle_command("cmd1", "C123", "U456"));
    assert(callback1_count == 1);
    assert(callback2_count == 0);
    
    assert(handler.handle_command("cmd2", "C123", "U456"));
    assert(callback1_count == 1);
    assert(callback2_count == 1);
    
    std::cout << "  ✓ CommandHandler multiple commands test passed" << std::endl;
}

void test_command_handler_args_parsing() {
    CommandHandler handler;
    std::vector<std::string> received_args;
    
    handler.register_command("test", [&received_args](const std::string&, 
                                                      const std::vector<std::string>& args) {
        received_args = args;
    });
    
    handler.handle_command("test arg1 arg2 arg3", "C123", "U456");
    assert(received_args.size() == 3);
    assert(received_args[0] == "arg1");
    assert(received_args[1] == "arg2");
    assert(received_args[2] == "arg3");
    
    handler.handle_command("test", "C123", "U456");
    assert(received_args.empty());
    
    std::cout << "  ✓ CommandHandler args parsing test passed" << std::endl;
}

// Export for test_main.cpp
void test_command_handler() {
    std::cout << "\n=== CommandHandler Tests ===" << std::endl;
    
    test_command_handler_creation();
    test_command_handler_register_command();
    test_command_handler_handle_command();
    test_command_handler_handle_command_with_slash();
    test_command_handler_handle_unknown_command();
    test_command_handler_multiple_commands();
    test_command_handler_args_parsing();
    
    std::cout << "All CommandHandler tests passed!" << std::endl;
}
