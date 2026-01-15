# Contributing to Neo Slack Bot

Thank you for your interest in contributing to Neo Slack Bot! This document provides guidelines and instructions for contributing.

## Code of Conduct

- Be respectful and inclusive
- Focus on constructive feedback
- Follow project coding standards
- Maintain professional communication

## Getting Started

1. **Fork the repository**
2. **Clone your fork**: `git clone https://github.com/YOUR_USERNAME/neo-slack-bot.git`
3. **Create a branch**: `git checkout -b feature/your-feature-name`
4. **Read documentation**: Check `docs/rules.md` and `CLAUDE.md`

## Development Workflow

### Before You Start

- Read `docs/rules.md` for coding standards
- Check `docs/INDEX.md` for file registry
- Review `docs/roadmap.md` for planned features
- Ensure you understand the 300-line file limit

### Making Changes

1. **Follow Coding Standards**
   - C++17 standard
   - snake_case naming
   - Max 300 lines per file
   - Smart pointers (no raw pointers)
   - RAII for resource management

2. **Write Tests**
   - 100% test coverage required
   - Tests in `tests/` directory
   - Test success and failure paths
   - Use descriptive test names

3. **Update Documentation**
   - Register new files in `docs/INDEX.md`
   - Update relevant documentation
   - Add code comments for public APIs
   - Keep docs under 300 lines

4. **Run Checks**
   ```bash
   # Format code
   clang-format -i src/**/*.{cpp,hpp}
   
   # Lint code
   clang-tidy src/**/*.{cpp,hpp}
   
   # Build
   cd src && make
   
   # Test
   cd ../tests && make test
   ```

## Pull Request Process

### Before Submitting

- [ ] Code follows style guide (`docs/rules.md`)
- [ ] All tests pass locally
- [ ] New code has tests (100% coverage)
- [ ] Files are under 300 lines
- [ ] All files registered in `docs/INDEX.md`
- [ ] Documentation updated
- [ ] No memory leaks or resource leaks
- [ ] Code formatted with clang-format
- [ ] No clang-tidy warnings

**Note**: CI runs automatically only for releases (when a release is created or a version tag is pushed). For pull requests, run tests locally before submitting.

### PR Description

Include:
- **What**: Description of changes
- **Why**: Reason for changes
- **How**: Implementation approach
- **Testing**: How you tested
- **Breaking Changes**: If any

### Review Process

1. PR will be reviewed by maintainers
2. Address feedback promptly
3. Keep PR focused (one feature/fix per PR)
4. Update PR based on comments

## Coding Standards

### File Organization

- Headers (`.hpp`) contain declarations
- Implementations (`.cpp`) contain definitions
- One class per file (or related small classes)
- Max 300 lines per file (strict limit)

### Naming Conventions

- **Variables/Functions**: `snake_case`
- **Classes**: `snake_case` (project convention)
- **Constants**: `UPPER_CASE`
- **Files**: `snake_case.cpp`, `snake_case.hpp`

### Code Style

- Use `const` correctness
- Prefer `constexpr` for compile-time constants
- Use smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- Avoid raw pointers
- Use RAII for resource management
- Handle errors appropriately

### Documentation

- Document public APIs
- Use clear, descriptive names
- Comment complex logic
- Keep comments up-to-date
- English for code and comments

## Testing Requirements

### Coverage

- **Target**: 100% code coverage
- All public APIs must have tests
- Test success and failure paths
- Test edge cases

### Test Structure

- Tests in `tests/` directory
- One test file per source file
- Descriptive test names: `test_component_scenario_result`
- Isolated, independent tests

### Running Tests

```bash
cd tests
make test
```

## Commit Messages

### Format

```
<type>: <subject>

<body>

<footer>
```

### Types

- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting)
- `refactor`: Code refactoring
- `test`: Test additions/changes
- `chore`: Maintenance tasks

### Examples

```
feat: add WebSocket reconnection logic

Implement automatic reconnection with exponential backoff.
Handle connection failures gracefully.

Closes #123
```

```
fix: memory leak in notification handler

Free notification resources properly on error paths.
Add RAII wrapper for notification objects.

Fixes #456
```

## File Registration

**CRITICAL**: All new files must be registered in `docs/INDEX.md`

Format: `Purpose : path/to/file`

Example:
```
WebSocket Client : src/websocket_client.cpp
WebSocket Client Header : src/websocket_client.hpp
```

## Questions?

- Check `docs/` directory for documentation
- Review `CLAUDE.md` for AI assistant guidelines
- Open an issue for questions
- Contact maintainers if needed

## Thank You!

Your contributions make this project better. We appreciate your time and effort!
