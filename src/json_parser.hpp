#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include <string>
#include <vector>

/**
 * @brief Simple JSON parser utilities
 * 
 * Provides basic JSON parsing functions without external dependencies.
 * Used for configuration file parsing.
 */
namespace json_parser {
    /**
     * @brief Extract JSON string value
     * @param json JSON string
     * @param key Key to extract
     * @return Value or empty string
     */
    std::string extract_string(const std::string& json, const std::string& key);
    
    /**
     * @brief Extract JSON boolean value
     * @param json JSON string
     * @param key Key to extract
     * @param default_val Default value if not found
     * @return Boolean value or default
     */
    bool extract_bool(const std::string& json, const std::string& key, bool default_val);
    
    /**
     * @brief Extract JSON integer value
     * @param json JSON string
     * @param key Key to extract
     * @param default_val Default value if not found
     * @return Integer value or default
     */
    int extract_int(const std::string& json, const std::string& key, int default_val);
    
    /**
     * @brief Extract JSON array of strings
     * @param json JSON string
     * @param key Key to extract
     * @return Vector of strings
     */
    std::vector<std::string> extract_string_array(const std::string& json, const std::string& key);
}

#endif // JSON_PARSER_HPP
