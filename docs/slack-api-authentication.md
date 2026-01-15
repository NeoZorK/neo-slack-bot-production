# Slack API - Authentication

## Token Types

### Bot User OAuth Token (`xoxb-...`)

**Usage**:
- HTTP API requests (send messages, query data)
- WebSocket connection authentication (legacy Socket Mode v1)

**Scopes Required**:
- `chat:write` - Send messages
- `channels:read` - Read channel info
- `channels:history` - Read message history
- `users:read` - Read user info
- `app_mentions:read` - Receive mentions
- `im:read` - Read direct messages
- `im:write` - Send direct messages

**Storage**: Environment variable `SLACK_BOT_TOKEN`

### App-Level Token (`xapp-...`)

**Usage**:
- `apps.connections.open` API call (Socket Mode v2)
- Dynamic WebSocket URL retrieval

**Scopes Required**:
- `connections:write` - Required for Socket Mode v2

**Storage**: Environment variable `SLACK_APP_TOKEN`

**⚠️ IMPORTANT**: Both tokens are required. Without `SLACK_APP_TOKEN`, the application will fail to connect with `403 Forbidden` error.

## Authentication Headers

### HTTP API

```
Authorization: Bearer xoxb-...
Content-Type: application/json
```

### apps.connections.open

```
Authorization: Bearer xapp-...
Content-Type: application/x-www-form-urlencoded
```

### WebSocket Handshake (Dynamic URL)

```
GET wss://wss-primary.slack.com/link/?ticket=...
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: <base64-key>
Sec-WebSocket-Version: 13
```

Note: Dynamic URLs include authentication ticket in the URL itself, so no Authorization header is needed.

## See Also

- [Slack API Overview](slack-api-overview.md) - Architecture overview
- [Slack API - Socket Mode](slack-api-socket-mode.md) - Socket Mode details
- [Getting Slack Token - Quick Guide](getting-slack-token-quick.md) - Quick token setup guide
