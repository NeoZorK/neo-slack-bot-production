# Project File Index

This document provides a comprehensive index of all files in the Neo Slack Bot project, organized by category.

## Source Files
Main Entry Point : src/main.cpp
Main Header : src/main.hpp
Main Initialization : src/main_init.cpp
Example Runtime : src/example_runtime.cpp
WebSocket Client : src/websocket_client.cpp
WebSocket Client Header : src/websocket_client.hpp
WebSocket Client Connection : src/websocket_client_connection.cpp
WebSocket Client Messaging : src/websocket_client_messaging.cpp
WebSocket Connection : src/websocket_connection.cpp
WebSocket Connection Header : src/websocket_connection.hpp
WebSocket Frame : src/websocket_frame.cpp
WebSocket Frame Header : src/websocket_frame.hpp
macOS Notifications : src/macos_notifications.cpp
macOS Notifications Header : src/macos_notifications.hpp
Notifications Factory : src/notifications_factory.cpp
Notifications Factory Header : src/notifications_factory.hpp
System Metrics Factory : src/system_metrics_factory.cpp
System Metrics Factory Header : src/system_metrics_factory.hpp
macOS Notifications Implementation : src/platform/macos/macos_notifications_impl.cpp
macOS Notifications Implementation Header : src/platform/macos/macos_notifications_impl.hpp
macOS System Metrics : src/platform/macos/macos_system_metrics.cpp
macOS System Metrics Header : src/platform/macos/macos_system_metrics.hpp
macOS Keychain Implementation : src/platform/macos/macos_keychain_impl.cpp
macOS Keychain Implementation Header : src/platform/macos/macos_keychain_impl.hpp
Linux Notifications Implementation : src/platform/linux/linux_notifications_impl.cpp
Linux Notifications Implementation Header : src/platform/linux/linux_notifications_impl.hpp
Linux System Metrics : src/platform/linux/linux_system_metrics.cpp
Linux System Metrics Header : src/platform/linux/linux_system_metrics.hpp
Windows Notifications Implementation : src/platform/windows/windows_notifications_impl.cpp
Windows Notifications Implementation Header : src/platform/windows/windows_notifications_impl.hpp
Windows System Metrics : src/platform/windows/windows_system_metrics.cpp
Windows System Metrics Header : src/platform/windows/windows_system_metrics.hpp
HTTP Client : src/http_client.cpp
HTTP Client Header : src/http_client.hpp
Slack API : src/slack_api.cpp
Slack API Header : src/slack_api.hpp
Slack API Core : src/slack_api_core.cpp
Event Parser : src/event_parser.cpp
Event Parser Header : src/event_parser.hpp
Settings : src/settings.cpp
Settings Header : src/settings.hpp
Config Loader : src/config_loader.cpp
Config Loader Defaults : src/config_loader_defaults.cpp
Config Loader Header : src/config_loader.hpp
JSON Parser : src/json_parser.cpp
JSON Parser Header : src/json_parser.hpp
Message Filter : src/message_filter.cpp
Message Filter Header : src/message_filter.hpp
Command Handler : src/command_handler.cpp
Command Handler Header : src/command_handler.hpp
Logger : src/logger.cpp
Logger Header : src/logger.hpp
Error Recovery : src/error_recovery.cpp
Error Recovery Header : src/error_recovery.hpp
Rate Limiter : src/rate_limiter.cpp
Rate Limiter Header : src/rate_limiter.hpp
Input Validator : src/input_validator.cpp
Input Validator Header : src/input_validator.hpp
Metrics : src/metrics.cpp
Metrics Header : src/metrics.hpp
Metrics Summary : src/metrics_summary.cpp
Root CMake Configuration : CMakeLists.txt
Examples CMake Configuration : examples/CMakeLists.txt
Source CMake Configuration : src/CMakeLists.txt
Tests CMake Configuration : tests/CMakeLists.txt
MinGW CMake Configuration : cmake/mingw-w64-x86_64.cmake
HTTP Library Header : src/include/httplib.h

## Examples
Examples README : examples/README.md
Basic Bot Example : examples/basic_bot.cpp
Custom Handler Example : examples/custom_handler.cpp
HTTP API Example : examples/http_api_example.cpp
Channel Management Example : examples/channel_management.cpp
Add Bot To Channel Example : examples/add_bot_to_channel.cpp
Two Bots Keychain Example : examples/two_bots_keychain.cpp
Bot Network Module Main : examples/bot_network_module/main.cpp
Bot Network Core : examples/bot_network_module/bot_network.cpp
Bot Network Header : examples/bot_network_module/bot_network.hpp
Bot Network Workers : examples/bot_network_module/bot_network_workers.cpp
Bot Network Rate Limiter : examples/bot_network_module/bot_network_rate_limiter.cpp
Bot Network Rate Limiter Header : examples/bot_network_module/bot_network_rate_limiter.hpp
Bot Network System Utils : examples/bot_network_module/system_utils.cpp
Bot Network System Utils Header : examples/bot_network_module/system_utils.hpp

