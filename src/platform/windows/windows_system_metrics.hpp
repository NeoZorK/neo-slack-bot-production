#ifndef WINDOWS_SYSTEM_METRICS_HPP
#define WINDOWS_SYSTEM_METRICS_HPP

#include "../../interfaces/i_system_metrics.hpp"
#include <chrono>

/**
 * @brief Windows implementation of ISystemMetrics (stub)
 * 
 * This is a placeholder implementation for Windows support.
 * Currently returns default values as Windows API integration
 * is planned for future versions.
 * 
 * Thread Safety: Thread-safe.
 */
class WindowsSystemMetrics : public ISystemMetrics {
public:
    WindowsSystemMetrics();
    ~WindowsSystemMetrics() override = default;
    
    size_t get_process_memory_usage() const override;
    double get_process_cpu_usage() const override;
    bool is_memory_available() const override;
    bool is_cpu_available() const override;
};

#endif // WINDOWS_SYSTEM_METRICS_HPP
