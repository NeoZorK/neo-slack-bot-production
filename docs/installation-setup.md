# Installation Guide - Requirements and Setup

# Installation Guide

## System Requirements

### Operating System
- **macOS**: 10.14 (Mojave) or later
- **Linux**: Ubuntu 20.04+, Debian 11+, or compatible distributions
- **Architecture**: x86_64 (Intel) or ARM64 (Apple Silicon / ARM)
- **Disk Space**: ~100 MB for source code and dependencies
- **RAM**: 512 MB minimum (2 GB recommended)

### Development Tools
- **CMake**: Version 3.15 or later (required)
- **Ninja**: Build system generator (required)
- **Xcode Command Line Tools**: Required for clang++ compiler
- **Git**: For cloning repository (version 2.0+)

## Prerequisites Installation

### 1. Install Xcode Command Line Tools

```bash
xcode-select --install
```

Verify installation:
```bash
clang++ --version
```

### 2. Install CMake and Ninja

```bash
# Install CMake
brew install cmake

# Install Ninja (recommended for faster builds)
brew install ninja

# Verify installation
cmake --version  # Should show 3.15 or later
ninja --version  # Optional, but recommended
```

### 3. Install Homebrew (if not already installed)

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### 4. Install OpenSSL

#### macOS

**On Apple Silicon (M1/M2/M3)**:
```bash
brew install openssl@3
```

**On Intel Macs**:
```bash
brew install openssl
```

**Note**: CMake automatically detects the correct OpenSSL installation:
- Tries `openssl@3` first (common on Apple Silicon)
- Falls back to `openssl` if `openssl@3` is not available
- Falls back to common paths if Homebrew is not available

Verify installation:
```bash
# Check which version is installed
brew list openssl@3 || brew list openssl
openssl version
```

**Note**: On Apple Silicon Macs, OpenSSL may be installed as `openssl@3`. The build system handles this automatically, but if you encounter issues, you can manually set paths:
```bash
# For openssl@3
export LDFLAGS="-L$(brew --prefix openssl@3)/lib"
export CPPFLAGS="-I$(brew --prefix openssl@3)/include"

# For openssl
export LDFLAGS="-L$(brew --prefix openssl)/lib"
export CPPFLAGS="-I$(brew --prefix openssl)/include"
```

#### Linux

**Ubuntu/Debian**:
```bash
sudo apt-get update
sudo apt-get install -y libssl-dev pkg-config libnotify-dev
```

**Fedora/RHEL/CentOS**:
```bash
sudo dnf install openssl-devel pkg-config libnotify-devel
# or for older versions:
sudo yum install openssl-devel pkg-config libnotify-devel
```

**Arch Linux**:
```bash
sudo pacman -S openssl pkg-config libnotify
```

Verify installation:
```bash
pkg-config --modversion openssl
pkg-config --modversion libnotify
```

## Project Setup

### 1. Clone Repository

```bash
git clone https://github.com/NeoZorK/neo-slack-bot.git
cd neo-slack-bot
```

### 2. Checkout Desired Version

```bash
git checkout v0.0.1  # or latest stable version
```

### 3. Verify Project Structure

```bash
ls -la
# Should see: docs/, src/, tests/, README.md, CLAUDE.md
```

## Dependencies Installation

### Header-Only Libraries

These libraries are header-only and will be included directly in the project:

#### cpp-httplib
- **Location**: Will be added to `src/include/` or `src/third_party/`
- **Installation**: Download header file or use git submodule
- **No build step required**

#### nlohmann/json
- **Location**: Will be added to `src/include/` or `src/third_party/`
- **Installation**: Download single header file (`json.hpp`)
- **No build step required**

**Future**: These may be managed via git submodules or package manager.

### System Libraries

#### OpenSSL
- **macOS**: Already installed via Homebrew (see Prerequisites section)
- **Linux**: Installed via system package manager (see Prerequisites section)

**Verify OpenSSL Paths (macOS)**:
```bash
brew --prefix openssl
# Should output: /opt/homebrew/opt/openssl (Apple Silicon)
# or: /usr/local/opt/openssl (Intel)
```

**Verify OpenSSL (Linux)**:
```bash
pkg-config --modversion openssl
# Should show OpenSSL version
```

#### libnotify (Linux only)
Required for desktop notifications on Linux systems.

**Installation**: Already installed via system package manager (see Prerequisites section)

**Verify libnotify**:
```bash
pkg-config --modversion libnotify
# Should show libnotify version (e.g., 0.7.7)
```

## Build Process

### 1. Configure CMake Build System

```bash
# Create build directory
mkdir -p build
cd build

# Configure CMake (uses Ninja if available, otherwise Makefiles)
cmake ..
```

This will:
- Detect OpenSSL installation
- Configure compiler settings
- Set up build system (Ninja or Makefiles)
- Enable parallel builds automatically

### 2. Build Release Version (default)

```bash
# Build with parallel compilation
cmake --build .
```

This will:
- Compile all source files with optimizations (`-O2`)
- Link with OpenSSL libraries
- Create executable in `build/bin/neo-slack-bot`
- Use all available CPU cores for parallel compilation

### 3. Build Debug Version (for development)

```bash
# Configure for debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build debug version
cmake --build .
```

This will:
- Compile with debug symbols (`-g`)
- Enable debug macros (`-DDEBUG`)
- Create executable with debugging information

### 4. Verify Build

```bash
# Check executable exists
file build/bin/neo-slack-bot

# Run executable (when implemented)
./build/bin/neo-slack-bot --version
```

### 5. Clean Build

```bash
cd build
# Remove all build artifacts
rm -rf *
# Or use CMake clean target
cmake --build . --target clean
```


## See Also

- [Installation Guide - Build and Verification](installation-build.md) - Build process and verification
- [Installation Guide](installation.md) - Overview
