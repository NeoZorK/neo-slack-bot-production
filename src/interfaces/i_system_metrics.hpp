#ifndef I_SYSTEM_METRICS_HPP
#define I_SYSTEM_METRICS_HPP

#include <cstddef>

/**
 * @brief Platform-independent interface for system metrics collection
 * 
 * Provides unified API for collecting system-level metrics (memory, CPU)
 * across different platforms (macOS, Linux, Windows).
 * 
 * Thread Safety: Implementation-dependent. Each platform implementation
 * should document its thread safety guarantees.
 */
class ISystemMetrics {
public:
    virtual ~ISystemMetrics() = default;
    
    /**
     * @brief Get current process memory usage in bytes
     * @return Memory usage in bytes, or 0 if unavailable
     */
    virtual size_t get_process_memory_usage() const = 0;
    
    /**
     * @brief Get current process CPU usage percentage
     * @return CPU usage percentage (0.0 to 100.0), or -1.0 if unavailable
     */
    virtual double get_process_cpu_usage() const = 0;
    
    /**
     * @brief Check if memory metrics are available
     * @return true if memory metrics can be collected
     */
    virtual bool is_memory_available() const = 0;
    
    /**
     * @brief Check if CPU metrics are available
     * @return true if CPU metrics can be collected
     */
    virtual bool is_cpu_available() const = 0;
};

#endif // I_SYSTEM_METRICS_HPP
