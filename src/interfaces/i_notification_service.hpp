#ifndef I_NOTIFICATION_SERVICE_HPP
#define I_NOTIFICATION_SERVICE_HPP

#include <string>
#include <memory>

// Forward declaration to avoid circular dependency
struct Settings;

/**
 * @brief Platform-independent interface for notification services
 * 
 * This interface provides a unified API for displaying notifications
 * across different platforms (macOS, Linux, Windows).
 * 
 * Thread Safety: Implementation-dependent. Each platform implementation
 * should document its thread safety guarantees.
 */
class INotificationService {
public:
    virtual ~INotificationService() = default;
    
    /**
     * @brief Show a notification with title and body
     * @param title Notification title
     * @param body Notification body text
     * @return true if notification was shown successfully, false otherwise
     */
    virtual bool show_notification(const std::string& title, 
                                   const std::string& body) = 0;
    
    /**
     * @brief Show a notification with title, body, and subtitle
     * @param title Notification title
     * @param body Notification body text
     * @param subtitle Optional subtitle text
     * @return true if notification was shown successfully, false otherwise
     */
    virtual bool show_notification(const std::string& title, 
                                   const std::string& body,
                                   const std::string& subtitle) = 0;
    
    /**
     * @brief Enable or disable notification sound
     * @param enabled true to enable sound, false to disable
     */
    virtual void set_sound_enabled(bool enabled) = 0;
    
    /**
     * @brief Check if sound is enabled
     * @return true if sound is enabled, false otherwise
     */
    virtual bool is_sound_enabled() const = 0;
    
    /**
     * @brief Enable or disable notifications
     * @param enabled true to enable notifications, false to disable
     */
    virtual void set_enabled(bool enabled) = 0;
    
    /**
     * @brief Check if notifications are enabled
     * @return true if notifications are enabled, false otherwise
     */
    virtual bool is_enabled() const = 0;
    
    /**
     * @brief Apply settings from Settings structure
     * @param settings Settings object containing notification preferences
     */
    virtual void apply_settings(const Settings& settings) = 0;
};

#endif // I_NOTIFICATION_SERVICE_HPP
