#ifndef INPUT_VALIDATOR_HPP
#define INPUT_VALIDATOR_HPP

#include <string>
#include <optional>

/**
 * @brief Input validation and sanitization utilities
 * 
 * Provides validation and sanitization functions for Slack API inputs
 * to prevent security issues and ensure data integrity.
 * 
 * Thread Safety: Thread-safe. All methods are static and stateless.
 */
class InputValidator {
public:
    /**
     * @brief Validate bot token format
     * @param token Token to validate
     * @return true if token format is valid, false otherwise
     */
    static bool is_valid_bot_token(const std::string& token);
    
    /**
     * @brief Validate app-level token format
     * @param token Token to validate
     * @return true if token format is valid, false otherwise
     */
    static bool is_valid_app_token(const std::string& token);
    
    /**
     * @brief Validate channel ID format
     * @param channel_id Channel ID to validate
     * @return true if channel ID format is valid, false otherwise
     */
    static bool is_valid_channel_id(const std::string& channel_id);
    
    /**
     * @brief Validate user ID format
     * @param user_id User ID to validate
     * @return true if user ID format is valid, false otherwise
     */
    static bool is_valid_user_id(const std::string& user_id);
    
    /**
     * @brief Validate message text
     * @param text Message text to validate
     * @return true if message text is valid, false otherwise
     */
    static bool is_valid_message_text(const std::string& text);
    
    /**
     * @brief Sanitize channel ID
     * @param channel_id Channel ID to sanitize
     * @return Sanitized channel ID or empty if invalid
     */
    static std::optional<std::string> sanitize_channel_id(const std::string& channel_id);
    
    /**
     * @brief Sanitize user ID
     * @param user_id User ID to sanitize
     * @return Sanitized user ID or empty if invalid
     */
    static std::optional<std::string> sanitize_user_id(const std::string& user_id);
    
    /**
     * @brief Sanitize message text
     * @param text Message text to sanitize
     * @return Sanitized message text or empty if invalid
     */
    static std::optional<std::string> sanitize_message_text(const std::string& text);

private:
    static constexpr size_t MAX_MESSAGE_LENGTH = 4000;
    static constexpr size_t MIN_TOKEN_LENGTH = 20;
    static constexpr size_t MAX_TOKEN_LENGTH = 200;
    static constexpr size_t MIN_CHANNEL_ID_LENGTH = 9;
    static constexpr size_t MAX_CHANNEL_ID_LENGTH = 15; // Slack channel IDs can vary in length
    static constexpr size_t MIN_USER_ID_LENGTH = 9;
    static constexpr size_t MAX_USER_ID_LENGTH = 15; // Slack user IDs can vary in length
    
    static bool is_alphanumeric_or_special(const std::string& str);
    static bool starts_with_prefix(const std::string& str, const std::string& prefix);
};

#endif // INPUT_VALIDATOR_HPP
