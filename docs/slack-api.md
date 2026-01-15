# Slack API Integration

## Overview

Neo Slack Bot integrates with Slack's API through two primary mechanisms:

1. **Socket Mode v2 (WebSocket)** - Real-time event streaming via WebSocket connection
2. **HTTP API** - RESTful API calls for sending messages and querying data

This document provides an overview of the Slack API integration. Detailed information is split across multiple documents for better organization.

## Documentation Structure

The Slack API documentation is organized into the following documents:

- **[Slack API - Overview](slack-api-overview.md)** - Architecture overview and component responsibilities

- **[Slack API - Socket Mode](slack-api-socket-mode.md)** - Socket Mode v2 integration details, connection flow, frame handling, and event types

- **[Slack API - HTTP Methods](slack-api-http.md)** - HTTP API methods, endpoints, request/response formats, and implementation details

- **[Slack API - Authentication](slack-api-authentication.md)** - Token types, authentication headers, and security considerations

- **[Slack API - Event Processing](slack-api-events.md)** - Event flow, parsing, acknowledgment, and message processing

- **[Slack API - Implementation](slack-api-implementation.md)** - Implementation details, error handling, rate limiting, best practices, and troubleshooting

## Quick Reference

- Base API URL: `https://slack.com/api/`
- WebSocket URL: Dynamic (obtained via `apps.connections.open`)
- Bot Token: `xoxb-...` (for HTTP API)
- App-Level Token: `xapp-...` (for Socket Mode v2)

## See Also

- [Slack API Documentation](https://api.slack.com/)
- [Socket Mode Guide](https://api.slack.com/apis/connections/socket)
- [WebSocket RFC 6455](https://tools.ietf.org/html/rfc6455)