## Interfaces
HTTP Client Interface : src/interfaces/i_http_client.hpp
WebSocket Client Interface : src/interfaces/i_websocket_client.hpp
Notification Service Interface : src/interfaces/i_notification_service.hpp
System Metrics Interface : src/interfaces/i_system_metrics.hpp

## Documentation
Project Index : docs/INDEX.md
Project README : README.md
Codex Agent Instructions : AGENTS.md
Changelog : CHANGELOG.md
Contributing Guide : CONTRIBUTING.md
License : LICENSE
Claude Instructions : CLAUDE.md
Quick Start Guide : docs/quick-start.md
Quick Start Setup : docs/quick-start-setup.md
Quick Start Usage : docs/quick-start-usage.md
Installation Guide : docs/installation.md
Installation Setup : docs/installation-setup.md
Installation Build : docs/installation-build.md
Development Setup : docs/development-setup.md
User Guide : docs/user-guide.md
User Guide Quick Start : docs/user-guide-quick-start.md
User Guide Testing : docs/user-guide-testing.md
User Guide Configuration : docs/user-guide-configuration.md
User Guide Advanced Usage : docs/user-guide-advanced.md
Manual Testing Guide : docs/manual-testing-guide.md
Manual Testing Guide Setup : docs/manual-testing-guide-setup.md
Manual Testing Guide Basic Scenarios : docs/manual-testing-guide-scenarios-basic.md
Manual Testing Guide Advanced Scenarios : docs/manual-testing-guide-scenarios-advanced.md
Manual Testing Guide Advanced : docs/manual-testing-guide-advanced.md
Testing Guide : docs/testing-guide.md
Testing Reference : docs/testing-reference.md
Testing Reference Categories : docs/testing-reference-categories.md
Testing Reference Execution : docs/testing-reference-execution.md
Security Testing Guide : docs/security-testing.md
Security Audit : docs/security-audit.md
Authentication Basic : docs/auth-basic.md
Authentication Advanced : docs/auth-advanced.md
Getting Slack Token Quick : docs/getting-slack-token-quick.md
Getting Slack Token Detailed : docs/getting-slack-token-detailed.md
Getting Slack Token Detailed Setup : docs/getting-slack-token-detailed-setup.md
Getting Slack Token Detailed Usage : docs/getting-slack-token-detailed-usage.md
Slack API Documentation : docs/slack-api.md
Slack API Overview : docs/slack-api-overview.md
Slack API Socket Mode : docs/slack-api-socket-mode.md
Slack API HTTP Methods : docs/slack-api-http.md
Slack API Authentication : docs/slack-api-authentication.md
Slack API Event Processing : docs/slack-api-events.md
Slack API Implementation : docs/slack-api-implementation.md
Features Overview : docs/features-overview.md
Features Detailed : docs/features-detailed.md
Features Core Implementation : docs/features-core.md
Features Technical Details : docs/features-technical.md
Features Platform Details : docs/features-platform.md
Tech Stack : docs/tech-stack.md
Project Structure Overview : docs/project-structure-overview.md
Project Structure Detailed : docs/project-structure-detailed.md
Project Structure Modules : docs/project-structure-modules.md
Project Structure Build : docs/project-structure-build.md
Project Structure Dependencies : docs/project-structure-dependencies.md
External Libraries Overview : docs/external-libs-overview.md
External Libraries Detailed : docs/external-libs-detailed.md
Docker Guide : docs/docker.md
Docker Setup : docs/docker-setup.md
Docker Usage : docs/docker-usage.md
User Settings Basic : docs/user-settings-basic.md
User Settings Advanced : docs/user-settings-advanced.md
User Settings Advanced Config : docs/user-settings-advanced-config.md
User Settings Management : docs/user-settings-management.md
User Settings Patterns : docs/user-settings-patterns.md
Codebase Review Report : docs/codebase-review.md
Codebase Review Architecture : docs/codebase-review-architecture.md
Codebase Review Testing : docs/codebase-review-testing.md
API Documentation : docs/api-documentation.md
Performance : docs/performance.md
Roadmap : docs/roadmap.md
Rules : docs/rules.md
Project Idea : docs/project-idea.md
Pre-Production Summary : docs/pre-production-summary.md
Production Testing Setup : docs/production-testing-setup.md
Production Testing Checklist : docs/production-testing-checklist.md
Production Checklist macOS : docs/production-checklist-macos.md
Production Checklist Linux : docs/production-checklist-linux.md
Production Checklist Windows : docs/production-checklist-windows.md
Audit Report Code Quality : docs/audit-report-code-quality.md
Audit Report Executive : docs/audit-report-executive.md
Audit Report Security Testing : docs/audit-report-security-testing.md
Troubleshooting Bot Setup : docs/troubleshooting-bot-setup.md
Linux Platform Support : docs/linux-support.md
Windows Platform Support : docs/windows-support.md
Code Coverage Guide : docs/coverage.md
Plugin System Architecture Overview : docs/plugin-system-architecture-overview.md
Plugin System Architecture Detailed : docs/plugin-system-architecture-detailed.md
GUI Tool Architecture Overview : docs/gui-tool-architecture-overview.md
GUI Tool Architecture Detailed : docs/gui-tool-architecture-detailed.md
Examples Guide : docs/examples.md
Examples Building : docs/examples-building.md
Examples Descriptions : docs/examples-descriptions.md
v0.0.7 Plan : docs/v0.0.7-plan.md

