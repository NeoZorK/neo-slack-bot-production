#include "system_metrics_factory.hpp"

// Platform-specific includes
#ifdef __APPLE__
#include "platform/macos/macos_system_metrics.hpp"
#elif defined(__linux__)
#include "platform/linux/linux_system_metrics.hpp"
#elif defined(_WIN32) || defined(_WIN64)
#include "platform/windows/windows_system_metrics.hpp"
#else
#error "Unsupported platform"
#endif

std::unique_ptr<ISystemMetrics> SystemMetricsFactory::create() {
#ifdef __APPLE__
    return std::make_unique<MacOSSystemMetrics>();
#elif defined(__linux__)
    return std::make_unique<LinuxSystemMetrics>();
#elif defined(_WIN32) || defined(_WIN64)
    return std::make_unique<WindowsSystemMetrics>();
#else
    #error "Unsupported platform"
#endif
}
