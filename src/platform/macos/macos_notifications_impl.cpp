#include "macos_notifications_impl.hpp"
#include "../../settings.hpp"

MacOSNotificationsImpl::MacOSNotificationsImpl() : impl_() {
}

MacOSNotificationsImpl::MacOSNotificationsImpl(const Settings& settings) 
    : impl_(settings) {
}

bool MacOSNotificationsImpl::show_notification(const std::string& title, 
                                                const std::string& body) {
    return impl_.show_notification(title, body);
}

bool MacOSNotificationsImpl::show_notification(const std::string& title, 
                                              const std::string& body,
                                              const std::string& subtitle) {
    return impl_.show_notification(title, body, subtitle);
}

void MacOSNotificationsImpl::set_sound_enabled(bool enabled) {
    impl_.set_sound_enabled(enabled);
}

bool MacOSNotificationsImpl::is_sound_enabled() const {
    return impl_.is_sound_enabled();
}

void MacOSNotificationsImpl::set_enabled(bool enabled) {
    impl_.set_enabled(enabled);
}

bool MacOSNotificationsImpl::is_enabled() const {
    return impl_.is_enabled();
}

void MacOSNotificationsImpl::apply_settings(const Settings& settings) {
    impl_.apply_settings(settings);
}
