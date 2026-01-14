#ifndef MACOS_NOTIFICATIONS_HPP
#define MACOS_NOTIFICATIONS_HPP

#include <string>
#include <mutex>
#include "settings.hpp"

/**
 * @brief macOS native notifications wrapper (legacy)
 * 
 * This class is maintained for backward compatibility.
 * New code should use INotificationService interface and
 * NotificationsFactory for platform-independent notifications.
 * 
 * Thread Safety: Thread-safe. All methods use internal synchronization.
 */
class MacOSNotifications {
public:
    /**
     * @brief Construct with default settings
     */
    MacOSNotifications();
    
    /**
     * @brief Construct with settings
     * @param settings Application settings
     */
    explicit MacOSNotifications(const Settings& settings);
    
    /**
     * @brief Destructor
     */
    ~MacOSNotifications();
    
    /**
     * @brief Show notification with title and body
     * @param title Notification title
     * @param body Notification body text
     * @return true if notification shown successfully
     */
    bool show_notification(const std::string& title, const std::string& body);
    
    /**
     * @brief Show notification with title, body, and subtitle
     * @param title Notification title
     * @param body Notification body text
     * @param subtitle Optional subtitle text
     * @return true if notification shown successfully
     */
    bool show_notification(const std::string& title, const std::string& body, 
                          const std::string& subtitle);
    
    /**
     * @brief Enable or disable notification sound
     * @param enabled true to enable sound, false to disable
     */
    void set_sound_enabled(bool enabled);
    
    /**
     * @brief Check if sound is enabled
     * @return true if sound is enabled, false otherwise
     */
    bool is_sound_enabled() const;
    
    /**
     * @brief Enable or disable notifications
     * @param enabled true to enable notifications, false to disable
     */
    void set_enabled(bool enabled);
    
    /**
     * @brief Check if notifications are enabled
     * @return true if notifications are enabled, false otherwise
     */
    bool is_enabled() const;
    
    /**
     * @brief Apply settings from Settings structure
     * @param settings Settings object containing notification preferences
     */
    void apply_settings(const Settings& settings);

private:
    mutable std::mutex mutex_;
    bool enabled_;
    bool sound_enabled_;
    Settings::NotificationSettings::DndHours dnd_hours_;
    void* notification_center_; // macOS NSUserNotificationCenter handle
};

#endif // MACOS_NOTIFICATIONS_HPP
