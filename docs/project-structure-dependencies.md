# Project Structure - Dependencies

## Overview

This document covers dependency management, internal dependencies, and future structure considerations.

## Dependency Management

### External Libraries

**Header-Only Libraries**:
- Included directly in project
- No build configuration needed
- Version controlled with project

**System Libraries**:
- Managed via package manager
- Linked at build time
- Documented in `docs/external-libs-overview.md`

### Internal Dependencies

**Dependency Graph**:
```
main.cpp
├── websocket_client
├── macos_notifications
└── slack_api
    └── http_client
```

**Circular Dependencies**: Avoided through:
- Forward declarations
- Interface segregation
- Dependency inversion

### Interface Abstractions

**Interfaces**:
- `IHttpClient` - HTTP client interface
- `IWebSocketClient` - WebSocket client interface
- `INotificationService` - Notification service interface
- `ISystemMetrics` - System metrics interface (memory, CPU)

**Benefits**:
- Testability
- Flexibility
- Dependency inversion
- Mock support

## Future Structure Considerations

### Potential Additions

- `include/` - Public headers (if library)
- `third_party/` - External dependencies
- `platform/` - Platform-specific code
- `config/` - Configuration files
- `logs/` - Log files (excluded from git)

### Scalability

As project grows:
- Create subdirectories for major modules
- Organize by feature/domain
- Maintain clear boundaries
- Keep structure flat when possible

### Platform-Specific Structure

**Current Structure**:
```
src/
├── platform/
│   ├── macos/
│   │   ├── macos_notifications_impl.cpp/hpp
│   │   └── macos_system_metrics.cpp/hpp
│   ├── linux/
│   │   ├── linux_notifications_impl.cpp/hpp (requires libnotify)
│   │   └── linux_system_metrics.cpp/hpp
│   └── windows/
│       ├── windows_notifications_impl.cpp/hpp (stub)
│       └── windows_system_metrics.cpp/hpp (stub)
├── interfaces/
│   ├── i_notification_service.hpp
│   └── i_system_metrics.hpp
├── notifications_factory.cpp/hpp
└── system_metrics_factory.cpp/hpp
```

**Platform-Specific Dependencies**:
- **macOS**: Foundation, AppKit frameworks (system)
- **Linux**: libnotify (via pkg-config), /proc filesystem
- **Windows**: WinRT (planned, currently stubs)

## See Also

- [Project Structure Overview](project-structure-overview.md) - Quick overview of project structure
- [Project Structure - Module Details](project-structure-modules.md) - Detailed module information
- [Project Structure - Build Details](project-structure-build.md) - Build artifacts and organization
