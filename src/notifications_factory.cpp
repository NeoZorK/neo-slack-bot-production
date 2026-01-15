#include "notifications_factory.hpp"
#include "settings.hpp"

// Platform-specific includes
#ifdef __APPLE__
#include "platform/macos/macos_notifications_impl.hpp"
#elif defined(__linux__)
#include "platform/linux/linux_notifications_impl.hpp"
#elif defined(_WIN32) || defined(_WIN64)
#include "platform/windows/windows_notifications_impl.hpp"
#else
#error "Unsupported platform"
#endif

std::unique_ptr<INotificationService> NotificationsFactory::create() {
#ifdef __APPLE__
    return std::make_unique<MacOSNotificationsImpl>();
#elif defined(__linux__)
    return std::make_unique<LinuxNotificationsImpl>();
#elif defined(_WIN32) || defined(_WIN64)
    return std::make_unique<WindowsNotificationsImpl>();
#else
    #error "Unsupported platform"
#endif
}

std::unique_ptr<INotificationService> NotificationsFactory::create(const Settings& settings) {
#ifdef __APPLE__
    return std::make_unique<MacOSNotificationsImpl>(settings);
#elif defined(__linux__)
    return std::make_unique<LinuxNotificationsImpl>(settings);
#elif defined(_WIN32) || defined(_WIN64)
    return std::make_unique<WindowsNotificationsImpl>(settings);
#else
    #error "Unsupported platform"
#endif
}
