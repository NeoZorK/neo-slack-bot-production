#ifndef MACOS_NOTIFICATIONS_IMPL_HPP
#define MACOS_NOTIFICATIONS_IMPL_HPP

#include "../../interfaces/i_notification_service.hpp"
#include "../../macos_notifications.hpp"

/**
 * @brief macOS implementation of INotificationService
 * 
 * This is a wrapper around the existing MacOSNotifications class
 * to provide platform-independent interface.
 * 
 * Thread Safety: Same as MacOSNotifications - not explicitly thread-safe.
 * External synchronization required if used from multiple threads.
 */
class MacOSNotificationsImpl : public INotificationService {
public:
    MacOSNotificationsImpl();
    explicit MacOSNotificationsImpl(const Settings& settings);
    ~MacOSNotificationsImpl() override = default;
    
    bool show_notification(const std::string& title, 
                          const std::string& body) override;
    bool show_notification(const std::string& title, 
                          const std::string& body,
                          const std::string& subtitle) override;
    
    void set_sound_enabled(bool enabled) override;
    bool is_sound_enabled() const override;
    
    void set_enabled(bool enabled) override;
    bool is_enabled() const override;
    
    void apply_settings(const Settings& settings) override;

private:
    MacOSNotifications impl_;
};

#endif // MACOS_NOTIFICATIONS_IMPL_HPP
