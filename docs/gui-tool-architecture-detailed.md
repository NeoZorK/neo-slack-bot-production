# GUI Configuration Tool Architecture - Detailed Implementation

## Overview

This document provides detailed implementation information for the GUI configuration tool. For overview, see [GUI Tool Architecture - Overview](gui-tool-architecture-overview.md).

## Application Structure

```
gui-tool/
├── main.cpp                 # Application entry point
├── main.qml                 # Main QML file
├── models/                  # Data models
│   ├── config_model.hpp
│   ├── config_model.cpp
│   ├── bot_status_model.hpp
│   └── bot_status_model.cpp
├── views/                   # QML views
│   ├── MainWindow.qml
│   ├── SettingsView.qml
│   ├── StatusView.qml
│   └── PluginsView.qml
├── controllers/             # C++ controllers
│   ├── config_controller.hpp
│   ├── config_controller.cpp
│   ├── bot_controller.hpp
│   └── bot_controller.cpp
└── api/                     # Bot API client
    ├── bot_api_client.hpp
    └── bot_api_client.cpp
```

## Communication Architecture

### REST API Server

Bot exposes HTTP REST API for configuration:

```cpp
/**
 * @brief REST API server for GUI tool communication
 * 
 * Thread Safety: Thread-safe. Handles concurrent requests.
 */
class ConfigAPIServer {
public:
    /**
     * @brief Start API server
     * @param port Port to listen on (default: 8080)
     * @return true if started successfully
     */
    bool start(int port = 8080);
    
    /**
     * @brief Stop API server
     */
    void stop();
    
    /**
     * @brief Register API endpoints
     */
    void register_endpoints();
};
```

### API Endpoints

```
GET  /api/config              # Get current configuration
PUT  /api/config              # Update configuration
GET  /api/status              # Get bot status
POST /api/restart             # Restart bot
GET  /api/plugins             # List plugins
POST /api/plugins/{id}/enable # Enable plugin
POST /api/plugins/{id}/disable # Disable plugin
GET  /api/metrics             # Get metrics
GET  /api/logs                # Get logs
```

## Data Models

### ConfigModel

```cpp
/**
 * @brief Configuration data model
 */
class ConfigModel : public QAbstractListModel {
    Q_OBJECT
    
public:
    enum ConfigRole {
        KeyRole = Qt::UserRole + 1,
        ValueRole,
        TypeRole,
        DescriptionRole
    };
    
    Q_INVOKABLE QVariant getValue(const QString& key) const;
    Q_INVOKABLE void setValue(const QString& key, const QVariant& value);
    Q_INVOKABLE bool validateConfig() const;
    
signals:
    void configChanged();
    void validationError(const QString& message);
};
```

### BotStatusModel

```cpp
/**
 * @brief Bot status data model
 */
class BotStatusModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(int uptime READ uptime NOTIFY uptimeChanged)
    
public:
    QString status() const { return status_; }
    QString version() const { return "0.0.7"; }
    int uptime() const { return uptime_; }
    
    Q_INVOKABLE void refresh();
    
signals:
    void statusChanged();
    void uptimeChanged();
};
```

## Controllers

### ConfigController

QObject controller managing configuration operations. Methods: `loadConfig()`, `saveConfig()`, `resetToDefaults()`, `validateAndSave()`. Emits `configLoaded()`, `configSaved()`, `error()` signals.

## Bot API Client

QObject HTTP client for bot API communication. Methods: `getConfig()`, `updateConfig()`, `getStatus()`, `restartBot()`. Uses QNetworkAccessManager for HTTP requests. Emits `connected()`, `disconnected()`, `error()` signals.

## Configuration Validation

```cpp
/**
 * @brief Configuration validator
 */
class ConfigValidator {
public:
    struct ValidationResult {
        bool valid;
        QStringList errors;
        QStringList warnings;
    };
    
    static ValidationResult validate(const QJsonObject& config);
    static bool validateNotificationSettings(const QJsonObject& settings);
    static bool validateConnectionSettings(const QJsonObject& settings);
    static bool validateEventSettings(const QJsonObject& settings);
};
```

## Hot Reload Support

**Configuration Hot Reload**:
- Bot watches configuration file
- Changes trigger reload without restart
- Validation before applying changes
- Rollback on validation failure

**Implementation**:
```cpp
class ConfigWatcher : public QObject {
    Q_OBJECT
    
public:
    void watchConfigFile(const QString& path);
    
signals:
    void configFileChanged();
    
private slots:
    void onFileChanged(const QString& path);
};
```

## Build Integration

**CMake Configuration**:
```cmake
# GUI Tool (optional component)
option(BUILD_GUI_TOOL "Build GUI configuration tool" OFF)

if(BUILD_GUI_TOOL)
    find_package(Qt6 REQUIRED COMPONENTS Core Quick Network)
    qt6_standard_project_setup()
    
    qt6_add_executable(gui-tool
        main.cpp
        models/config_model.cpp
        controllers/config_controller.cpp
        api/bot_api_client.cpp
    )
    
    qt6_add_resources(gui-tool "qml"
        PREFIX "/"
        FILES
            views/MainWindow.qml
            views/SettingsView.qml
    )
    
    target_link_libraries(gui-tool PRIVATE
        Qt6::Core
        Qt6::Quick
        Qt6::Network
    )
endif()
```

## API Specification

**GET /api/config**: Returns current configuration JSON (notifications, connection, events settings)

**PUT /api/config**: Updates configuration. Request: same as GET response. Response: `{"success": true}` or error

**GET /api/status**: Returns bot status (connection state, version, uptime, memory, CPU usage)

See [GUI Tool Architecture - Overview](gui-tool-architecture-overview.md) for detailed API examples.

## See Also

- [GUI Tool Architecture - Overview](gui-tool-architecture-overview.md) - Overview and concepts
- [Configuration Guide](user-settings-basic.md) - Configuration documentation
- [Plugin System Architecture](plugin-system-architecture-overview.md) - Plugin system design
