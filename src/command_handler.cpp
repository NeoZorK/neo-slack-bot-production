#include "command_handler.hpp"
#include <sstream>
#include <algorithm>

CommandHandler::CommandHandler() {
}

CommandHandler::~CommandHandler() {
}

void CommandHandler::register_command(const std::string& command, 
                                      CommandCallback callback) {
    commands_[command] = callback;
}

std::vector<std::string> CommandHandler::parse_command_args(
    const std::string& text) {
    std::vector<std::string> args;
    std::istringstream iss(text);
    std::string arg;
    
    while (iss >> arg) {
        args.push_back(arg);
    }
    
    return args;
}

bool CommandHandler::handle_command(const std::string& command_text,
                                   const std::string& channel_id,
                                   const std::string& /* user_id */) {
    auto args = parse_command_args(command_text);
    if (args.empty()) {
        return false;
    }
    
    std::string command_name = args[0];
    if (command_name[0] == '/') {
        command_name = command_name.substr(1);
    }
    
    auto it = commands_.find(command_name);
    if (it != commands_.end()) {
        std::vector<std::string> command_args(args.begin() + 1, args.end());
        it->second(channel_id, command_args);
        return true;
    }
    
    return false;
}

std::vector<std::string> CommandHandler::get_available_commands() const {
    std::vector<std::string> result;
    for (const auto& [cmd, _] : commands_) {
        result.push_back(cmd);
    }
    return result;
}
