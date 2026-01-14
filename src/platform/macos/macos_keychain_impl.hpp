#ifndef MACOS_KEYCHAIN_IMPL_HPP
#define MACOS_KEYCHAIN_IMPL_HPP

#include <string>
#include <optional>

/**
 * @class MacOSKeychainImpl
 * @brief Wrapper around macOS Security framework for Keychain access.
 * 
 * Provides static methods to securely store and retrieve secrets
 * using the macOS Keychain Services API.
 */
class MacOSKeychainImpl {
public:
    /**
     * @brief Set a password in the Keychain.
     * 
     * If the item already exists, it updates it.
     * 
     * @param service Service name (e.g., "neo-slack-bot").
     * @param account Account name (e.g., "SLACK_BOT_TOKEN").
     * @param password The secret to store.
     * @return true if successful, false otherwise.
     */
    static bool set_password(const std::string& service, const std::string& account, const std::string& password);

    /**
     * @brief Get a password from the Keychain.
     * 
     * @param service Service name.
     * @param account Account name.
     * @return std::optional containing the password if found, or std::nullopt.
     */
    static std::optional<std::string> get_password(const std::string& service, const std::string& account);

    /**
     * @brief Delete a password from the Keychain.
     * 
     * @param service Service name.
     * @param account Account name.
     * @return true if successful or not found, false on error.
     */
    static bool delete_password(const std::string& service, const std::string& account);
};

#endif // MACOS_KEYCHAIN_IMPL_HPP
