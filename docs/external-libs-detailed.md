# External Libraries Detailed

## Overview

This document provides detailed information about external library integration, security considerations, dependency management, and future plans.

## Installation Details

### cpp-httplib

**Installation**:
- Download `httplib.h` from repository
- Place in `src/include/` or `src/third_party/`
- Include in source files

**Future**: May be managed via git submodule or package manager

**Configuration**:
- System certificate store integration
- Custom CA bundle support (if needed)
- TLS version negotiation

### nlohmann/json

**Installation**:
- Download `json.hpp` from repository
- Place in `src/include/` or `src/third_party/`
- Include in source files

**Future**: May use package manager or git submodule

**Advanced Features**:
- JSON Schema validation (optional)
- Binary formats (CBOR, MessagePack)
- Custom type serialization
- Exception-based error handling

### OpenSSL

**Installation** (macOS):
```bash
brew install openssl
```

**Configuration**:
- System certificate store integration
- Custom CA bundle support (if needed)
- TLS version negotiation

**Note**: OpenSSL is a system dependency, not bundled with the project

## Security Considerations

### Dependency Security

**Best Practices**:
- Use latest stable versions
- Monitor security advisories
- Update dependencies regularly
- Review dependency code (when possible)

**Security Updates**:
- Header-only libraries: Update header file
- System libraries: Update via package manager
- Monitor CVE databases
- Test after updates

### Supply Chain Security

**Risks**:
- Compromised dependencies
- Malicious code injection
- License violations
- Outdated dependencies

**Mitigation**:
- Use trusted sources
- Verify checksums (future)
- Review dependency code
- Keep dependencies updated

## Build Integration

### Include Paths

**Header-Only Libraries**:
```makefile
INCLUDES = -I./include -I./third_party
```

**System Libraries**:
```makefile
LDFLAGS = -lssl -lcrypto -lpthread
CPPFLAGS = -I$(brew --prefix openssl)/include
```

### Makefile Integration

**Current Approach**:
- Libraries included in source
- System libraries linked
- No external build steps

**Future Enhancements**:
- Dependency detection
- Automatic download
- Version checking
- Build configuration

## Version Management

### Version Pinning

**Current**: Latest stable versions

**Future**: Pin specific versions for reproducibility

**Benefits**:
- Reproducible builds
- Known compatibility
- Security updates tracked
- Easier debugging

### Version Tracking

**Current Versions**:
- cpp-httplib: Latest stable
- nlohmann/json: v3.11.2+
- OpenSSL: System-provided (1.1.x or 3.x)

**Update Strategy**:
- Security updates: Immediately
- Feature updates: As needed
- Regular updates: Quarterly

## Future Dependencies

### Potential Additions

**Testing Framework**:
- Catch2 or Google Test
- Mock framework
- Coverage tools

**Logging Library**:
- spdlog (header-only)
- Custom logging solution

**Configuration Library**:
- toml11 (TOML parsing)
- Custom JSON config

**Utilities**:
- fmt (formatting library)
- date (date/time library)

### Evaluation Criteria

**Before Adding Dependency**:
1. **Necessity**: Is it really needed?
2. **License**: Compatible license?
3. **Maintenance**: Actively maintained?
4. **Size**: Reasonable size?
5. **Dependencies**: Minimal dependencies?
6. **Documentation**: Well documented?
7. **Community**: Active community?

## Maintenance

### Updating Dependencies

**Process**:
1. Check for updates
2. Review changelog
3. Test compatibility
4. Update dependency
5. Run test suite
6. Update documentation

**Frequency**:
- Security updates: Immediately
- Feature updates: As needed
- Regular updates: Quarterly

### Monitoring

**Tools** (future):
- Dependabot (GitHub)
- Renovate
- Manual monitoring

**Tracking**:
- Version numbers
- Security advisories
- Compatibility issues
- Performance impact

## Documentation

### Dependency Documentation

**Included**:
- Purpose and usage
- Version information
- Installation instructions
- Integration details

**External Resources**:
- Library documentation
- API references
- Examples and tutorials
- Community resources

## Integration Examples

### Using cpp-httplib

**Basic HTTP Request**:
```cpp
#include "httplib.h"

httplib::Client cli("slack.com");
cli.set_bearer_token_auth("xoxb-BOT_TOKEN_PLACEHOLDER");

std::string body = R"({"channel":"C123","text":"Hello"})";
auto res = cli.Post("/api/chat.postMessage", body, "application/json");
```

**HTTPS with SSL**:
```cpp
httplib::SSLClient cli("slack.com");
// Automatic SSL/TLS handling
```

### Using nlohmann/json

**Parsing JSON**:
```cpp
#include "json.hpp"
using json = nlohmann::json;

std::string json_str = R"({"ok":true,"message":{"text":"Hello"}})";
json j = json::parse(json_str);
std::string text = j["message"]["text"];
```

**Creating JSON**:
```cpp
json j;
j["channel"] = "C123";
j["text"] = "Hello";
std::string json_str = j.dump();
```

### Using OpenSSL

**TLS Connection**:
- Used internally by cpp-httplib
- Used for WebSocket TLS connections
- Automatic certificate validation
- System certificate store integration

## Troubleshooting

### Common Issues

**OpenSSL Not Found**:
- Install via Homebrew: `brew install openssl`
- Set include path: `-I$(brew --prefix openssl)/include`
- Set library path: `-L$(brew --prefix openssl)/lib`

**Header Not Found**:
- Check include paths in Makefile
- Verify header files exist in `src/include/`
- Check file permissions

**Link Errors**:
- Verify OpenSSL is installed
- Check library paths
- Ensure `-lssl -lcrypto` in LDFLAGS

## See Also

- [External Libraries Overview](external-libs-overview.md) - Quick overview of all dependencies
