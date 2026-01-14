#ifndef MESSAGE_FILTER_HPP
#define MESSAGE_FILTER_HPP

#include <string>
#include <vector>
#include <set>
#include <functional>
#include <mutex>
#include "settings.hpp"

/**
 * @brief Message filtering system
 * 
 * Thread Safety: Thread-safe. All methods use internal synchronization.
 */
class MessageFilter {
public:
    /**
     * @brief Construct message filter with default settings
     */
    MessageFilter();
    
    /**
     * @brief Construct message filter with settings
     * @param settings Settings object containing filter configuration
     */
    explicit MessageFilter(const Settings& settings);
    
    /**
     * @brief Destructor
     */
    ~MessageFilter();
    
    /**
     * @brief Add channel to whitelist
     * @param channel_id Channel ID to whitelist
     */
    void add_channel_whitelist(const std::string& channel_id);
    
    /**
     * @brief Add channel to blacklist
     * @param channel_id Channel ID to blacklist
     */
    void add_channel_blacklist(const std::string& channel_id);
    
    /**
     * @brief Add keyword to filter
     * @param keyword Keyword to filter
     */
    void add_keyword_filter(const std::string& keyword);
    
    /**
     * @brief Add user to whitelist
     * @param user_id User ID to whitelist
     */
    void add_user_whitelist(const std::string& user_id);
    
    /**
     * @brief Add user to blacklist
     * @param user_id User ID to blacklist
     */
    void add_user_blacklist(const std::string& user_id);
    
    /**
     * @brief Clear all filters
     */
    void clear_filters();
    
    /**
     * @brief Apply settings to filter
     * @param settings Settings object containing filter configuration
     */
    void apply_settings(const Settings& settings);
    
    /**
     * @brief Check if message should be processed
     * @param channel_id Channel ID where message was sent
     * @param message_text Message text content
     * @return true if message should be processed, false otherwise
     */
    bool should_process(const std::string& channel_id, 
                       const std::string& message_text) const;
    
    /**
     * @brief Check if message should be processed with user filtering
     * @param channel_id Channel ID where message was sent
     * @param message_text Message text content
     * @param user_id User ID who sent the message
     * @return true if message should be processed, false otherwise
     */
    bool should_process(const std::string& channel_id,
                       const std::string& message_text,
                       const std::string& user_id) const;

private:
    mutable std::mutex mutex_;
    std::set<std::string> channel_whitelist_;
    std::set<std::string> channel_blacklist_;
    std::vector<std::string> keywords_;
    std::set<std::string> user_whitelist_;
    std::set<std::string> user_blacklist_;
    
    Settings::EventSettings::KeywordFilter::Mode keyword_mode_;
    bool keyword_filter_enabled_;
    
    bool matches_keyword(const std::string& text) const;
    bool matches_user_filter(const std::string& user_id) const;
};

#endif // MESSAGE_FILTER_HPP
