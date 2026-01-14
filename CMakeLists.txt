cmake_minimum_required(VERSION 3.15)

project(neo-slack-bot
    VERSION 0.0.7
    DESCRIPTION "Neo Slack Bot - Real-time Slack event processing with macOS notifications"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enforce Ninja generator
if(NOT CMAKE_GENERATOR MATCHES "Ninja")
    message(FATAL_ERROR "This project requires Ninja generator. Please run cmake with -G Ninja")
endif()

# Enable parallel builds
# For Ninja: automatically uses all cores, override with: cmake --build . -j N
# For Makefiles: use all cores by default, override with: cmake --build . -j N
include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
    set(CMAKE_BUILD_PARALLEL_LEVEL ${N} CACHE STRING "Number of parallel build jobs (empty = auto)")
endif()

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# Coverage support
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)
if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(STATUS "Enabling code coverage with Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        message(STATUS "Enabling code coverage with GCC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    else()
        message(WARNING "Code coverage not supported for compiler: ${CMAKE_CXX_COMPILER_ID}")
        set(ENABLE_COVERAGE OFF)
    endif()
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Platform-specific settings
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "Minimum macOS version")
endif()

# Find OpenSSL with fallback paths for macOS Homebrew
if(APPLE)
    # Try common Homebrew paths first
    set(OPENSSL_PATHS
        "/opt/homebrew/opt/openssl@3"
        "/opt/homebrew/opt/openssl"
        "/usr/local/opt/openssl@3"
        "/usr/local/opt/openssl"
    )
    
    foreach(PATH ${OPENSSL_PATHS})
        if(EXISTS "${PATH}/include/openssl/ssl.h")
            set(OPENSSL_ROOT_DIR ${PATH})
            break()
        endif()
    endforeach()
endif()

# Find OpenSSL
find_package(OpenSSL REQUIRED)

if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found. Install via: brew install openssl")
endif()

# Global include directories (for compatibility)
if(OpenSSL_INCLUDE_DIRS)
    include_directories(${OpenSSL_INCLUDE_DIRS})
elseif(OPENSSL_INCLUDE_DIR)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif()
include_directories(${CMAKE_SOURCE_DIR}/src/include)

# Enable testing
enable_testing()

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(examples)

# Summary
message(STATUS "")
message(STATUS "=== Configuration Summary ===")
message(STATUS "Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Generator: ${CMAKE_GENERATOR}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
if(OpenSSL_INCLUDE_DIRS)
    message(STATUS "OpenSSL: ${OpenSSL_INCLUDE_DIRS}")
elseif(OPENSSL_INCLUDE_DIR)
    message(STATUS "OpenSSL: ${OPENSSL_INCLUDE_DIR}")
else()
    message(STATUS "OpenSSL: found")
endif()
if(CMAKE_BUILD_PARALLEL_LEVEL)
    message(STATUS "Parallel jobs: ${CMAKE_BUILD_PARALLEL_LEVEL}")
else()
    message(STATUS "Parallel jobs: auto")
endif()
if(ENABLE_COVERAGE)
    message(STATUS "Coverage: enabled")
else()
    message(STATUS "Coverage: disabled")
endif()
message(STATUS "=============================")
message(STATUS "")
