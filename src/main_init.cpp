/**
 * @file main_init.cpp
 * @brief Main initialization functions
 * 
 * Handles application initialization, token loading, and configuration setup.
 */

#include "main.hpp"
#include "config_loader.hpp"
#include "input_validator.hpp"
#include "logger.hpp"
#include "settings.hpp"
#include <cstdlib>
#include <iostream>

bool initialize_application(ConfigLoader& config_loader, Settings& settings) {
    settings = config_loader.load_settings();
    
    const char* log_level_env = std::getenv("LOG_LEVEL");
    if (log_level_env) {
        std::string level_str(log_level_env);
        if (level_str == "DEBUG") {
            Logger::get_instance().set_level(LogLevel::DEBUG);
        } else if (level_str == "WARN") {
            Logger::get_instance().set_level(LogLevel::WARN);
        } else if (level_str == "ERROR") {
            Logger::get_instance().set_level(LogLevel::ERR);
        } else {
            Logger::get_instance().set_level(LogLevel::INFO);
        }
    } else {
        Logger::get_instance().set_level(LogLevel::INFO);
    }

    const char* log_format_env = std::getenv("LOG_FORMAT");
    if (log_format_env) {
        std::string format_str(log_format_env);
        if (format_str == "JSON") {
            Logger::get_instance().set_format(LogFormat::JSON);
        } else {
            Logger::get_instance().set_format(LogFormat::PLAIN);
        }
    } else {
        Logger::get_instance().set_format(LogFormat::PLAIN);
    }

    if (!settings.bot_token.empty() &&
        !InputValidator::is_valid_bot_token(settings.bot_token)) {
        Logger::get_instance().error("Invalid SLACK_BOT_TOKEN format");
        settings.bot_token.clear();
    }

    if (!settings.app_token.empty() &&
        !InputValidator::is_valid_app_token(settings.app_token)) {
        Logger::get_instance().error("Invalid SLACK_APP_TOKEN format");
        settings.app_token.clear();
    }
    
    return true;
}
