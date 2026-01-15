# Slack API - HTTP Methods

## Overview

Neo Slack Bot uses Slack's HTTP API for sending messages and querying data. All API calls go through the `SlackAPI` wrapper class which provides:

- Rate limiting
- Error recovery
- Retry logic
- Input validation

## Base URL

```
https://slack.com/api/
```

## Implemented Methods

### 1. `chat.postMessage`

Send a message to a Slack channel.

**Endpoint**: `POST https://slack.com/api/chat.postMessage`

**Request**:
```json
{
  "channel": "C1234567890",
  "text": "Hello, world!"
}
```

**Response**:
```json
{
  "ok": true,
  "channel": "C1234567890",
  "ts": "1234567890.123456",
  "message": {...}
}
```

**Implementation**: `src/slack_api.cpp`

```cpp
bool SlackAPI::send_message(const std::string& channel, const std::string& text) {
    // Validate channel ID and message text
    // Build JSON request body
    // Call make_api_request with POST
    // Return success status
}
```

**Usage**:
```cpp
SlackAPI api(bot_token);
if (api.send_message("C1234567890", "Hello!")) {
    // Message sent successfully
}
```

### 2. `conversations.info`

Get information about a channel.

**Endpoint**: `GET https://slack.com/api/conversations.info?channel=C1234567890`

**Response**:
```json
{
  "ok": true,
  "channel": {
    "id": "C1234567890",
    "name": "general",
    "is_channel": true,
    ...
  }
}
```

**Implementation**: `src/slack_api.cpp`

```cpp
bool SlackAPI::get_channel_info(const std::string& channel_id, std::string& info) {
    // Validate channel ID
    // Build URL with query parameter
    // Call make_api_request with GET
    // Return channel info JSON string
}
```

### 3. `users.info`

Get information about a user.

**Endpoint**: `GET https://slack.com/api/users.info?user=U1234567890`

**Response**:
```json
{
  "ok": true,
  "user": {
    "id": "U1234567890",
    "name": "john.doe",
    "real_name": "John Doe",
    ...
  }
}
```

**Implementation**: `src/slack_api.cpp`

```cpp
bool SlackAPI::get_user_info(const std::string& user_id, std::string& info) {
    // Validate user ID
    // Build URL with query parameter
    // Call make_api_request with GET
    // Return user info JSON string
}
```

### 4. `apps.connections.open`

Get dynamic WebSocket URL for Socket Mode v2.

**Endpoint**: `POST https://slack.com/api/apps.connections.open`

**Request**: Empty body (form-urlencoded)

**Response**:
```json
{
  "ok": true,
  "url": "wss://wss-primary.slack.com/link/?ticket=..."
}
```

**Implementation**: `src/slack_api.cpp`

```cpp
bool SlackAPI::get_socket_mode_url(std::string& websocket_url) {
    // Set Content-Type to form-urlencoded
    // Call make_api_request with POST
    // Parse response and extract URL
    // Decode escaped slashes in URL
    // Return websocket_url
}
```

**Note**: This method requires App-Level Token (`xapp-...`) with `connections:write` scope.

## See Also

- [Slack API Overview](slack-api-overview.md) - Architecture overview
- [Slack API - Authentication](slack-api-authentication.md) - Authentication details
- [Slack API - Implementation](slack-api-implementation.md) - Implementation details
