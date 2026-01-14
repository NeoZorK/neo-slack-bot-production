#include "macos_notifications.hpp"
#include <iostream>
#include <CoreFoundation/CoreFoundation.h>
#include <objc/runtime.h>
#include <objc/message.h>

namespace {
    id get_notification_center() {
        Class ns_user_notification_center = objc_getClass("NSUserNotificationCenter");
        if (!ns_user_notification_center) {
            return nullptr;
        }
        
        SEL default_center_sel = sel_registerName("defaultUserNotificationCenter");
        if (!default_center_sel) {
            return nullptr;
        }
        
        id (*default_center)(Class, SEL) = 
            reinterpret_cast<id (*)(Class, SEL)>(objc_msgSend);
        return default_center(ns_user_notification_center, default_center_sel);
    }
    
    id create_notification(const std::string& title, 
                          const std::string& body,
                          const std::string& subtitle) {
        Class ns_user_notification = objc_getClass("NSUserNotification");
        if (!ns_user_notification) {
            return nullptr;
        }
        
        id (*alloc_notification)(Class, SEL) = 
            reinterpret_cast<id (*)(Class, SEL)>(objc_msgSend);
        id notification = alloc_notification(ns_user_notification, 
                                            sel_registerName("alloc"));
        
        if (!notification) {
            return nullptr;
        }
        
        SEL init_sel = sel_registerName("init");
        id (*init_notification)(id, SEL) = 
            reinterpret_cast<id (*)(id, SEL)>(objc_msgSend);
        notification = init_notification(notification, init_sel);
        
        if (!notification) {
            return nullptr;
        }
        
        SEL set_title_sel = sel_registerName("setTitle:");
        SEL set_informative_text_sel = sel_registerName("setInformativeText:");
        SEL set_subtitle_sel = sel_registerName("setSubtitle:");
        
        CFStringRef title_cf = CFStringCreateWithCString(
            kCFAllocatorDefault, title.c_str(), kCFStringEncodingUTF8);
        CFStringRef body_cf = CFStringCreateWithCString(
            kCFAllocatorDefault, body.c_str(), kCFStringEncodingUTF8);
        
        void (*set_title)(id, SEL, CFStringRef) = 
            reinterpret_cast<void (*)(id, SEL, CFStringRef)>(objc_msgSend);
        void (*set_body)(id, SEL, CFStringRef) = 
            reinterpret_cast<void (*)(id, SEL, CFStringRef)>(objc_msgSend);
        
        set_title(notification, set_title_sel, title_cf);
        set_body(notification, set_informative_text_sel, body_cf);
        
        if (!subtitle.empty()) {
            CFStringRef subtitle_cf = CFStringCreateWithCString(
                kCFAllocatorDefault, subtitle.c_str(), kCFStringEncodingUTF8);
            void (*set_subtitle)(id, SEL, CFStringRef) = 
                reinterpret_cast<void (*)(id, SEL, CFStringRef)>(objc_msgSend);
            set_subtitle(notification, set_subtitle_sel, subtitle_cf);
            CFRelease(subtitle_cf);
        }
        
        CFRelease(title_cf);
        CFRelease(body_cf);
        
        return notification;
    }
    
    void deliver_notification(id notification_center, id notification) {
        if (!notification_center || !notification) {
            return;
        }
        
        SEL deliver_sel = sel_registerName("deliverNotification:");
        void (*deliver)(id, SEL, id) = 
            reinterpret_cast<void (*)(id, SEL, id)>(objc_msgSend);
        deliver(notification_center, deliver_sel, notification);
    }
}

MacOSNotifications::MacOSNotifications() 
    : enabled_(true), sound_enabled_(true), notification_center_(nullptr) {
    notification_center_ = get_notification_center();
    dnd_hours_.enabled = false;
}

MacOSNotifications::MacOSNotifications(const Settings& settings)
    : enabled_(true), sound_enabled_(true), notification_center_(nullptr) {
    notification_center_ = get_notification_center();
    apply_settings(settings);
}

MacOSNotifications::~MacOSNotifications() {
    notification_center_ = nullptr;
}

bool MacOSNotifications::show_notification(const std::string& title, 
                                          const std::string& body) {
    return show_notification(title, body, "");
}

bool MacOSNotifications::show_notification(const std::string& title,
                                          const std::string& body,
                                          const std::string& subtitle) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!enabled_) {
        return false;
    }
    
    if (dnd_hours_.enabled) {
        Settings temp_settings;
        temp_settings.notifications.dnd_hours = dnd_hours_;
        if (temp_settings.is_dnd_active()) {
            return false;
        }
    }
    
    if (!notification_center_) {
        std::cerr << "Warning: Notification center not available" << std::endl;
        return false;
    }
    
    id notification = create_notification(title, body, subtitle);
    if (!notification) {
        std::cerr << "Warning: Failed to create notification" << std::endl;
        return false;
    }
    
    deliver_notification(static_cast<id>(notification_center_), notification);
    
    SEL release_sel = sel_registerName("release");
    void (*release)(id, SEL) = 
        reinterpret_cast<void (*)(id, SEL)>(objc_msgSend);
    release(notification, release_sel);
    
    return true;
}

void MacOSNotifications::set_sound_enabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    sound_enabled_ = enabled;
}

bool MacOSNotifications::is_sound_enabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return sound_enabled_;
}

void MacOSNotifications::set_enabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = enabled;
}

bool MacOSNotifications::is_enabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return enabled_;
}

void MacOSNotifications::apply_settings(const Settings& settings) {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = settings.notifications.enabled;
    sound_enabled_ = settings.notifications.sound_enabled;
    dnd_hours_ = settings.notifications.dnd_hours;
}
