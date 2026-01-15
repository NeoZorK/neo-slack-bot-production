#include "linux_notifications_impl.hpp"
#include "../../settings.hpp"
#include "../../logger.hpp"
#include <libnotify/notify.h>
#include <iostream>
#include <mutex>

LinuxNotificationsImpl::LinuxNotificationsImpl() 
    : enabled_(true), sound_enabled_(true), notify_handle_(nullptr) {
    initialize_libnotify();
}

LinuxNotificationsImpl::LinuxNotificationsImpl(const Settings& settings)
    : enabled_(true), sound_enabled_(true), notify_handle_(nullptr) {
    initialize_libnotify();
    apply_settings(settings);
}

LinuxNotificationsImpl::~LinuxNotificationsImpl() {
    cleanup_libnotify();
}

bool LinuxNotificationsImpl::initialize_libnotify() {
    // Initialize libnotify
    if (notify_init("neo-slack-bot")) {
        notify_handle_ = reinterpret_cast<void*>(1); // Mark as initialized
        Logger::get_instance().info("libnotify initialized successfully");
        return true;
    }
    Logger::get_instance().warn("Failed to initialize libnotify");
    return false;
}

void LinuxNotificationsImpl::cleanup_libnotify() {
    if (notify_handle_) {
        notify_uninit();
        notify_handle_ = nullptr;
        Logger::get_instance().info("libnotify cleaned up");
    }
}

bool LinuxNotificationsImpl::show_notification(const std::string& title, 
                                              const std::string& body) {
    return show_notification(title, body, "");
}

bool LinuxNotificationsImpl::show_notification(const std::string& title, 
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
    
    if (!notify_handle_) {
        Logger::get_instance().warn("libnotify not initialized, cannot show notification");
        return false;
    }
    
    // Combine subtitle with body if subtitle is provided
    std::string full_body = body;
    if (!subtitle.empty()) {
        full_body = subtitle + "\n" + body;
    }
    
    // Create notification
    NotifyNotification* notification = notify_notification_new(
        title.c_str(), full_body.c_str(), nullptr);
    
    if (!notification) {
        Logger::get_instance().error("Failed to create notification");
        return false;
    }
    
    // Set timeout (5 seconds default)
    notify_notification_set_timeout(notification, 5000);
    
    // Set urgency level (normal)
    notify_notification_set_urgency(notification, NOTIFY_URGENCY_NORMAL);
    
    // Show notification
    GError* error = nullptr;
    bool result = notify_notification_show(notification, &error);
    
    if (error) {
        Logger::get_instance().error("Failed to show notification: " + std::string(error->message));
        g_error_free(error);
        g_object_unref(G_OBJECT(notification));
        return false;
    }
    
    // Cleanup
    g_object_unref(G_OBJECT(notification));
    
    if (result) {
        Logger::get_instance().debug("Notification shown: " + title);
    }
    
    return result;
}

void LinuxNotificationsImpl::set_sound_enabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    sound_enabled_ = enabled;
}

bool LinuxNotificationsImpl::is_sound_enabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return sound_enabled_;
}

void LinuxNotificationsImpl::set_enabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = enabled;
}

bool LinuxNotificationsImpl::is_enabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return enabled_;
}

void LinuxNotificationsImpl::apply_settings(const Settings& settings) {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = settings.notifications.enabled;
    sound_enabled_ = settings.notifications.sound_enabled;
}
