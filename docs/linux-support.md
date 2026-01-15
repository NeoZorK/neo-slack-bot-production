# Linux Platform Support

## Overview

Linux support is a **first-class target** for v0.0.7. The project includes complete implementations for Linux-specific components using standard Linux libraries and APIs, and v0.0.7 formalizes Linux as a supported environment for headless and CI workloads.

## Current Status

### ✅ Completed (pre-v0.0.7)

- **Platform Abstraction**: Linux-specific implementations are abstracted through interfaces
- **CMake Configuration**: CMake is configured to build on Linux (detects libnotify via pkg-config)
- **Notifications**: Full implementation using libnotify library
- **System Metrics**: Full implementation using /proc filesystem
- **Docker Support**: Dockerfile uses Linux build with CMake/Ninja

### 🎯 v0.0.7 Targets

- Treat Linux as a reference CI platform alongside macOS
- Ensure `test_neo_slack_bot` passes on at least one reference distribution
- Validate notification flow using libnotify in a real desktop session
- Document any Linux-specific limitations or behavioral differences

### Features

- **Desktop Notifications**: Native Linux desktop notifications via libnotify
- **System Metrics**: Memory and CPU usage tracking via /proc/self/status and /proc/stat
- **Thread Safety**: All implementations are thread-safe

## Requirements

### System Requirements

- **OS**: Linux distributions (Ubuntu 20.04+, Debian 11+, Fedora 34+, etc.)
- **Architecture**: x86_64 (64-bit) or ARM64
- **Compiler**: 
  - GCC 9.0+ or Clang 10.0+
- **Build Tools**: CMake 3.15+, Ninja (recommended)

### Dependencies

- **OpenSSL**: Required for SSL/TLS support
  - **Ubuntu/Debian**: `sudo apt-get install libssl-dev`
  - **Fedora/RHEL**: `sudo dnf install openssl-devel`
  - **Arch**: `sudo pacman -S openssl`
- **libnotify**: Required for desktop notifications
  - **Ubuntu/Debian**: `sudo apt-get install libnotify-dev`
  - **Fedora/RHEL**: `sudo dnf install libnotify-devel`
  - **Arch**: `sudo pacman -S libnotify`
- **pkg-config**: Required for finding libnotify
  - Usually included with development tools

## Building on Linux

### Using CMake with Ninja (Recommended)

1. **Install Prerequisites**:
   ```bash
   # Ubuntu/Debian
   sudo apt-get update
   sudo apt-get install build-essential cmake ninja-build \
       libssl-dev libnotify-dev pkg-config clang
   
   # Fedora/RHEL
   sudo dnf install gcc-c++ cmake ninja-build \
       openssl-devel libnotify-devel pkg-config clang
   
   # Arch Linux
   sudo pacman -S base-devel cmake ninja \
       openssl libnotify pkg-config clang
   ```

2. **Configure Build**:
   ```bash
   mkdir build
   cd build
   cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
   ```

3. **Build**:
   ```bash
   cmake --build . --parallel $(nproc)
   ```

4. **Run Tests**:
   ```bash
   ctest --output-on-failure
   ```

### Using CMake with Make

1. **Configure Build**:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

2. **Build**:
   ```bash
   cmake --build . -j$(nproc)
   ```

## Implementation Details

### Linux Notifications

**Implementation**: Full implementation using libnotify.

**Library**: libnotify (GNOME notification daemon)

**Features**:
- Desktop notifications with title and body
- Subtitle support (combined with body)
- Configurable timeout (default: 5 seconds)
- Urgency levels (normal, low, critical)
- Thread-safe implementation with mutex protection

**API Usage**:
```cpp
#include <libnotify/notify.h>

// Initialize libnotify
notify_init("neo-slack-bot");

// Create notification
NotifyNotification* notification = notify_notification_new(
    title.c_str(), body.c_str(), nullptr);

// Set timeout and urgency
notify_notification_set_timeout(notification, 5000);
notify_notification_set_urgency(notification, NOTIFY_URGENCY_NORMAL);

// Show notification
notify_notification_show(notification, nullptr);
```

**Thread Safety**: libnotify is generally thread-safe, but the implementation uses a mutex for additional safety when accessing internal state.

### Linux System Metrics

**Implementation**: Full implementation using /proc filesystem.

**Memory Metrics**: Reads from `/proc/self/status` (VmRSS field)

