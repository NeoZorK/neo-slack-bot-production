#ifndef WINDOWS_NOTIFICATIONS_IMPL_HPP
#define WINDOWS_NOTIFICATIONS_IMPL_HPP

#include "../../interfaces/i_notification_service.hpp"
#include <string>
#include <mutex>

/**
 * @brief Windows implementation of INotificationService (stub)
 * 
 * This is a placeholder implementation for Windows support.
 * Currently, it only logs to console as Windows notification API
 * integration is planned for future versions.
 * 
 * Thread Safety: Thread-safe. Uses mutex for synchronization.
 */
class WindowsNotificationsImpl : public INotificationService {
public:
    WindowsNotificationsImpl();
    explicit WindowsNotificationsImpl(const Settings& settings);
    ~WindowsNotificationsImpl() override = default;
    
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
    bool enabled_;
    bool sound_enabled_;
    mutable std::mutex mutex_;
};

#endif // WINDOWS_NOTIFICATIONS_IMPL_HPP
