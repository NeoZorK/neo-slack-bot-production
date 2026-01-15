#include "windows_system_metrics.hpp"

WindowsSystemMetrics::WindowsSystemMetrics() {
}

size_t WindowsSystemMetrics::get_process_memory_usage() const {
    // Stub implementation
    // TODO: Implement Windows Performance API
    // Future implementation will use:
    // - GetProcessMemoryInfo from psapi.h for memory metrics
    // - Link with psapi.lib library
    // See docs/windows-support.md for details
    return 0;
}

double WindowsSystemMetrics::get_process_cpu_usage() const {
    // Stub implementation
    // TODO: Implement Windows Performance API
    // Future implementation will use:
    // - GetProcessTimes for CPU usage calculation
    // - Calculate percentage based on system time
    // See docs/windows-support.md for details
    return -1.0;
}

bool WindowsSystemMetrics::is_memory_available() const {
    return false; // Not implemented yet
}

bool WindowsSystemMetrics::is_cpu_available() const {
    return false; // Not implemented yet
}
