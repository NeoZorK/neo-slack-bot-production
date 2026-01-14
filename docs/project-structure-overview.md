# Project Structure Overview

## Directory Layout

```
neo-slack-bot/
├── docs/                    # Documentation directory
├── src/                     # Source code directory
├── tests/                   # Test directory
├── examples/                # Example code
├── scripts/                 # Utility scripts
├── .gitignore              # Git ignore rules
├── CLAUDE.md               # Claude AI assistant instructions
└── README.md               # Main project readme
```

## Directory Purposes

### `docs/` - Documentation

**Purpose**: All project documentation files

**Contents**:
- Project documentation and guides
- Architecture and design documents
- User guides and tutorials
- Development guidelines

**Rules**:
- Only `.md` files (Markdown)
- No source code files
- No binary files
- All files registered in `INDEX.md`
- Maximum 300 lines per file

### `src/` - Source Code

**Purpose**: C++ source code and headers

**Contents**:
- Application source files (`.cpp`)
- Header files (`.hpp`)
- Build configuration (`Makefile` or `CMakeLists.txt`)
- Generated build artifacts (`obj/`, `bin/`)

**Rules**:
- Only C++ source code
- Headers and implementations separated
- Max 300 lines per file
- All files registered in `docs/INDEX.md`

**Structure**:
- One class per file (or related small classes)
- Headers contain declarations
- Implementations in `.cpp` files
- Build artifacts excluded from version control

### `tests/` - Test Files

**Purpose**: Unit tests and test utilities

**Contents**:
- Test source files
- Test fixtures and mocks
- Test configuration
- Test executables (generated)

**Rules**:
- Mirror `src/` structure
- One test file per source file
- Tests for all public APIs
- 100% coverage target

### `examples/` - Example Code

**Purpose**: Example implementations and usage

**Contents**:
- Example bot implementations
- Usage examples
- Integration examples

### `scripts/` - Utility Scripts

**Purpose**: Build and utility scripts

**Contents**:
- Format scripts
- Lint scripts
- Coverage scripts
- Installation scripts

## File Organization Principles

### Naming Conventions

**Source Files**:
- `snake_case.cpp` for implementations
- `snake_case.hpp` for headers
- Descriptive, clear names
- Match class/component names

**Test Files**:
- `test_<module>.cpp` pattern
- Mirror source file names
- Clear test purpose in name

**Documentation Files**:
- `kebab-case.md` for readability
- Descriptive names
- Consistent naming

### File Size Limits

**Strict Rule**: Maximum 300 lines per file

**Enforcement**:
- Includes all lines (code, comments, blank lines)
- Applies to `.cpp`, `.hpp`, `.md`, test files
- Split files if limit exceeded
- Extract helper functions/classes

**Splitting Strategy**:
- Extract related functionality to separate files
- Create helper modules
- Use forward declarations
- Organize into logical groups

## Module Structure

### Core Modules

**WebSocket Client Module**:
- WebSocket connection management
- Message sending/receiving
- Connection state handling
- Reconnection logic

**macOS Notifications Module**:
- Native notification display
- Notification configuration
- System integration

**HTTP Client Module**:
- HTTP request/response handling
- SSL/TLS support
- Header management

**Slack API Module**:
- Slack API wrapper
- Request construction
- Response parsing

**Main Application**:
- Application initialization
- Component orchestration
- Event loop coordination

## File Registration

### INDEX.md Format

All project files must be registered in `docs/INDEX.md`:

```
Purpose : path/to/file
```

**Example**:
```
WebSocket Client : src/websocket_client.cpp
WebSocket Client Header : src/websocket_client.hpp
```

**When to Update**:
- Adding new files
- Removing files
- Renaming files
- Moving files

## Best Practices

1. **Keep files focused**: Single responsibility per file
2. **Maintain structure**: Follow established patterns
3. **Document changes**: Update INDEX.md immediately
4. **Respect limits**: Keep files under 300 lines
5. **Organize logically**: Group related functionality
6. **Separate concerns**: Headers vs. implementations
7. **Test structure**: Mirror source structure in tests

## See Also

- [Project Structure Detailed](project-structure-detailed.md) - Detailed module structure and dependencies
