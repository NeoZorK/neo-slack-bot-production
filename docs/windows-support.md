# Windows Platform Support

## Overview

Windows support is in a **basic headless/stub phase** and is planned to reach a clearly defined baseline in v0.0.7. The project includes stub implementations for Windows-specific components that are sufficient for core event processing and console-based notifications.

## Current Status

### ✅ Completed (pre-v0.0.7)

- Platform abstractions for Windows through interfaces
- CMake configuration for Windows (MSVC or MinGW)
- Stub implementations for:
  - Windows Notifications (`WindowsNotificationsImpl`)
  - Windows System Metrics (`WindowsSystemMetrics`)

### ⚠️ Limitations

- **Notifications**: Currently logs to console only. Windows Toast Notifications API integration is planned.
- **System Metrics**: Returns default/placeholder values. Windows API integration (`GetProcessMemoryInfo`, `GetProcessTimes`) is planned.
- **Testing**: Windows builds have not been tested in CI/CD yet.

## Requirements

### System Requirements

- **OS**: Windows 10 (version 1809+) or Windows 11
- **Architecture**: x86_64 (64-bit)
- **Compiler**: 
  - MSVC 2019 or later (Visual Studio 2019+)
  - OR MinGW-w64 with GCC 9.0+
- **Build Tools**: CMake 3.15+

### Dependencies

- **OpenSSL**: Required for SSL/TLS support
  - **MSVC**: Install via [vcpkg](https://vcpkg.io/) or [Shining Light Productions](https://slproweb.com/products/Win32OpenSSL.html)
  - **MinGW**: Install via `pacman -S mingw-w64-x86_64-openssl` (MSYS2)
- **Windows SDK**: Required for Windows API (included with Visual Studio)

## Building on Windows

### Using Visual Studio (MSVC)

1. **Install Prerequisites**:
   ```powershell
   # Install CMake
   choco install cmake
   # OR download from https://cmake.org/download/
   
   # Install OpenSSL via vcpkg
   vcpkg install openssl:x64-windows
   ```

2. **Configure Build**:
   ```powershell
   mkdir build
   cd build
   cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
   ```

3. **Build**:
   ```powershell
   cmake --build . --config Release
   ```

### Using MinGW-w64 (MSYS2)

1. **Install Prerequisites**:
   ```bash
   # Install MSYS2 from https://www.msys2.org/
   # Then install dependencies:
   pacman -S mingw-w64-x86_64-cmake
   pacman -S mingw-w64-x86_64-openssl
   pacman -S mingw-w64-x86_64-gcc
   ```

2. **Configure Build**:
   ```bash
   mkdir build
   cd build
   cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=/mingw64
   ```

3. **Build**:
   ```bash
   cmake --build .
   ```

## Implementation Details

### Windows Notifications

**Current Implementation**: Stub that logs to console.

**Planned Implementation**: Windows Toast Notifications API (Windows 10+).

**API Requirements**:
- `Windows.UI.Notifications` namespace (WinRT)
- `ToastNotificationManager` for creating notifications
- App ID registration in Windows registry

**Example Future Implementation**:
```cpp
// TODO: Implement Windows Toast Notifications
#include <windows.ui.notifications.h>
#include <wrl.h>

// Use WinRT APIs for notifications
```

### Windows System Metrics

**Current Implementation**: Returns placeholder values.

**Planned Implementation**: Windows Performance API.

**API Requirements**:
- `GetProcessMemoryInfo` from `psapi.h` for memory metrics
- `GetProcessTimes` for CPU usage
- `GlobalMemoryStatusEx` for system memory

**Example Future Implementation**:
```cpp
// TODO: Implement Windows Performance API
#include <windows.h>
#include <psapi.h>

PROCESS_MEMORY_COUNTERS_EX pmc;
GetProcessMemoryInfo(GetCurrentProcess(), 
                     (PROCESS_MEMORY_COUNTERS*)&pmc, 
                     sizeof(pmc));
```

## CMake Configuration

Windows-specific CMake settings are handled in `src/CMakeLists.txt`:

```cmake
elseif(WIN32)
    list(APPEND SOURCES
        platform/windows/windows_notifications_impl.cpp
        platform/windows/windows_system_metrics.cpp
    )
endif()
```

**Note**: Windows-specific libraries are not yet linked. Future implementation will require:
- `windowsapp.lib` (for WinRT notifications)
- `psapi.lib` (for system metrics)

## Testing

Windows builds are not yet tested in CI/CD. Manual testing is required in v0.0.7:

1. **Build Verification**:
   ```powershell
   # Verify build succeeds
   cmake --build . --config Release
   ```
   
2. **Runtime Verification**:
   ```powershell
   # Run executable (notifications will log to console)
   .\bin\Release\neo-slack-bot.exe
   ```
   - Send a simple test event (or use a mock) to confirm that:
     - Events are processed
     - Console-based notification output appears

### v0.0.7 Acceptance Checklist (Windows)

- [ ] Project configures and builds on at least one Windows toolchain (MSVC or MinGW)
- [ ] Core bot binary starts and can process a minimal event flow
- [ ] Console-based notification stub logs expected output
- [ ] No Windows-specific build errors remain in the main configuration
- [ ] Windows limitations and future work are documented and aligned with v0.0.7 plan

## Future Work

### Phase 1: Basic Windows Support
- [ ] Implement Windows Toast Notifications API
- [ ] Implement Windows Performance API for metrics
- [ ] Add Windows-specific libraries to CMake
- [ ] Test on Windows 10/11

### Phase 2: CI/CD Integration
- [ ] Add Windows runner to GitHub Actions
- [ ] Test builds on Windows (MSVC and MinGW)
- [ ] Verify all tests pass on Windows

### Phase 3: Production Readiness
- [ ] Full feature parity with macOS/Linux
- [ ] Performance optimization
- [ ] Documentation updates

## Troubleshooting

### Build Issues

**OpenSSL Not Found**:
```
CMake Error: Could not find OpenSSL
```
**Solution**: Install OpenSSL via vcpkg or MSYS2, and set `CMAKE_PREFIX_PATH` or `OPENSSL_ROOT_DIR`.

**Windows SDK Not Found**:
```
CMake Error: Could not find Windows SDK
```
**Solution**: Install Windows SDK via Visual Studio Installer.

### Runtime Issues

**Notifications Not Working**:
- Expected: Currently logs to console only
- Future: Will use Windows Toast Notifications API

**Metrics Return Zero**:
- Expected: Currently returns placeholder values
- Future: Will use Windows Performance API

## See Also

- [Installation Guide](installation.md) - General installation instructions
- [Development Setup](development-setup.md) - Development environment setup
- [Project Structure - Dependencies](project-structure-dependencies.md) - Platform-specific dependencies
