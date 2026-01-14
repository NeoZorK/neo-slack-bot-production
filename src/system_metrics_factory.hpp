#ifndef SYSTEM_METRICS_FACTORY_HPP
#define SYSTEM_METRICS_FACTORY_HPP

#include "interfaces/i_system_metrics.hpp"
#include <memory>

/**
 * @brief Factory for creating platform-specific system metrics implementations
 * 
 * Automatically detects the current platform and creates the appropriate
 * system metrics implementation.
 * 
 * Thread Safety: Factory methods are thread-safe. The returned instances
 * have platform-specific thread safety guarantees.
 */
class SystemMetricsFactory {
public:
    /**
     * @brief Create system metrics service
     * @return Unique pointer to ISystemMetrics implementation
     */
    static std::unique_ptr<ISystemMetrics> create();
    
private:
    SystemMetricsFactory() = delete;
    SystemMetricsFactory(const SystemMetricsFactory&) = delete;
    SystemMetricsFactory& operator=(const SystemMetricsFactory&) = delete;
};

#endif // SYSTEM_METRICS_FACTORY_HPP
