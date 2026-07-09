# Codex AI Assistant Instructions

**Start here**: docs/INDEX.md

You are an AI coding assistant helping develop the Neo Slack Bot project. Follow these instructions carefully.

## Project Context
This is a C++17 Slack bot using Socket Mode (WebSocket) with macOS notifications. The project follows strict architectural principles and coding standards.

## Primary Rules
1. **Always check `docs/INDEX.md` first** - it contains the complete file registry
2. **Read `docs/rules.md`** - contains all coding standards and architectural rules
3. **300 line limit** - NO file (code, tests, docs) can exceed 300 lines
4. **File registration** - Every new file MUST be added to `docs/INDEX.md`

## Code Generation Guidelines

### When Writing Code
- Use C++17 standard features
- Follow snake_case naming convention
- Use smart pointers (`std::unique_ptr`, `std::shared_ptr`) instead of raw pointers
- Implement RAII for resource management
- Add error handling for all operations
- Write self-documenting code with clear variable names
- Keep functions focused and small (single responsibility)

### When Creating Files
- Check if file already exists in `docs/INDEX.md`
- Ensure file is under 300 lines
- If splitting is needed, create logical submodules
- Immediately register new file in `docs/INDEX.md`
- Create corresponding test file in `tests/` directory

### When Modifying Code
- Maintain existing code style
- Preserve API contracts unless explicitly changing them
- Update tests if changing behavior
- Update documentation if changing interfaces
- Ensure all tests still pass

## Build and Test Commands

### Build
**NEVER use `make` directly. ALWAYS use `cmake` with `Ninja`.**

```bash
mkdir -p build
cd build
cmake -G Ninja ..
ninja         # Build all
```

### Test
```bash
cd build
ctest --output-on-failure  # Run all tests
```

### Test Coverage
- Target: 100% code coverage
- All new code must have tests
- Tests should cover success and failure paths

## Project Structure
```
neo-slack-bot/
├── docs/          # All documentation
├── src/           # C++ source code
├── tests/         # Unit tests
├── AGENTS.md      # This file
├── CMakeLists.txt # Root CMake configuration
└── README.md      # Main readme
```

## Documentation Standards
- Technical docs in `docs/` folder
- Code comments in English
- User docs can be Russian (with RU postfix)
- Keep docs updated with code changes

## Common Tasks

### Adding a New Feature
1. Check `docs/INDEX.md` for existing related files
2. Read `docs/rules.md` for architectural guidelines
3. Create source files in `src/` (max 300 lines each)
4. Create test files in `tests/`
5. Register all files in `docs/INDEX.md`
6. Update relevant documentation in `docs/`
7. Ensure tests pass and coverage is maintained

### Fixing a Bug
1. Reproduce the issue
2. Write a failing test case
3. Fix the code
4. Verify test passes
5. Check for similar issues elsewhere
6. Update documentation if behavior changed

### Refactoring
1. Ensure comprehensive test coverage
2. Make small, incremental changes
3. Run tests after each change
4. Update documentation as needed
5. Maintain API compatibility or document breaking changes

## Important Constraints
- **Never** create files in project root (except AGENTS.md, README.md)
- **Never** exceed 300 lines in any file
- **Always** register files in `docs/INDEX.md`
- **Always** write tests for new code
- **Never** commit sensitive data (tokens, passwords)
- **Always** use English for code and comments

## Code Quality Checklist
Before suggesting code, ensure:
- [ ] Follows C++17 standard
- [ ] Uses snake_case naming
- [ ] Uses smart pointers (no raw pointers)
- [ ] Has error handling
- [ ] Is under 300 lines
- [ ] Has corresponding tests
- [ ] Is registered in INDEX.md
- [ ] Follows architectural principles from rules.md

## When Uncertain
1. Check `docs/INDEX.md` for existing patterns
2. Review `docs/rules.md` for guidelines
3. Look at existing code for style examples
4. Ask clarifying questions if needed

## Response Format
- Be concise and direct
- Show code changes using proper citations
- Explain architectural decisions when relevant
- Reference specific rules when applicable
- Provide complete, working code examples