## Tests
Test Suite README : tests/README.md
Test Main : tests/test_main.cpp
Unit Tests Directory : tests/unit/
Integration Tests Directory : tests/integration/
Security Tests Directory : tests/security/
Performance Tests Directory : tests/performance/
Stress Tests Directory : tests/stress/
Benchmark Tests Directory : tests/benchmark/
E2E Tests Directory : tests/e2e/
WebSocket Tests : tests/unit/test_websocket.cpp
macOS Notifications Tests : tests/unit/test_macos_notifications.cpp
Linux Notifications Tests : tests/unit/test_linux_notifications.cpp
HTTP Client Tests : tests/unit/test_http_client.cpp
Slack API Tests : tests/unit/test_slack_api.cpp
Event Parser Tests : tests/unit/test_event_parser.cpp
Settings Tests : tests/unit/test_settings.cpp
JSON Parser Tests : tests/unit/test_json_parser.cpp
Config Loader Tests : tests/unit/test_config_loader.cpp
Message Filter Extended Tests : tests/unit/test_message_filter_extended.cpp
Logger Tests : tests/unit/test_logger.cpp
Error Recovery Tests : tests/unit/test_error_recovery.cpp
Rate Limiter Tests : tests/unit/test_rate_limiter.cpp
Input Validator Tests : tests/unit/test_input_validator.cpp
Validator Limiter Boundary Tests : tests/unit/test_validator_limiter_boundaries.cpp
Metrics Tests : tests/unit/test_metrics.cpp
Command Handler Tests : tests/unit/test_command_handler.cpp
Integration Tests : tests/integration/test_integration.cpp
Slack Integration Tests : tests/integration/test_slack_integration.cpp
Security Tests : tests/security/test_security.cpp
Security Input Validation Tests : tests/security/test_security_input.cpp
Security Input Attacks Tests : tests/security/test_security_input_attacks.cpp
Security Input Format Tests : tests/security/test_security_input_format.cpp
Security Input Sanitization Tests : tests/security/test_security_input_sanitization.cpp
Performance Tests : tests/performance/test_performance.cpp
Performance Latency Tests : tests/performance/test_performance_latency.cpp
Performance Throughput Tests : tests/performance/test_performance_throughput.cpp
Performance Advanced Tests : tests/performance/test_performance_advanced.cpp
Stress Tests : tests/stress/test_stress.cpp
Stress Connection Tests : tests/stress/test_stress_connection.cpp
Stress Components Tests : tests/stress/test_stress_components.cpp
Stress System Tests : tests/stress/test_stress_system.cpp
Stress Concurrent Tests : tests/stress/test_stress_concurrent.cpp
Stress Advanced Tests : tests/stress/test_stress_advanced.cpp
Benchmark Tests : tests/benchmark/test_benchmark.cpp
Slack End-to-End Tests : tests/e2e/test_slack_e2e.cpp
Slack E2E API Tests : tests/e2e/test_slack_e2e_api.cpp
Slack E2E Components Tests : tests/e2e/test_slack_e2e_components.cpp
Slack E2E Integration Tests : tests/e2e/test_slack_e2e_integration.cpp
Example Tests Directory : tests/examples/
Bot Network Example Tests : tests/examples/test_bot_network.cpp
Channel Management Example Tests : tests/examples/test_channel_management.cpp
Bot Network Rate Limiter Tests : tests/unit/test_bot_network_rate_limiter.cpp
Mock HTTP Client : tests/mocks/mock_http_client.hpp
Mock WebSocket Client : tests/mocks/mock_websocket_client.hpp

## Scripts
Install Launchd Service : scripts/install-launchd.sh
Uninstall Launchd Service : scripts/uninstall-launchd.sh
Format Script : scripts/format.sh
Lint Script : scripts/lint.sh
File Size Check : scripts/check-file-sizes.sh
Coverage Script : scripts/coverage.sh

## Docker
Dockerfile : Dockerfile
Docker Compose : docker-compose.yml

## Documentation Generation
Doxygen Config : Doxyfile
