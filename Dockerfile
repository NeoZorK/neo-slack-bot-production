# Dockerfile for Neo Slack Bot testing environment
# Note: Docker is for testing only. Production requires macOS due to macOS-specific features.
FROM --platform=linux/amd64 ubuntu:22.04

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
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
    ca-certificates \
    pkg-config \
    llvm \
    lcov \
    mingw-w64 \
    wine \
    wine64 \
    wine32 \
    xvfb \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Build OpenSSL for Windows (MinGW)
RUN wget https://www.openssl.org/source/openssl-3.0.12.tar.gz \
    && tar -xzf openssl-3.0.12.tar.gz \
    && cd openssl-3.0.12 \
    && ./Configure mingw64 shared --cross-compile-prefix=x86_64-w64-mingw32- --prefix=/usr/local/windows no-tests \
    && make -j$(nproc) \
    && make install_sw \
    && cd .. \
    && rm -rf openssl-3.0.12* \
    && ln -s /usr/local/windows/lib64 /usr/local/windows/lib

# Set WINEPATH to find DLLs (OpenSSL and MinGW runtime)
ENV WINEPATH="Z:\\usr\\local\\windows\\bin;Z:\\usr\\lib\\gcc\\x86_64-w64-mingw32\\10-win32;Z:\\usr\\x86_64-w64-mingw32\\lib"
ENV WINEDEBUG=-all


# Create build directory
WORKDIR /app

# Copy project files
COPY . .

# Build project and tests using CMake with Ninja
# Note: macOS-specific features (notifications) will use Linux implementation (libnotify)
# Support coverage build via build arg
ARG ENABLE_COVERAGE=OFF
RUN mkdir -p build && cd build && \
    cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DENABLE_COVERAGE=${ENABLE_COVERAGE} && \
    cmake --build . --parallel $(nproc)

# Default command: run tests using CTest
CMD ["bash", "-c", "cd build && ctest --output-on-failure"]
