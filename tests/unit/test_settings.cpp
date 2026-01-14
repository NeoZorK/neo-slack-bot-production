#include <iostream>
#include <cassert>
#include "../src/settings.hpp"

void test_settings_defaults() {
    Settings settings = Settings::get_defaults();
    
    assert(settings.notifications.enabled == true);
    assert(settings.notifications.sound_enabled == true);
    assert(settings.notifications.dnd_hours.enabled == false);
    assert(settings.notifications.dnd_hours.start == "22:00");
    assert(settings.notifications.dnd_hours.end == "08:00");
    assert(settings.notifications.channel_filter.mode == Settings::NotificationSettings::ChannelFilter::ALL);
    
    assert(settings.connection.max_retries == 10);
    assert(settings.connection.timeout_seconds == 30);
    assert(settings.connection.ping_interval_seconds == 30);
    assert(settings.connection.retry_delay_seconds == 1);
    
    assert(settings.events.keyword_filter.enabled == false);
    assert(settings.events.keyword_filter.mode == Settings::EventSettings::KeywordFilter::INCLUDE);
    assert(settings.events.user_filter.mode == Settings::EventSettings::UserFilter::ALL);
}

void test_settings_dnd_check() {
    Settings settings = Settings::get_defaults();
    
    settings.notifications.dnd_hours.enabled = false;
    assert(!settings.is_dnd_active());
    
    settings.notifications.dnd_hours.enabled = true;
    settings.notifications.dnd_hours.start = "00:00";
    settings.notifications.dnd_hours.end = "23:59";
    assert(settings.is_dnd_active());
    
    settings.notifications.dnd_hours.start = "23:59";
    settings.notifications.dnd_hours.end = "00:01";
    (void)settings.is_dnd_active();
}

void test_settings_channel_filter_modes() {
    Settings settings = Settings::get_defaults();
    
    settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::WHITELIST;
    assert(settings.notifications.channel_filter.mode == Settings::NotificationSettings::ChannelFilter::WHITELIST);
    
    settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::BLACKLIST;
    assert(settings.notifications.channel_filter.mode == Settings::NotificationSettings::ChannelFilter::BLACKLIST);
    
    settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::ALL;
    assert(settings.notifications.channel_filter.mode == Settings::NotificationSettings::ChannelFilter::ALL);
}

void test_settings_user_filter_modes() {
    Settings settings = Settings::get_defaults();
    
    settings.events.user_filter.mode = Settings::EventSettings::UserFilter::WHITELIST;
    assert(settings.events.user_filter.mode == Settings::EventSettings::UserFilter::WHITELIST);
    
    settings.events.user_filter.mode = Settings::EventSettings::UserFilter::BLACKLIST;
    assert(settings.events.user_filter.mode == Settings::EventSettings::UserFilter::BLACKLIST);
    
    settings.events.user_filter.mode = Settings::EventSettings::UserFilter::ALL;
    assert(settings.events.user_filter.mode == Settings::EventSettings::UserFilter::ALL);
}

void test_settings_keyword_filter_modes() {
    Settings settings = Settings::get_defaults();
    
    settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::INCLUDE;
    assert(settings.events.keyword_filter.mode == Settings::EventSettings::KeywordFilter::INCLUDE);
    
    settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::EXCLUDE;
    assert(settings.events.keyword_filter.mode == Settings::EventSettings::KeywordFilter::EXCLUDE);
}

void test_settings() {
    test_settings_defaults();
    test_settings_dnd_check();
    test_settings_channel_filter_modes();
    test_settings_user_filter_modes();
    test_settings_keyword_filter_modes();
    std::cout << "All settings tests passed!" << std::endl;
}
