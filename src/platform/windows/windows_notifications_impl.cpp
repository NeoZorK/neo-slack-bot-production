#include "windows_notifications_impl.hpp"
#include "../../settings.hpp"
#include <iostream>
#include <mutex>

WindowsNotificationsImpl::WindowsNotificationsImpl() 
    : enabled_(true), sound_enabled_(true) {
}

WindowsNotificationsImpl::WindowsNotificationsImpl(const Settings& settings)
    : enabled_(true), sound_enabled_(true) {
    apply_settings(settings);
}

bool WindowsNotificationsImpl::show_notification(const std::string& title, 
                                                const std::string& body) {
    return show_notification(title, body, "");
}

bool WindowsNotificationsImpl::show_notification(const std::string& title, 
                                                const std::string& body,
                                                const std::string& subtitle) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!enabled_) {
        return false;
    }
    
    // Check DND hours if needed
    Settings temp_settings;
    temp_settings.notifications.enabled = enabled_;
    if (temp_settings.is_dnd_active()) {
        return false;
    }
    
    // Stub implementation: log to console
    // TODO: Implement Windows 10+ Toast Notifications API
    // Future implementation will use:
    // - Windows.UI.Notifications namespace (WinRT)
    // - ToastNotificationManager for creating notifications
    // - App ID registration in Windows registry
    // See docs/windows-support.md for details
    std::cout << "[Windows Notification] " << title;
    if (!subtitle.empty()) {
        std::cout << " - " << subtitle;
    }
    std::cout << ": " << body << std::endl;
    
    return true;
}

void WindowsNotificationsImpl::set_sound_enabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    sound_enabled_ = enabled;
}

bool WindowsNotificationsImpl::is_sound_enabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return sound_enabled_;
}

void WindowsNotificationsImpl::set_enabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = enabled;
}

bool WindowsNotificationsImpl::is_enabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return enabled_;
}

void WindowsNotificationsImpl::apply_settings(const Settings& settings) {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = settings.notifications.enabled;
    sound_enabled_ = settings.notifications.sound_enabled;
}
