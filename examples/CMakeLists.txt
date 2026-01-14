# Examples

# Single file examples
set(EXAMPLE_SOURCES
    basic_bot.cpp
    custom_handler.cpp
    http_api_example.cpp
    channel_management.cpp
    add_bot_to_channel.cpp
)

if(APPLE)
    list(APPEND EXAMPLE_SOURCES two_bots_keychain.cpp)
endif()

foreach(SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(EXE_NAME ${SOURCE} NAME_WE)
    add_executable(${EXE_NAME} ${SOURCE})
    target_link_libraries(${EXE_NAME} PRIVATE neo_slack_bot_examples_lib)
    target_compile_definitions(${EXE_NAME} PRIVATE NEO_SLACK_EXAMPLES CPPHTTPLIB_OPENSSL_SUPPORT)
    set_target_properties(${EXE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endforeach()

# Special case for bot_network which is refactored into module
add_executable(bot_network 
    bot_network_module/main.cpp
    bot_network_module/bot_network.cpp
    bot_network_module/bot_network_workers.cpp
    bot_network_module/bot_network_rate_limiter.cpp
    bot_network_module/system_utils.cpp
)
target_link_libraries(bot_network PRIVATE neo_slack_bot_examples_lib)
target_compile_definitions(bot_network PRIVATE NEO_SLACK_EXAMPLES CPPHTTPLIB_OPENSSL_SUPPORT)
set_target_properties(bot_network PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_custom_target(run_bot_network
    COMMAND bot_network
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
