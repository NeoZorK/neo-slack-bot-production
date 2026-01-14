# Docker Guide

## Overview

This guide explains how to use Docker with Neo Slack Bot for testing, development, and CI/CD purposes. The guide is organized into multiple documents for better navigation.

**⚠️ Important**: Docker builds are **limited** due to macOS-specific dependencies. The bot requires macOS frameworks (Foundation, AppKit) and macOS notification APIs that are not available in Linux containers. Docker is suitable for testing core functionality (WebSocket, HTTP API, JSON parsing) but **not** for full feature testing or production deployment.

## Documentation Structure

The Docker guide is organized into the following documents:

- **[Docker Guide - Overview and Setup](docker-setup.md)** - Overview, prerequisites, Docker setup, limitations

- **[Docker Guide - Usage and Examples](docker-usage.md)** - Building Docker image, running containers, testing, troubleshooting

## Quick Reference

- **Setup**: See [Docker Guide - Overview and Setup](docker-setup.md)
- **Usage**: See [Docker Guide - Usage and Examples](docker-usage.md)

## See Also

- [Development Setup](development-setup.md) - Development environment setup
- [Testing Guide](testing-guide.md) - Testing guide
