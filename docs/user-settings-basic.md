# User Settings - Basic Configuration

## Overview

Neo Slack Bot supports configurable user settings to customize behavior, notifications, and connection parameters. Settings can be managed through configuration files or environment variables.

## Notification Settings

### Enable/Disable Notifications

**Setting**: `notifications.enabled` (Boolean, default: `true`)

**Usage**:
```json
{
  "notifications": {
    "enabled": true
  }
}
```

**Environment Variable**: `NEO_SLACK_NOTIFICATIONS_ENABLED=1`

### Notification Sound

**Setting**: `notifications.sound_enabled` (Boolean, default: `true`)

**Usage**:
```json
{
  "notifications": {
    "sound_enabled": true
  }
}
```

### Do Not Disturb Hours

**Setting**: `notifications.dnd_hours` (Object, default: disabled)

**Usage**:
```json
{
  "notifications": {
    "dnd_hours": {
      "enabled": true,
      "start": "22:00",
      "end": "08:00"
    }
  }
}
```

**Features**: 24-hour format (HH:MM), respects macOS Do Not Disturb

### Channel Filtering

**Setting**: `notifications.channel_filter` (Object, default: all channels)

**Usage**:
```json
{
  "notifications": {
    "channel_filter": {
      "mode": "blacklist",
      "channels": ["C1234567890"]
    }
  }
}
```

**Modes**: `whitelist`, `blacklist`, `all` (default)

## Connection Settings

### Reconnection Attempts

**Setting**: `connection.max_retries` (Integer, default: `10`)

**Usage**:
```json
{
  "connection": {
    "max_retries": 10
  }
}
```

**Behavior**: Exponential backoff between retries, reset on successful connection

### Connection Timeout

**Setting**: `connection.timeout_seconds` (Integer, default: `30`)

**Usage**:
```json
{
  "connection": {
    "timeout_seconds": 30
  }
}
```

### Ping Interval

**Setting**: `connection.ping_interval_seconds` (Integer, default: `30`)

**Usage**:
```json
{
  "connection": {
    "ping_interval_seconds": 30
  }
}
```

**Purpose**: Keep connection alive, detect disconnections

### Retry Delay

**Setting**: `connection.retry_delay_seconds` (Integer, default: `5`)

**Usage**:
```json
{
  "connection": {
    "retry_delay_seconds": 5
  }
}
```

**Backoff Strategy**: Initial delay × 2^attempt, capped at 60 seconds

## Event Filtering

### Message Types

**Setting**: `events.message_types` (Array, default: all types)

**Usage**:
```json
{
  "events": {
    "message_types": ["message", "app_mention"]
  }
}
```

**Available Types**: `message`, `app_mention`, `reaction_added`, `channel_created`, `member_joined_channel`

### Keyword Filtering

**Setting**: `events.keyword_filter` (Object, default: disabled)

**Usage**:
```json
{
  "events": {
    "keyword_filter": {
      "enabled": true,
      "mode": "include",
      "keywords": ["urgent", "important"]
    }
  }
}
```

**Modes**: `include` - only messages with keywords, `exclude` - exclude messages with keywords

### User Filtering

**Setting**: `events.user_filter` (Object, default: all users)

**Usage**:
```json
{
  "events": {
    "user_filter": {
      "mode": "whitelist",
      "users": ["U1234567890"]
    }
  }
}
```

**Modes**: `whitelist`, `blacklist`, `all` (default)

## Configuration File

### File Location

**Default Path**: `~/.config/neo-slack-bot/config.json`

**macOS Path**: `~/Library/Application Support/neo-slack-bot/config.json`

**File Format**: JSON

**Permissions**: 600 (owner read/write only)

### Complete Example

```json
{
  "notifications": {
    "enabled": true,
    "sound_enabled": true,
    "dnd_hours": {
      "enabled": false
    },
    "channel_filter": {
      "mode": "all"
    }
  },
  "connection": {
    "max_retries": 10,
    "timeout_seconds": 30,
    "ping_interval_seconds": 30,
    "retry_delay_seconds": 5
  },
  "events": {
    "message_types": ["message", "app_mention"],
    "keyword_filter": {
      "enabled": false
    },
    "user_filter": {
      "mode": "all"
    }
  }
}
```

### Environment Variables

Environment variables take precedence over config file:

```bash
export NEO_SLACK_NOTIFICATIONS_ENABLED=0
export NEO_SLACK_CONNECTION_MAX_RETRIES=5
```

**Naming Convention**: `NEO_SLACK_<CATEGORY>_<SETTING>`

**Conversion**: `notifications.enabled` → `NEO_SLACK_NOTIFICATIONS_ENABLED`

### Default Values

**Fallback Order**:
1. Environment variable
2. Configuration file
3. Built-in defaults

**Default Configuration**:
- All notifications enabled
- All channels enabled
- Standard connection settings
- No filtering

## Validation

Settings are validated on load:
- Type checking (JSON types)
- Range validation for numbers
- Format validation for strings
- Enum validation for modes

Invalid values use defaults. Missing settings use defaults.

## Best Practices

1. Start with defaults, customize as needed
2. Validate on startup
3. Keep settings documented
4. Protect sensitive settings
5. Provide clear error messages

## See Also

- [User Settings - Advanced Configuration](user-settings-advanced.md) - Advanced settings and future features
