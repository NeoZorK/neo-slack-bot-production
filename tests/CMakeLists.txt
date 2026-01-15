# Enable testing
enable_testing()

# Test source files
file(GLOB_RECURSE UNIT_SOURCES "unit/*.cpp")
file(GLOB_RECURSE INTEGRATION_SOURCES "integration/*.cpp")
file(GLOB_RECURSE SECURITY_SOURCES "security/*.cpp")
file(GLOB_RECURSE PERFORMANCE_SOURCES "performance/*.cpp")
file(GLOB_RECURSE STRESS_SOURCES "stress/*.cpp")
file(GLOB_RECURSE BENCHMARK_SOURCES "benchmark/*.cpp")
file(GLOB_RECURSE E2E_SOURCES "e2e/*.cpp")

# Exclude Linux-only tests on non-Linux platforms
if(NOT (UNIX AND NOT APPLE))
    list(FILTER UNIT_SOURCES EXCLUDE REGEX ".*test_linux_notifications\\.cpp$")
endif()

# Exclude macOS-only tests on non-Apple platforms
if(NOT APPLE)
    list(FILTER UNIT_SOURCES EXCLUDE REGEX ".*test_macos_notifications\\.cpp$")
endif()

set(TEST_SOURCES
    test_main.cpp
    ${UNIT_SOURCES}
    ${INTEGRATION_SOURCES}
    ${SECURITY_SOURCES}
    ${PERFORMANCE_SOURCES}
    ${STRESS_SOURCES}
    ${BENCHMARK_SOURCES}
    ${E2E_SOURCES}
    ${CMAKE_SOURCE_DIR}/examples/bot_network_module/bot_network_rate_limiter.cpp
)

# Create test executable
add_executable(test_neo_slack_bot ${TEST_SOURCES})

# Link with library
target_link_libraries(test_neo_slack_bot PRIVATE
    neo_slack_bot_lib
)

# Add library search paths for OpenSSL
if(OPENSSL_ROOT_DIR)
    target_link_directories(test_neo_slack_bot PRIVATE ${OPENSSL_ROOT_DIR}/lib)
elseif(OpenSSL_LIBRARIES)
    get_filename_component(OPENSSL_LIB_DIR ${OpenSSL_LIBRARIES} DIRECTORY)
    target_link_directories(test_neo_slack_bot PRIVATE ${OPENSSL_LIB_DIR})
endif()

# Include directories
target_include_directories(test_neo_slack_bot PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/include
    ${CMAKE_SOURCE_DIR}/src/interfaces
    ${CMAKE_SOURCE_DIR}/src/platform/macos
    ${CMAKE_SOURCE_DIR}/src/platform/linux
    ${CMAKE_SOURCE_DIR}/src/platform/windows
)

if(OpenSSL_INCLUDE_DIRS)
    target_include_directories(test_neo_slack_bot PRIVATE ${OpenSSL_INCLUDE_DIRS})
elseif(OPENSSL_INCLUDE_DIR)
    target_include_directories(test_neo_slack_bot PRIVATE ${OPENSSL_INCLUDE_DIR})
endif()

# Compiler definitions
target_compile_definitions(test_neo_slack_bot PRIVATE
    CPPHTTPLIB_OPENSSL_SUPPORT
)

# Set output directory
set_target_properties(test_neo_slack_bot PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_test(NAME neo-slack-bot-tests COMMAND test_neo_slack_bot)

find_program(WINE_EXECUTABLE wine)
find_program(XVFB_RUN xvfb-run)

if(WINE_EXECUTABLE)
    if(XVFB_RUN)
        add_custom_target(test_windows_wine
            COMMAND ${XVFB_RUN} -a ${WINE_EXECUTABLE} ${CMAKE_BINARY_DIR}/bin/test_neo_slack_bot.exe
            COMMENT "Running Windows tests under Wine (headless)"
            VERBATIM
        )
    else()
        add_custom_target(test_windows_wine
            COMMAND ${WINE_EXECUTABLE} ${CMAKE_BINARY_DIR}/bin/test_neo_slack_bot.exe
            COMMENT "Running Windows tests under Wine"
            VERBATIM
        )
    endif()
endif()

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_tests_properties(neo-slack-bot-tests PROPERTIES
        ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/default.profraw"
    )
endif()

if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        find_program(LLVM_COV llvm-cov
            PATHS
            /opt/homebrew/opt/llvm/bin
            /usr/local/opt/llvm/bin
            /usr/bin
            /usr/local/bin
        )
        
        find_program(LLVM_PROFDATA llvm-profdata
            PATHS
            /opt/homebrew/opt/llvm/bin
            /usr/local/opt/llvm/bin
            /usr/bin
            /usr/local/bin
        )
        
        if(LLVM_COV AND LLVM_PROFDATA)
            # Custom target for coverage report
            add_custom_target(coverage
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
                COMMAND ${LLVM_PROFDATA} merge -sparse ${CMAKE_BINARY_DIR}/default.profraw -o ${CMAKE_BINARY_DIR}/default.profdata || true
                COMMAND ${LLVM_COV} show $<TARGET_FILE:test_neo_slack_bot>
                    -instr-profile=${CMAKE_BINARY_DIR}/default.profdata
                    -format=html
                    -output-dir=${CMAKE_BINARY_DIR}/coverage/
                    -ignore-filename-regex=".*test.*|.*/usr/.*|.*/opt/.*"
                COMMAND ${LLVM_COV} report $<TARGET_FILE:test_neo_slack_bot>
                    -instr-profile=${CMAKE_BINARY_DIR}/default.profdata
                    -ignore-filename-regex=".*test.*|.*/usr/.*|.*/opt/.*"
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating code coverage report"
            )
            
            message(STATUS "Coverage target available: make coverage")
        else()
            message(WARNING "llvm-cov or llvm-profdata not found. Coverage target not available.")
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        # GCC coverage with gcov
        find_program(GCOV gcov)
        find_program(LCOV lcov)
        find_program(GENHTML genhtml)
        
        if(GCOV AND LCOV AND GENHTML)
            add_custom_target(coverage
                COMMAND ${LCOV} --directory ${CMAKE_BINARY_DIR} --capture --output-file ${CMAKE_BINARY_DIR}/coverage.info
                COMMAND ${LCOV} --remove ${CMAKE_BINARY_DIR}/coverage.info '*/tests/*' '*/usr/*' '*/opt/*' --output-file ${CMAKE_BINARY_DIR}/coverage_filtered.info
                COMMAND ${GENHTML} ${CMAKE_BINARY_DIR}/coverage_filtered.info --output-directory ${CMAKE_BINARY_DIR}/coverage
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating code coverage report"
            )
            
            message(STATUS "Coverage target available: make coverage")
        else()
            message(WARNING "gcov, lcov, or genhtml not found. Coverage target not available.")
        endif()
    endif()
endif()
