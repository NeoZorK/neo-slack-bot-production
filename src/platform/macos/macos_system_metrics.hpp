#ifndef MACOS_SYSTEM_METRICS_HPP
#define MACOS_SYSTEM_METRICS_HPP

#include "../../interfaces/i_system_metrics.hpp"
#include <chrono>

/**
 * @brief macOS implementation of ISystemMetrics using Mach API
 * 
 * Uses Mach API for accurate memory metrics and getrusage for CPU metrics.
 * 
 * Thread Safety: Thread-safe. Uses system APIs that are safe to call
 * from multiple threads.
 */
class MacOSSystemMetrics : public ISystemMetrics {
public:
    MacOSSystemMetrics();
    ~MacOSSystemMetrics() override = default;
    
    size_t get_process_memory_usage() const override;
    double get_process_cpu_usage() const override;
    bool is_memory_available() const override;
    bool is_cpu_available() const override;

private:
    mutable size_t last_cpu_time_;
    mutable std::chrono::steady_clock::time_point last_cpu_check_;
};

#endif // MACOS_SYSTEM_METRICS_HPP
