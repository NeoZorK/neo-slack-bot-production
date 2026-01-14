#ifndef EVENT_PARSER_HPP
#define EVENT_PARSER_HPP

#include <string>
#include <optional>
#include <map>

/**
 * @brief Parses Slack Socket Mode events
 * 
 * Handles parsing of JSON events from Slack Socket Mode.
 * Provides type-safe access to event data.
 */
class event_parser {
public:
    /**
     * @brief Event types from Slack Socket Mode
     */
    enum class event_type {
        hello,
        events_api,
        interactive,
        slash_commands,
        disconnect,
        unknown
    };

    /**
     * @brief Parse event type from JSON string
     * @param json JSON string from Slack
     * @return Event type or unknown if parsing fails
     */
    static event_type parse_type(const std::string& json);

    /**
     * @brief Extract event envelope ID for acknowledgment
     * @param json JSON string from Slack
     * @return Envelope ID or empty if not found
     */
    static std::optional<std::string> extract_envelope_id(const std::string& json);

    /**
     * @brief Extract event payload
     * @param json JSON string from Slack
     * @return Event payload as JSON string or empty if not found
     */
    static std::optional<std::string> extract_payload(const std::string& json);

    /**
     * @brief Extract message text from events_api event
     * @param json JSON string from Slack
     * @return Message text or empty if not found
     */
    static std::optional<std::string> extract_message_text(const std::string& json);

    /**
     * @brief Extract channel ID from event
     * @param json JSON string from Slack
     * @return Channel ID or empty if not found
     */
    static std::optional<std::string> extract_channel_id(const std::string& json);

    /**
     * @brief Extract user ID from event
     * @param json JSON string from Slack
     * @return User ID or empty if not found
     */
    static std::optional<std::string> extract_user_id(const std::string& json);

    /**
     * @brief Check if event is a message event
     * @param json JSON string from Slack
     * @return true if event type is message
     */
    static bool is_message_event(const std::string& json);

    /**
     * @brief Check if event is an app mention
     * @param json JSON string from Slack
     * @return true if event is app_mention
     */
    static bool is_app_mention(const std::string& json);

    /**
     * @brief Extract command text from slash_commands event
     * @param json JSON string from Slack
     * @return Command text or empty if not found
     */
    static std::optional<std::string> extract_command_text(const std::string& json);

private:
    /**
     * @brief Simple JSON value extraction (without full parser)
     * @param json JSON string
     * @param key Key to extract
     * @return Value or empty string
     */
    static std::string extract_json_value(const std::string& json, const std::string& key);
};

#endif // EVENT_PARSER_HPP
