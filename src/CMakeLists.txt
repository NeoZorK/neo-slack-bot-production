# Source files for library
set(LIB_SOURCES
    main_init.cpp
    websocket_client.cpp
    websocket_client_connection.cpp
    websocket_client_messaging.cpp
    websocket_connection.cpp
    websocket_frame.cpp
    http_client.cpp
    slack_api.cpp
    slack_api_core.cpp
    event_parser.cpp
    message_filter.cpp
    command_handler.cpp
    settings.cpp
    config_loader.cpp
    config_loader_defaults.cpp
    json_parser.cpp
    logger.cpp
    error_recovery.cpp
    rate_limiter.cpp
    input_validator.cpp
    metrics.cpp
    metrics_summary.cpp
    notifications_factory.cpp
    system_metrics_factory.cpp
)

# Platform-specific sources
if(APPLE)
    list(APPEND LIB_SOURCES
        macos_notifications.cpp
        platform/macos/macos_notifications_impl.cpp
        platform/macos/macos_system_metrics.cpp
        platform/macos/macos_keychain_impl.cpp
    )
elseif(UNIX AND NOT APPLE)
    list(APPEND LIB_SOURCES
        platform/linux/linux_notifications_impl.cpp
        platform/linux/linux_system_metrics.cpp
    )
    # Find libnotify for Linux
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBNOTIFY REQUIRED libnotify)
elseif(WIN32)
    list(APPEND LIB_SOURCES
        platform/windows/windows_notifications_impl.cpp
        platform/windows/windows_system_metrics.cpp
    )
    # TODO: Future Windows-specific libraries:
    # - windowsapp.lib (for WinRT Toast Notifications)
    # - psapi.lib (for system metrics via GetProcessMemoryInfo)
    # - winmm.lib (optional, for sound support)
endif()

# Create static library for main app
add_library(neo_slack_bot_lib STATIC ${LIB_SOURCES})

# Create static library for examples and tests
# Add example_runtime.cpp to provide g_running for examples
add_library(neo_slack_bot_examples_lib STATIC ${LIB_SOURCES} example_runtime.cpp)
target_compile_definitions(neo_slack_bot_examples_lib PUBLIC NEO_SLACK_EXAMPLES)

# Create executable
add_executable(neo-slack-bot main.cpp)

# Compiler definitions
target_compile_definitions(neo_slack_bot_lib PUBLIC
    CPPHTTPLIB_OPENSSL_SUPPORT
)
target_compile_definitions(neo_slack_bot_examples_lib PUBLIC
    CPPHTTPLIB_OPENSSL_SUPPORT
)

# Include directories
foreach(TARGET neo_slack_bot_lib neo_slack_bot_examples_lib)
    target_include_directories(${TARGET} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/macos
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/linux
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/windows
    )
    
    if(OpenSSL_INCLUDE_DIRS)
        target_include_directories(${TARGET} PUBLIC ${OpenSSL_INCLUDE_DIRS})
    elseif(OPENSSL_INCLUDE_DIR)
        target_include_directories(${TARGET} PUBLIC ${OPENSSL_INCLUDE_DIR})
    endif()

    # Link libraries
    if(TARGET OpenSSL::SSL AND TARGET OpenSSL::Crypto)
        if(WIN32)
            target_link_libraries(${TARGET} PUBLIC OpenSSL::SSL OpenSSL::Crypto)
        else()
            target_link_libraries(${TARGET} PUBLIC OpenSSL::SSL OpenSSL::Crypto pthread)
        endif()
    else()
        if(WIN32)
            target_link_libraries(${TARGET} PUBLIC ${OpenSSL_LIBRARIES})
        else()
            target_link_libraries(${TARGET} PUBLIC ${OpenSSL_LIBRARIES} pthread)
        endif()
    endif()

    # Platform-specific libraries
    if(UNIX AND NOT APPLE)
        target_link_libraries(${TARGET} PUBLIC ${LIBNOTIFY_LIBRARIES})
        target_include_directories(${TARGET} PUBLIC ${LIBNOTIFY_INCLUDE_DIRS})
    elseif(WIN32)
        target_link_libraries(${TARGET} PUBLIC ws2_32 psapi iphlpapi crypt32)
    endif()

    # Add library search paths for OpenSSL
    if(OPENSSL_ROOT_DIR)
        target_link_directories(${TARGET} PUBLIC ${OPENSSL_ROOT_DIR}/lib)
    elseif(OpenSSL_LIBRARIES)
        get_filename_component(OPENSSL_LIB_DIR ${OpenSSL_LIBRARIES} DIRECTORY)
        target_link_directories(${TARGET} PUBLIC ${OPENSSL_LIB_DIR})
    endif()

    # Platform-specific linking
    if(APPLE)
        target_link_libraries(${TARGET} PUBLIC
            "-framework Foundation"
            "-framework AppKit"
            "-framework Security"
        )
    endif()
endforeach()

# Link executable with library
target_link_libraries(neo-slack-bot PRIVATE neo_slack_bot_lib)

# Set output directory
set_target_properties(neo-slack-bot PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation
install(TARGETS neo-slack-bot
    RUNTIME DESTINATION bin
)
