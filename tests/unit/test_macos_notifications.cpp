#include <iostream>
#include <cassert>
#include "../src/macos_notifications.hpp"

void test_notification_creation() {
    MacOSNotifications notifications;
    assert(notifications.is_sound_enabled());
}

void test_notification_display() {
    MacOSNotifications notifications;
    bool result = notifications.show_notification("Test", "Test message");
    // Notification may fail in test environment, so we just verify it doesn't crash
    // In real environment with proper permissions, result should be true
    (void)result; // Suppress unused variable warning
}

void test_notification_sound_toggle() {
    MacOSNotifications notifications;
    notifications.set_sound_enabled(false);
    assert(!notifications.is_sound_enabled());
    notifications.set_sound_enabled(true);
    assert(notifications.is_sound_enabled());
}

// Export for test_main.cpp
void test_macos_notifications() {
    test_notification_creation();
    test_notification_display();
    test_notification_sound_toggle();
}
