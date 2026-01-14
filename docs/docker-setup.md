# Docker Guide - Overview and Setup

# Docker Guide

## Overview

This guide explains how to use Docker with Neo Slack Bot for testing, development, and CI/CD purposes.

**⚠️ Important**: Docker builds are **limited** due to macOS-specific dependencies. The bot requires macOS frameworks (Foundation, AppKit) and macOS notification APIs that are not available in Linux containers. Docker is suitable for testing core functionality (WebSocket, HTTP API, JSON parsing) but **not** for full feature testing or production deployment.

## Prerequisites

- Docker Engine 20.10+ installed
- Docker Compose 2.0+ installed (optional, for docker-compose.yml)
- At least 2 GB of free disk space

## Dockerfile

The project includes a `Dockerfile` for building a cross-platform testing environment (Linux & Windows via Wine):

```dockerfile
FROM --platform=linux/amd64 ubuntu:22.04

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies including Wine for Windows support
RUN dpkg --add-architecture i386 && \
    apt-get update && apt-get install -y \
    build-essential \
    clang \
    clang-format \
    clang-tidy \
    cmake \
    ninja-build \
    libssl-dev \
    libnotify-dev \
    git \
    curl \
    wget \
    ca-certificates \
    pkg-config \
    llvm \
    lcov \
    wine32 \
    xvfb \
    mingw-w64 \
    && rm -rf /var/lib/apt/lists/*

# Pre-download and setup OpenSSL for Windows (MinGW)
RUN wget https://www.openssl.org/source/openssl-1.1.1w.tar.gz && \
    tar -xzf openssl-1.1.1w.tar.gz && \
    cd openssl-1.1.1w && \
    ./Configure mingw64 --cross-compile-prefix=x86_64-w64-mingw32- --prefix=/usr/local/windows --openssldir=/usr/local/windows shared zlib-dynamic && \
    make && make install && \
    cd .. && rm -rf openssl-1.1.1w*

# Setup WINEPATH to find DLLs
ENV WINEPATH="Z:\\usr\\local\\windows\\bin;Z:\\usr\\lib\\gcc\\x86_64-w64-mingw32\\10-win32;Z:\\usr\\x86_64-w64-mingw32\\lib"

WORKDIR /app
COPY . .

# Build project and tests using CMake with Ninja (Linux default)
ARG ENABLE_COVERAGE=OFF
RUN mkdir -p build && cd build && \
    cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DENABLE_COVERAGE=${ENABLE_COVERAGE} && \
    cmake --build . --parallel $(nproc)

# Default command: run Linux tests
CMD ["bash", "-c", "cd build && ctest --output-on-failure"]
```

**⚠️ Important**: The Dockerfile builds a Linux testing environment and includes Wine/MinGW for Windows cross-compilation and testing. macOS-specific features are still not available.

## Building Docker Image (v0.0.7 Baseline)

### Build Image

```bash
docker build -t neo-slack-bot:latest .
```

### Build with Tag

```bash
docker build -t neo-slack-bot:v0.0.6 .
```

## Running Tests in Docker (Core Suite)

### Run Tests

```bash
docker run --rm neo-slack-bot:latest
```

### Run Tests with Environment Variables

**Both tokens are required** (even for testing):

```bash
docker run --rm \
  -e SLACK_BOT_TOKEN=test-token \
  -e SLACK_APP_TOKEN=test-app-token \
  neo-slack-bot:latest
```

### Run Performance/Stress Tests (Optional)

```bash
docker run --rm neo-slack-bot:latest bash -c "cd tests && make test-perf"
```

## Docker Compose

The project includes `docker-compose.yml` for orchestrated testing:

### Services

1. **test**: Runs standard test suite
2. **test-perf**: Runs performance tests
3. **coverage**: Generates code coverage report

### Usage

#### Run Standard Tests

```bash
docker-compose run --rm test
```

#### Run Performance Tests

```bash
docker-compose run --rm test-perf
```

#### Generate Coverage Report

```bash
docker-compose run --rm coverage
```

#### Run All Services

```bash
docker-compose up
```

## Development Workflow (Linux Focus)

### Mount Source Code

For development with live code changes:

```bash
docker run --rm -v $(pwd):/app neo-slack-bot:latest bash -c "mkdir -p build && cd build && cmake -G Ninja .. && ninja && ctest --output-on-failure"
```

### Interactive Shell

```bash
docker run --rm -it -v $(pwd):/app neo-slack-bot:latest bash
```

Inside container:
```bash
mkdir -p build
cd build
cmake -G Ninja ..
ninja
ctest --output-on-failure
```

## CI/CD Integration (v0.0.7 Expectations)

### GitHub Actions Example

```yaml
name: Docker Tests

on: [push, pull_request]

jobs:

## See Also

- [Docker Guide - Usage](docker-usage.md) - Docker usage and examples
- [Docker Guide](docker.md) - Overview
