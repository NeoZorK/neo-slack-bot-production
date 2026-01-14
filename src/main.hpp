/**
 * @file main.hpp
 * @brief Main application declarations
 */

#ifndef MAIN_HPP
#define MAIN_HPP

#include "config_loader.hpp"
#include "settings.hpp"
#include <string>

/**
 * @brief Initialize application with configuration
 * @param config_loader Config loader instance
 * @param settings Output parameter for loaded settings
 * @return true if initialization successful, false otherwise
 */
bool initialize_application(ConfigLoader& config_loader, Settings& settings);

/**
 * @brief Load bot token from environment variable
 * @return Bot token string, empty if not found
 */
std::string load_bot_token();

/**
 * @brief Load app-level token from environment variable
 * @return App token string, empty if not found
 */
std::string load_app_token();

#endif // MAIN_HPP
