#include "message_filter.hpp"
#include <algorithm>
#include <cctype>

MessageFilter::MessageFilter() 
    : keyword_mode_(Settings::EventSettings::KeywordFilter::INCLUDE),
      keyword_filter_enabled_(false) {
}

MessageFilter::MessageFilter(const Settings& settings)
    : keyword_mode_(Settings::EventSettings::KeywordFilter::INCLUDE),
      keyword_filter_enabled_(false) {
    apply_settings(settings);
}

MessageFilter::~MessageFilter() {
}

void MessageFilter::add_channel_whitelist(const std::string& channel_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    channel_whitelist_.insert(channel_id);
}

void MessageFilter::add_channel_blacklist(const std::string& channel_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    channel_blacklist_.insert(channel_id);
}

void MessageFilter::add_keyword_filter(const std::string& keyword) {
    std::lock_guard<std::mutex> lock(mutex_);
    keywords_.push_back(keyword);
}

void MessageFilter::add_user_whitelist(const std::string& user_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    user_whitelist_.insert(user_id);
}

void MessageFilter::add_user_blacklist(const std::string& user_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    user_blacklist_.insert(user_id);
}

void MessageFilter::clear_filters() {
    std::lock_guard<std::mutex> lock(mutex_);
    channel_whitelist_.clear();
    channel_blacklist_.clear();
    keywords_.clear();
    user_whitelist_.clear();
    user_blacklist_.clear();
    keyword_filter_enabled_ = false;
}

void MessageFilter::apply_settings(const Settings& settings) {
    std::lock_guard<std::mutex> lock(mutex_);
    channel_whitelist_.clear();
    channel_blacklist_.clear();
    keywords_.clear();
    user_whitelist_.clear();
    user_blacklist_.clear();
    
    if (settings.notifications.channel_filter.mode == Settings::NotificationSettings::ChannelFilter::WHITELIST) {
        channel_whitelist_ = settings.notifications.channel_filter.channels;
    } else if (settings.notifications.channel_filter.mode == Settings::NotificationSettings::ChannelFilter::BLACKLIST) {
        channel_blacklist_ = settings.notifications.channel_filter.channels;
    }
    
    if (settings.events.keyword_filter.enabled) {
        keyword_filter_enabled_ = true;
        keyword_mode_ = settings.events.keyword_filter.mode;
        keywords_ = settings.events.keyword_filter.keywords;
    }
    
    if (settings.events.user_filter.mode == Settings::EventSettings::UserFilter::WHITELIST) {
        user_whitelist_ = settings.events.user_filter.users;
    } else if (settings.events.user_filter.mode == Settings::EventSettings::UserFilter::BLACKLIST) {
        user_blacklist_ = settings.events.user_filter.users;
    }
}

bool MessageFilter::matches_keyword(const std::string& text) const {
    // Private method, assumes lock is held
    if (!keyword_filter_enabled_ || keywords_.empty()) {
        return true;
    }
    
    std::string lower_text = text;
    std::transform(lower_text.begin(), lower_text.end(), 
                   lower_text.begin(), ::tolower);
    
    bool found = false;
    for (const auto& keyword : keywords_) {
        std::string lower_keyword = keyword;
        std::transform(lower_keyword.begin(), lower_keyword.end(),
                      lower_keyword.begin(), ::tolower);
        if (lower_text.find(lower_keyword) != std::string::npos) {
            found = true;
            break;
        }
    }
    
    if (keyword_mode_ == Settings::EventSettings::KeywordFilter::INCLUDE) {
        return found;
    } else {
        return !found;
    }
}

bool MessageFilter::matches_user_filter(const std::string& user_id) const {
    // Private method, assumes lock is held
    if (!user_blacklist_.empty() && 
        user_blacklist_.find(user_id) != user_blacklist_.end()) {
        return false;
    }
    
    if (!user_whitelist_.empty() && 
        user_whitelist_.find(user_id) == user_whitelist_.end()) {
        return false;
    }
    
    return true;
}

bool MessageFilter::should_process(const std::string& channel_id,
                                  const std::string& message_text) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!channel_blacklist_.empty() && 
        channel_blacklist_.find(channel_id) != channel_blacklist_.end()) {
        return false;
    }
    
    if (!channel_whitelist_.empty() && 
        channel_whitelist_.find(channel_id) == channel_whitelist_.end()) {
        return false;
    }
    
    return matches_keyword(message_text);
}

bool MessageFilter::should_process(const std::string& channel_id,
                                  const std::string& message_text,
                                  const std::string& user_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!matches_user_filter(user_id)) {
        return false;
    }
    
    // Duplicate logic to avoid recursive lock
    if (!channel_blacklist_.empty() && 
        channel_blacklist_.find(channel_id) != channel_blacklist_.end()) {
        return false;
    }
    
    if (!channel_whitelist_.empty() && 
        channel_whitelist_.find(channel_id) == channel_whitelist_.end()) {
        return false;
    }
    
    return matches_keyword(message_text);
}
