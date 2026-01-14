#include "macos_keychain_impl.hpp"
#include <Security/Security.h>
#include <CoreFoundation/CoreFoundation.h>
#include <vector>
#include <iostream>

namespace {
    // Helper to convert std::string to CFStringRef
    CFStringRef create_cf_string(const std::string& str) {
        return CFStringCreateWithCString(kCFAllocatorDefault, str.c_str(), kCFStringEncodingUTF8);
    }
}

bool MacOSKeychainImpl::set_password(const std::string& service, const std::string& account, const std::string& password) {
    CFStringRef service_cf = create_cf_string(service);
    CFStringRef account_cf = create_cf_string(account);
    CFDataRef password_data = CFDataCreate(kCFAllocatorDefault, 
                                          reinterpret_cast<const UInt8*>(password.c_str()), 
                                          password.length());

    // Dictionary for the item
    const void* keys[] = {
        kSecClass,
        kSecAttrService,
        kSecAttrAccount,
        kSecValueData
    };
    const void* values[] = {
        kSecClassGenericPassword,
        service_cf,
        account_cf,
        password_data
    };
    
    CFDictionaryRef query = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 4, 
                                              &kCFTypeDictionaryKeyCallBacks, 
                                              &kCFTypeDictionaryValueCallBacks);

    OSStatus status = SecItemAdd(query, nullptr);
    
    // If it already exists, update it
    if (status == errSecDuplicateItem) {
        // Create query for existing item
        const void* query_keys[] = {
            kSecClass,
            kSecAttrService,
            kSecAttrAccount
        };
        const void* query_values[] = {
            kSecClassGenericPassword,
            service_cf,
            account_cf
        };
        CFDictionaryRef update_query = CFDictionaryCreate(kCFAllocatorDefault, query_keys, query_values, 3,
                                                         &kCFTypeDictionaryKeyCallBacks,
                                                         &kCFTypeDictionaryValueCallBacks);
        
        // Attributes to update
        const void* update_keys[] = { kSecValueData };
        const void* update_values[] = { password_data };
        CFDictionaryRef update_attributes = CFDictionaryCreate(kCFAllocatorDefault, update_keys, update_values, 1,
                                                              &kCFTypeDictionaryKeyCallBacks,
                                                              &kCFTypeDictionaryValueCallBacks);
        
        status = SecItemUpdate(update_query, update_attributes);
        
        CFRelease(update_query);
        CFRelease(update_attributes);
    }

    CFRelease(service_cf);
    CFRelease(account_cf);
    CFRelease(password_data);
    CFRelease(query);

    return (status == errSecSuccess);
}

std::optional<std::string> MacOSKeychainImpl::get_password(const std::string& service, const std::string& account) {
    CFStringRef service_cf = create_cf_string(service);
    CFStringRef account_cf = create_cf_string(account);

    const void* keys[] = {
        kSecClass,
        kSecAttrService,
        kSecAttrAccount,
        kSecReturnData,
        kSecMatchLimit
    };
    const void* values[] = {
        kSecClassGenericPassword,
        service_cf,
        account_cf,
        kCFBooleanTrue,
        kSecMatchLimitOne
    };
    
    CFDictionaryRef query = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 5,
                                              &kCFTypeDictionaryKeyCallBacks,
                                              &kCFTypeDictionaryValueCallBacks);

    CFTypeRef result = nullptr;
    OSStatus status = SecItemCopyMatching(query, &result);
    
    CFRelease(service_cf);
    CFRelease(account_cf);
    CFRelease(query);

    if (status == errSecSuccess && result != nullptr) {
        CFDataRef data = (CFDataRef)result;
        CFIndex length = CFDataGetLength(data);
        const UInt8* bytes = CFDataGetBytePtr(data);
        
        std::string password(reinterpret_cast<const char*>(bytes), length);
        CFRelease(result);
        return password;
    }

    return std::nullopt;
}

bool MacOSKeychainImpl::delete_password(const std::string& service, const std::string& account) {
    CFStringRef service_cf = create_cf_string(service);
    CFStringRef account_cf = create_cf_string(account);

    const void* keys[] = {
        kSecClass,
        kSecAttrService,
        kSecAttrAccount
    };
    const void* values[] = {
        kSecClassGenericPassword,
        service_cf,
        account_cf
    };
    
    CFDictionaryRef query = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 3,
                                              &kCFTypeDictionaryKeyCallBacks,
                                              &kCFTypeDictionaryValueCallBacks);

    OSStatus status = SecItemDelete(query);
    
    CFRelease(service_cf);
    CFRelease(account_cf);
    CFRelease(query);

    return (status == errSecSuccess || status == errSecItemNotFound);
}
