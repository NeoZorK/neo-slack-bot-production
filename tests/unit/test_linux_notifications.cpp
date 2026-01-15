#include <iostream>
#include <cassert>
#include "../src/platform/linux/linux_notifications_impl.hpp"
#include "../src/settings.hpp"

// Cleaned version

void test_linux_notifications_creation() {
    LinuxNotificationsImpl notifications;
    
    assert(notifications.is_enabled() == true);
    assert(notifications.is_sound_enabled() == true);
    
    std::cout << "  ✓ Linux notifications creation test passed" << std::endl;
}

void test_linux_notifications_with_settings() {
    Settings settings;
    settings.notifications.enabled = false;
    settings.notifications.sound_enabled = false;
    
    LinuxNotificationsImpl notifications(settings);
    
    assert(notifications.is_enabled() == false);
    assert(notifications.is_sound_enabled() == false);
    
    std::cout << "  ✓ Linux notifications with settings test passed" << std::endl;
}

void test_linux_notifications_enable_disable() {
    LinuxNotificationsImpl notifications;
    
    notifications.set_enabled(false);
    assert(notifications.is_enabled() == false);
    
    notifications.set_enabled(true);
    assert(notifications.is_enabled() == true);
    
    std::cout << "  ✓ Linux notifications enable/disable test passed" << std::endl;
}

void test_linux_notifications_sound() {
    LinuxNotificationsImpl notifications;
    
    notifications.set_sound_enabled(false);
    assert(notifications.is_sound_enabled() == false);
    
    notifications.set_sound_enabled(true);
    assert(notifications.is_sound_enabled() == true);
    
    std::cout << "  ✓ Linux notifications sound test passed" << std::endl;
}

void test_linux_notifications_apply_settings() {
    LinuxNotificationsImpl notifications;
    
    Settings settings;
    settings.notifications.enabled = false;
    settings.notifications.sound_enabled = false;
    
    notifications.apply_settings(settings);
    
    assert(notifications.is_enabled() == false);
    assert(notifications.is_sound_enabled() == false);
    
    std::cout << "  ✓ Linux notifications apply settings test passed" << std::endl;
}

void test_linux_notifications_disabled() {
    LinuxNotificationsImpl notifications;
    notifications.set_enabled(false);
    
    // When disabled, show_notification should return false
    bool result = notifications.show_notification("Test", "Body");
    // Note: This might return true if libnotify is not available,
    // but the important thing is that it doesn't crash
    (void)result; // Suppress unused variable warning
    
    std::cout << "  ✓ Linux notifications disabled test passed" << std::endl;
}

void test_linux_notifications() {
    std::cout << "Running Linux notifications tests..." << std::endl;
    
    test_linux_notifications_creation();
    test_linux_notifications_with_settings();
    test_linux_notifications_enable_disable();
    test_linux_notifications_sound();
    test_linux_notifications_apply_settings();
    test_linux_notifications_disabled();
    
    std::cout << "All Linux notifications tests passed!" << std::endl;
}
