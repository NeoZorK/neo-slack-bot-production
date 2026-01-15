#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

#include <string>
#include <functional>
#include <map>
#include <vector>

/**
 * @brief Command handling system for slash commands
 * 
 * Thread Safety: NOT thread-safe for modifications (register_command).
 * handle_command() can be called concurrently after initialization, but
 * registration requires external synchronization.
 */
class CommandHandler {
public:
    /**
     * @brief Command callback function type
     * 
     * @param channel_id Channel where command was invoked
     * @param args Command arguments
     */
    using CommandCallback = std::function<void(const std::string&, 
                                               const std::vector<std::string>&)>;
    
    /**
     * @brief Construct command handler
     */
    CommandHandler();
    
    /**
     * @brief Destructor
     */
    ~CommandHandler();
    
    /**
     * @brief Register command handler
     * @param command Command name (e.g., "status", "help")
     * @param callback Function to call when command is invoked
     */
    void register_command(const std::string& command, CommandCallback callback);
    
    /**
     * @brief Handle command invocation
     * @param command_text Full command text (e.g., "/status")
     * @param channel_id Channel where command was invoked
     * @param user_id User who invoked command
     * @return true if command was handled, false otherwise
     */
    bool handle_command(const std::string& command_text, 
                       const std::string& channel_id,
                       const std::string& user_id);
    
    /**
     * @brief Get list of available commands
     * @return Vector of command names
     */
    std::vector<std::string> get_available_commands() const;

private:
    std::map<std::string, CommandCallback> commands_;
    std::vector<std::string> parse_command_args(const std::string& text);
};

#endif // COMMAND_HANDLER_HPP