**CPU Metrics**: Calculates from `/proc/stat` and `/proc/self/stat`

**Features**:
- Process memory usage (RSS - Resident Set Size)
- Process CPU usage percentage
- Thread-safe file reading operations

**File Reading**:
```cpp
// Memory from /proc/self/status
std::ifstream status_file("/proc/self/status");
// Parse VmRSS line

// CPU from /proc/stat and /proc/self/stat
std::ifstream stat_file("/proc/stat");
std::ifstream self_stat_file("/proc/self/stat");
// Calculate CPU percentage
```

**Thread Safety**: File reading operations are safe to call from multiple threads.

## CMake Configuration

Linux-specific CMake settings are handled in `src/CMakeLists.txt`:

```cmake
elseif(UNIX AND NOT APPLE)
    list(APPEND SOURCES
        platform/linux/linux_notifications_impl.cpp
        platform/linux/linux_system_metrics.cpp
    )
    # Find libnotify for Linux
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBNOTIFY REQUIRED libnotify)
    target_link_libraries(neo-slack-bot PRIVATE ${LIBNOTIFY_LIBRARIES})
    target_include_directories(neo-slack-bot PRIVATE ${LIBNOTIFY_INCLUDE_DIRS})
endif()
```

**Dependencies**:
- `libnotify` library (linked via pkg-config)
- OpenSSL (detected automatically by CMake)

## Testing

### Local Testing

1. **Build Tests**:
   ```bash
   cd build
   cmake --build . --target test_neo_slack_bot
   ```
   
2. **Run Tests**:
   ```bash
   ctest --output-on-failure
   ```
   
3. **Run with Coverage**:
   ```bash
   cmake .. -DENABLE_COVERAGE=ON
   cmake --build .
   ctest --output-on-failure
   cmake --build . --target coverage
   ```

4. **Manual Notification Check** (optional but recommended):
   - Run the bot with a simple configuration that triggers a notification
   - Verify that a desktop notification appears via libnotify

### v0.0.7 Acceptance Checklist (Linux)

- [ ] Project configures and builds with CMake + Ninja on a reference distro
- [ ] `test_neo_slack_bot` passes without Linux-specific failures
- [ ] At least one manual notification scenario is verified
- [ ] System metrics return non-zero, reasonable values for CPU and memory
- [ ] Docker-based Linux tests are wired into at least one CI pipeline

### Docker Testing

The project includes a Dockerfile configured for Linux testing:

```bash
# Build Docker image
docker build -t neo-slack-bot:test .

# Run tests
docker run --rm neo-slack-bot:test

# Or use docker-compose
docker-compose up test
```

**Note**: Docker is for testing only. Production requires macOS due to macOS-specific features.

## Troubleshooting

### Build Issues

**libnotify Not Found**:
```
CMake Error: Could not find libnotify
```
**Solution**: Install libnotify development package:
```bash
# Ubuntu/Debian
sudo apt-get install libnotify-dev

# Fedora/RHEL
sudo dnf install libnotify-devel

# Arch
sudo pacman -S libnotify
```

**OpenSSL Not Found**:
```
CMake Error: Could not find OpenSSL
```
**Solution**: Install OpenSSL development package:
```bash
# Ubuntu/Debian
sudo apt-get install libssl-dev

# Fedora/RHEL
sudo dnf install openssl-devel

# Arch
sudo pacman -S openssl
```

### Runtime Issues

**Notifications Not Showing**:
- **Check**: Desktop environment supports notifications (GNOME, KDE, XFCE, etc.)
- **Check**: Notification daemon is running (`pgrep -a notification`)
- **Check**: D-Bus is running (`pgrep -a dbus`)
- **Solution**: Ensure you're running in a desktop environment with notification support

**Metrics Return Zero**:
- **Check**: `/proc/self/status` is readable (`cat /proc/self/status`)
- **Check**: `/proc/stat` is readable (`cat /proc/stat`)
- **Solution**: Ensure running on a Linux system with /proc filesystem

**Permission Denied**:
- **Check**: User has read access to /proc filesystem
- **Solution**: Run as regular user (not root required)

## See Also

- [Installation Guide](installation.md) - General installation instructions
- [Development Setup](development-setup.md) - Development environment setup
- [Docker Setup](docker-setup.md) - Docker testing environment
- [Windows Support](windows-support.md) - Windows platform support (stub)
- [Project Structure - Dependencies](project-structure-dependencies.md) - Platform-specific dependencies
