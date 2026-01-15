# Project Rules

## Language Standard
- **C++17** minimum standard
- Use modern C++ features: smart pointers, lambdas, auto, range-based for
- Avoid raw pointers, prefer `std::unique_ptr` and `std::shared_ptr`
- Use `const` correctness everywhere possible
- Prefer `constexpr` for compile-time constants

## Code Style
- **snake_case** for variables, functions, namespaces
- **snake_case** for class names (consistent with project style)
- **UPPER_CASE** for constants and macros
- **PascalCase** only for external API compatibility if required
- Use meaningful, descriptive names
- Avoid abbreviations unless widely understood

## Architecture Principles
- **Separation of Concerns**: Each module has single responsibility
- **Dependency Inversion**: Depend on abstractions, not concretions
- **Interface Segregation**: Small, focused interfaces
- **DRY**: Don't Repeat Yourself - extract common functionality
- **KISS**: Keep It Simple, Stupid - prefer simple solutions
- **YAGNI**: You Aren't Gonna Need It - don't over-engineer

## Design Patterns
- Use RAII for resource management
- Prefer composition over inheritance
- Use factory pattern for object creation when needed
- Observer pattern for event handling
- Strategy pattern for algorithm selection
- Singleton only when absolutely necessary (prefer dependency injection)

## Memory Management
- Use smart pointers: `std::unique_ptr` for ownership, `std::shared_ptr` for shared ownership
- Avoid `new`/`delete` - use smart pointers or containers
- Use `std::vector`, `std::string`, `std::map` instead of C arrays
- Be aware of ownership semantics - document when transferring ownership
- Use move semantics (`std::move`) for expensive copies

## Error Handling
- Use exceptions for exceptional conditions only
- Prefer `std::optional` or error codes for expected failures
- Never ignore errors - always handle or propagate
- Use RAII to ensure cleanup on exceptions
- Document exception guarantees (basic, strong, nothrow)
- Validate inputs at boundaries (public APIs)

## Thread Safety
- Document thread safety guarantees for each class
- Use `std::mutex`, `std::lock_guard`, `std::unique_lock` for synchronization
- Prefer `std::atomic` for simple atomic operations
- Avoid data races - use synchronization primitives
- Consider lock-free data structures for high-performance paths
- Use thread-local storage (`thread_local`) when appropriate

## Performance
- Measure before optimizing - use profilers
- Prefer stack allocation over heap when possible
- Use `const` references for function parameters (avoid unnecessary copies)
- Use move semantics for large objects
- Reserve capacity for containers when size is known
- Avoid premature optimization

## Security
- Validate all external inputs (network, files, user input)
- Use secure defaults (HTTPS, secure protocols)
- Never log sensitive data (tokens, passwords)
- Use OpenSSL for cryptographic operations
- Sanitize data before processing
- Follow OWASP guidelines for security

## File Size Limit
- **STRICT LIMIT: 300 lines maximum** for ANY file
- Includes: source files (.cpp), headers (.hpp), tests, documentation (.md)
- If file exceeds limit, split into multiple files logically
- Extract helper functions to separate files
- Split large classes into smaller, focused classes

## File Organization
- One class per file (or related small classes)
- Headers (.hpp) contain declarations only
- Implementations in .cpp files
- Inline functions only when performance-critical
- Forward declarations in headers when possible

## File Registration
- All project files MUST be registered in `docs/INDEX.md`
- Format: `Purpose : path/file`
- Update INDEX.md immediately when adding new files
- Remove entries when deleting files

## Project Structure
- `src/` - Source code only (C++ files)
- `tests/` - Test files only
- `docs/` - Documentation only
- No files in project root except CLAUDE.md and README.md
- Use subdirectories when module grows beyond 5-6 files

## Documentation
- All documentation in `docs/` folder
- English language for code, comments, and technical docs
- Russian for user-facing documentation (with RU postfix)
- Document public APIs with clear comments
- Use Doxygen-style comments for complex functions
- Document preconditions, postconditions, side effects

## Code Comments
- Write self-documenting code - comments explain "why", not "what"
- Comment complex algorithms and business logic
- Document non-obvious behavior and edge cases
- Keep comments up-to-date with code changes
- Remove commented-out code (use version control)

## Testing
- **100% test coverage** required for all new code
- All new features must have corresponding tests
- Tests in `tests/` directory, mirror `src/` structure
- Use descriptive test names: `test_function_name_scenario_expected_result`
- Test both success and failure paths
- Test edge cases and boundary conditions
- Use mocks for external dependencies
- Keep tests independent and isolated

## Build System
- **ALWAYS use CMake + Ninja**
- **NEVER use Make directly** (make is deprecated and forbidden)
- Use `cmake -G Ninja ..` for configuration
- Use `ninja` for building (supports parallel builds automatically)
- Support both debug and release configurations via CMake
- Clean build artifacts with `ninja clean` or remove build directory
- Build should be reproducible

## Dependencies
- **cpp-httplib** - HTTP client/server (header-only)
- **nlohmann/json** - JSON parsing (header-only)
- **OpenSSL** - SSL/TLS support (system library)
- Prefer header-only libraries when possible
- Minimize external dependencies
- Document all dependencies and versions

## Version Control
- Meaningful commit messages
- Atomic commits (one logical change per commit)
- Use branches for features
- Never commit build artifacts or sensitive data
- Follow semantic versioning (v0.0.x)

## Code Review Checklist
- Follows style guide
- Has tests with good coverage
- Handles errors appropriately
- No memory leaks or resource leaks
- Thread-safe if needed
- Documented appropriately
- Registered in INDEX.md
- Under 300 lines per file
