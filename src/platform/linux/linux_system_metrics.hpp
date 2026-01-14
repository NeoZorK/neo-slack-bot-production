#ifndef LINUX_SYSTEM_METRICS_HPP
#define LINUX_SYSTEM_METRICS_HPP

#include "../../interfaces/i_system_metrics.hpp"
#include <chrono>

/**
 * @brief Linux implementation of ISystemMetrics using /proc filesystem
 * 
 * Uses /proc/self/status for memory metrics and /proc/stat for CPU metrics.
 * 
 * Thread Safety: Thread-safe. File reading operations are safe to call
 * from multiple threads.
 */
class LinuxSystemMetrics : public ISystemMetrics {
public:
    LinuxSystemMetrics();
    ~LinuxSystemMetrics() override = default;
    
    size_t get_process_memory_usage() const override;
    double get_process_cpu_usage() const override;
    bool is_memory_available() const override;
    bool is_cpu_available() const override;

private:
    mutable size_t last_cpu_time_;
    mutable std::chrono::steady_clock::time_point last_cpu_check_;
    
    size_t read_memory_from_proc() const;
    double calculate_cpu_usage() const;
};

#endif // LINUX_SYSTEM_METRICS_HPP
