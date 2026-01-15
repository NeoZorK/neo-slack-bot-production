# API Documentation

## Generating API Documentation

Neo Slack Bot uses Doxygen to generate API documentation from source code comments.

### Prerequisites

Install Doxygen:
```bash
brew install doxygen
```

### Generate Documentation

```bash
doxygen Doxyfile
```

Documentation will be generated in `docs/api/html/` directory.

### View Documentation

Open `docs/api/html/index.html` in your browser.

## Documentation Style

### Class Documentation

```cpp
/**
 * @class WebSocketClient
 * @brief WebSocket client for Slack Socket Mode connection
 * 
 * This class manages WebSocket connection to Slack's Socket Mode,
 * handles message sending/receiving, and manages connection state.
 */
class WebSocketClient {
    // ...
};
```

### Function Documentation

```cpp
/**
 * @brief Connect to Slack Socket Mode
 * 
 * Establishes WebSocket connection to Slack's Socket Mode endpoint.
 * Authenticates using bot token and waits for hello event.
 * 
 * @return true if connection successful, false otherwise
 * 
 * @note Connection is asynchronous, check is_connected() after calling
 * 
 * @see disconnect()
 * @see is_connected()
 */
bool connect();
```

### Parameter Documentation

```cpp
/**
 * @brief Send message to Slack channel
 * 
 * @param channel Channel ID (e.g., "C1234567890")
 * @param text Message text to send
 * @return true if message sent successfully
 * 
 * @throws std::runtime_error if API request fails
 */
bool send_message(const std::string& channel, const std::string& text);
```

## Documentation Tags

- `@brief` - Brief description
- `@param` - Parameter description
- `@return` - Return value description
- `@throws` - Exception documentation
- `@note` - Additional notes
- `@see` - See also references
- `@warning` - Warnings
- `@deprecated` - Deprecated functions

## Best Practices

1. Document all public APIs
2. Include parameter descriptions
3. Document return values
4. Note exceptions and errors
5. Provide usage examples
6. Keep documentation up-to-date
