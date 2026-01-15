#ifndef LINUX_NOTIFICATIONS_IMPL_HPP
#define LINUX_NOTIFICATIONS_IMPL_HPP

#include "../../interfaces/i_notification_service.hpp"
#include <string>
#include <mutex>

/**
 * @brief Linux implementation of INotificationService using libnotify
 * 
 * Uses libnotify library for displaying desktop notifications on Linux.
 * 
 * Thread Safety: libnotify is generally thread-safe, but this implementation
 * uses a mutex for additional safety when accessing internal state.
 */
class LinuxNotificationsImpl : public INotificationService {
public:
    LinuxNotificationsImpl();
    explicit LinuxNotificationsImpl(const Settings& settings);
    ~LinuxNotificationsImpl() override;
    
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
    void* notify_handle_; // libnotify handle
    mutable std::mutex mutex_;
    
    bool initialize_libnotify();
    void cleanup_libnotify();
};

#endif // LINUX_NOTIFICATIONS_IMPL_HPP
