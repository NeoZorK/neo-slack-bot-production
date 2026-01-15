# External Libraries Overview

## Overview

Neo Slack Bot uses a minimal set of external dependencies, prioritizing header-only libraries where possible to simplify integration and reduce build complexity. All dependencies are carefully selected for performance, reliability, and ease of use.

## Dependency List

### cpp-httplib

**Purpose**: HTTP client/server library for Slack API requests

**Version**: Latest stable (header-only)

**License**: MIT License

**Type**: Header-only library

**Repository**: https://github.com/yhirose/cpp-httplib

**Integration**:
- Single header file: `httplib.h`
- No build configuration required
- Include directly in source files

**Usage**:
```cpp
#include "httplib.h"

httplib::Client cli("slack.com");
auto res = cli.Post("/api/chat.postMessage", body, "application/json");
```

**Features**:
- Simple, intuitive API
- SSL/TLS support via OpenSSL
- HTTP/1.1 compliant
- Request/response handling
- Header management
- Timeout support

**Benefits**:
- Zero dependencies (except OpenSSL)
- Lightweight implementation
- Cross-platform compatibility
- Active maintenance

### nlohmann/json

**Purpose**: Modern C++ JSON parsing and generation

**Version**: v3.11.2+ (latest stable)

**License**: MIT License

**Type**: Header-only library (single header)

**Repository**: https://github.com/nlohmann/json

**Integration**:
- Single header file: `json.hpp`
- No build configuration required
- Include directly in source files

**Usage**:
```cpp
#include "json.hpp"
using json = nlohmann::json;

json j = json::parse(slack_response);
std::string text = j["message"]["text"];
```

**Features**:
- Intuitive API similar to Python's json
- Type-safe conversions
- Exception-based error handling
- Support for custom types
- JSON Schema validation (optional)

**Benefits**:
- Modern C++ design
- Excellent performance
- Comprehensive documentation
- Wide adoption in C++ community

### OpenSSL

**Purpose**: SSL/TLS support for secure connections

**Version**: System-provided (1.1.x or 3.x)

**License**: Apache 2.0 / OpenSSL License (dual license)

**Type**: System library (shared library)

**Repository**: https://www.openssl.org/

**Integration**:
- **macOS**: Installed via Homebrew (`brew install openssl@3` or `brew install openssl`)
- **Linux**: Installed via system package manager (`apt-get install libssl-dev` or equivalent)
- Linked at build time: `-lssl -lcrypto`
- Headers: `openssl/ssl.h`, `openssl/crypto.h`

**Usage**:
- Used by cpp-httplib for HTTPS
- Used for WebSocket TLS (wss://)
- Certificate validation
- Cryptographic operations

**Features**:
- TLS 1.2+ support
- Certificate chain validation
- Secure random number generation
- Cryptographic hash functions

**Installation** (macOS):
```bash
brew install openssl
```

**Benefits**:
- Industry standard
- Widely trusted
- Comprehensive security features
- System integration

**Note**: OpenSSL is a system dependency, not bundled with the project

### libnotify (Linux only)

**Purpose**: Desktop notifications on Linux systems

**Version**: System-provided (0.7.x or later)

**License**: LGPL 2.1+

**Type**: System library (shared library)

**Repository**: https://gitlab.gnome.org/GNOME/libnotify

**Integration**:
- **Linux**: Installed via system package manager (`apt-get install libnotify-dev` or equivalent)
- **macOS**: Not required (uses native UserNotifications framework)
- **Windows**: Not required (planned WinRT implementation)

**Usage**:
- Used by `LinuxNotificationsImpl` for displaying desktop notifications
- Automatically detected via pkg-config
- Linked at build time: `-lnotify`

**Features**:
- Desktop notification support
- Integration with notification daemon (GNOME, KDE, etc.)
- Configurable timeout and urgency
- Thread-safe API

**Installation** (Linux):
```bash
# Ubuntu/Debian
sudo apt-get install libnotify-dev

# Fedora/RHEL
sudo dnf install libnotify-devel

# Arch Linux
sudo pacman -S libnotify
```

**Benefits**:
- Standard Linux notification API
- Works with all major desktop environments
- Lightweight and efficient
- Well-maintained

**Note**: libnotify is only required on Linux. macOS uses native frameworks, Windows support is planned.

## WebSocket Implementation

### Custom WebSocket Client

**Status**: ✅ Implemented

**Purpose**: WebSocket client for Slack Socket Mode

**Protocol**: RFC 6455 (WebSocket Protocol)

**Implementation Approach**:
- Custom implementation using OpenSSL
- Handshake and frame handling
- Message parsing and construction
- Connection management

**Dependencies**:
- OpenSSL for TLS support
- System socket APIs
- No external WebSocket library

**Rationale**:
- Minimal dependencies
- Full control over implementation
- Optimized for Slack Socket Mode
- Lightweight solution

## Dependency Management Strategy

### Header-Only Libraries

**Advantages**:
- No build configuration needed
- Easy to include in project
- Version controlled with project
- Simple updates

**Management**:
- Included directly in project
- Version controlled in repository
- Easy to update manually
- No external build step

### System Libraries

**Advantages**:
- System-wide installation
- Shared across applications
- System updates apply
- Optimized for platform

**Management**:
- Installed via package manager
- Documented in installation guide
- Version requirements specified
- Fallback options considered

## License Compliance

### License Summary

**cpp-httplib**: MIT License
- Permissive license
- Commercial use allowed
- Modification allowed
- Distribution allowed

**nlohmann/json**: MIT License
- Permissive license
- Commercial use allowed
- Modification allowed
- Distribution allowed

**OpenSSL**: Apache 2.0 / OpenSSL License
- Dual license option
- Apache 2.0 is permissive
- OpenSSL License has specific requirements
- Check license for your use case

**libnotify**: LGPL 2.1+
- Lesser GPL license
- Allows linking with proprietary code
- Modification requires LGPL compliance
- Check license for your use case

### License Compatibility

All dependencies use permissive licenses compatible with private/commercial projects.

**Compliance**:
- Include license notices (if distributing)
- Respect license terms
- Document dependencies
- Track license changes

## Summary

Neo Slack Bot maintains a minimal, carefully selected set of dependencies:

- **2 header-only libraries**: cpp-httplib, nlohmann/json
- **1-2 system libraries**: OpenSSL (all platforms), libnotify (Linux only)
- **Total external dependencies**: 3-4 (platform-dependent)

This minimal approach ensures:
- Easy installation and setup
- Fast builds
- Low maintenance burden
- Clear dependency tree
- Security through simplicity

## See Also

- [External Libraries Detailed](external-libs-detailed.md) - Detailed integration, security, and maintenance information
