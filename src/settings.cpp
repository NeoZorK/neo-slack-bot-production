#include "settings.hpp"
#include <ctime>
#include <sstream>
#include <iomanip>

bool Settings::is_dnd_active() const {
    if (!notifications.dnd_hours.enabled) {
        return false;
    }
    
    auto now = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_now = std::localtime(&time_now);
    
    int current_minutes = tm_now->tm_hour * 60 + tm_now->tm_min;
    
    std::istringstream start_stream(notifications.dnd_hours.start);
    std::istringstream end_stream(notifications.dnd_hours.end);
    int start_hour, start_min, end_hour, end_min;
    char colon;
    
    start_stream >> start_hour >> colon >> start_min;
    end_stream >> end_hour >> colon >> end_min;
    
    int start_minutes = start_hour * 60 + start_min;
    int end_minutes = end_hour * 60 + end_min;
    
    if (start_minutes <= end_minutes) {
        return current_minutes >= start_minutes && current_minutes < end_minutes;
    } else {
        return current_minutes >= start_minutes || current_minutes < end_minutes;
    }
}

Settings Settings::get_defaults() {
    Settings defaults;
    defaults.notifications.enabled = true;
    defaults.notifications.sound_enabled = true;
    defaults.notifications.dnd_hours.enabled = false;
    defaults.notifications.dnd_hours.start = "22:00";
    defaults.notifications.dnd_hours.end = "08:00";
    defaults.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::ALL;
    
    defaults.connection.max_retries = 10;
    defaults.connection.timeout_seconds = 30;
    defaults.connection.ping_interval_seconds = 30;
    defaults.connection.retry_delay_seconds = 1;
    
    defaults.events.keyword_filter.enabled = false;
    defaults.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::INCLUDE;
    defaults.events.user_filter.mode = Settings::EventSettings::UserFilter::ALL;
    
    return defaults;
}
