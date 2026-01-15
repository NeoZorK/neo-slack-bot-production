#ifndef NOTIFICATIONS_FACTORY_HPP
#define NOTIFICATIONS_FACTORY_HPP

#include "interfaces/i_notification_service.hpp"
#include <memory>

// Forward declaration
struct Settings;

/**
 * @brief Factory for creating platform-specific notification service implementations
 * 
 * Automatically detects the current platform and creates the appropriate
 * notification service implementation.
 * 
 * Thread Safety: Factory methods are thread-safe. The returned instances
 * have platform-specific thread safety guarantees.
 */
class NotificationsFactory {
public:
    /**
     * @brief Create notification service with default settings
     * @return Unique pointer to INotificationService implementation
     */
    static std::unique_ptr<INotificationService> create();
    
    /**
     * @brief Create notification service with custom settings
     * @param settings Settings object containing notification preferences
     * @return Unique pointer to INotificationService implementation
     */
    static std::unique_ptr<INotificationService> create(const Settings& settings);
    
private:
    NotificationsFactory() = delete;
    NotificationsFactory(const NotificationsFactory&) = delete;
    NotificationsFactory& operator=(const NotificationsFactory&) = delete;
};

#endif // NOTIFICATIONS_FACTORY_HPP
