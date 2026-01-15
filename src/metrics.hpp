#ifndef METRICS_HPP
#define METRICS_HPP

#include <chrono>
#include <atomic>
#include <string>
#include <map>
#include <mutex>
#include <vector>
#include <deque>

/**
 * @brief Metrics collection and monitoring system
 * 
 * Thread Safety: Thread-safe. All public methods can be called concurrently
 * from multiple threads. Uses atomic operations and mutex for synchronization.
 */
class Metrics {
public:
    /**
     * @brief Get singleton instance
     * @return Reference to Metrics instance
     */
    static Metrics& get_instance() {
        static Metrics instance;
        return instance;
    }
    
    /**
     * @brief Increment event counter
     * @param event_type Type of event to increment
     */
    void increment_event_count(const std::string& event_type);
    
    /**
     * @brief Record operation latency
     * @param operation Operation name
     * @param duration Duration in milliseconds
     */
    void record_latency(const std::string& operation, 
                       std::chrono::milliseconds duration);
    
    /**
     * @brief Record error occurrence
     * @param error_type Type of error
     */
    void record_error(const std::string& error_type);
    
    /**
     * @brief Record memory usage
     * @param bytes Memory usage in bytes
     */
    void record_memory_usage(size_t bytes);
    
    /**
     * @brief Update memory usage from system
     * 
     * Queries system for current process memory usage and updates metrics.
     */
    void update_memory_usage();
    
    /**
     * @brief Get event count
     * @param event_type Event type to query
     * @return Count of events
     */
    uint64_t get_event_count(const std::string& event_type) const;
    
    /**
     * @brief Get average latency for operation
     * @param operation Operation name
     * @return Average latency in milliseconds, or 0.0 if no data
     */
    double get_average_latency(const std::string& operation) const;
    
    /**
     * @brief Get error count
     * @param error_type Error type to query
     * @return Count of errors
     */
    uint64_t get_error_count(const std::string& error_type) const;
    
    /**
     * @brief Get current memory usage
     * @return Current memory usage in bytes
     */
    size_t get_current_memory_usage() const;
    
    /**
     * @brief Get current CPU usage percentage
     * @return CPU usage percentage (0.0 to 100.0), or -1.0 if unavailable
     */
    double get_current_cpu_usage() const;
    
    /**
     * @brief Update CPU usage from system
     * 
     * Queries system for current process CPU usage and updates metrics.
     */
    void update_cpu_usage();
    
    /**
     * @brief Record CPU usage value
     * @param cpu_percent CPU usage percentage (0.0 to 100.0)
     */
    void record_cpu_usage(double cpu_percent);
    
    /**
     * @brief Get minimum latency for operation
     * @param operation Operation name
     * @return Minimum latency in milliseconds, or 0.0 if no data
     */
    double get_min_latency(const std::string& operation) const;
    
    /**
     * @brief Get maximum latency for operation
     * @param operation Operation name
     * @return Maximum latency in milliseconds, or 0.0 if no data
     */
    double get_max_latency(const std::string& operation) const;
    
    /**
     * @brief Get total latency sum for operation
     * @param operation Operation name
     * @return Total latency sum in milliseconds
     */
    uint64_t get_total_latency(const std::string& operation) const;
    
    /**
     * @brief Get latency count for operation
     * @param operation Operation name
     * @return Number of latency measurements
     */
    uint64_t get_latency_count(const std::string& operation) const;
    
    /**
     * @brief Reset all metrics
     */
    void reset();
    
    /**
     * @brief Get metrics summary as string
     * @return Formatted summary string
     */
    std::string get_summary() const;
    
    /**
     * @brief Get peak memory usage since last reset
     * @return Peak memory usage in bytes
     */
    size_t get_peak_memory_usage() const;
    
    /**
     * @brief Get minimum memory usage since last reset
     * @return Minimum memory usage in bytes
     */
    size_t get_min_memory_usage() const;
    
    /**
     * @brief Get average memory usage since last reset
     * @return Average memory usage in bytes, or 0 if no samples
     */
    double get_average_memory_usage() const;
    
    /**
     * @brief Get peak CPU usage since last reset
     * @return Peak CPU usage percentage, or -1.0 if unavailable
     */
    double get_peak_cpu_usage() const;
    
    /**
     * @brief Get minimum CPU usage since last reset
     * @return Minimum CPU usage percentage, or -1.0 if unavailable
     */
    double get_min_cpu_usage() const;
    
    /**
     * @brief Get average CPU usage since last reset
     * @return Average CPU usage percentage, or -1.0 if unavailable
     */
    double get_average_cpu_usage() const;
    
    /**
     * @brief Get median latency for operation
     * @param operation Operation name
     * @return Median latency in milliseconds, or 0.0 if no data
     */
    double get_median_latency(const std::string& operation) const;

    /**
     * @brief Get percentile latency for operation
     * @param operation Operation name
     * @param percentile Percentile (0.0 to 100.0)
     * @return Percentile latency in milliseconds, or 0.0 if no data
     */
    double get_percentile_latency(const std::string& operation, double percentile) const;
    
    /**
     * @brief Update all system metrics (memory and CPU)
     * 
     * Convenience method to update both memory and CPU usage from system.
     */
    void update_all_metrics();
    
private:
    static constexpr size_t MAX_LATENCY_SAMPLES = 1000;

    Metrics() = default;
    ~Metrics() = default;
    Metrics(const Metrics&) = delete;
    Metrics& operator=(const Metrics&) = delete;
    
    std::map<std::string, std::atomic<uint64_t>> event_counts_;
    std::map<std::string, std::atomic<uint64_t>> error_counts_;
    std::map<std::string, std::atomic<uint64_t>> latency_sums_;
    std::map<std::string, std::atomic<uint64_t>> latency_counts_;
    std::map<std::string, std::atomic<uint64_t>> latency_mins_;
    std::map<std::string, std::atomic<uint64_t>> latency_maxs_;
    std::map<std::string, std::deque<uint64_t>> latency_samples_;
    std::atomic<size_t> current_memory_usage_{0};
    std::atomic<double> current_cpu_usage_{-1.0};
    
    // Historical metrics tracking
    std::atomic<size_t> peak_memory_usage_{0};
    std::atomic<size_t> min_memory_usage_{SIZE_MAX};
    std::atomic<uint64_t> memory_sum_{0};
    std::atomic<uint64_t> memory_samples_{0};
    
    std::atomic<double> peak_cpu_usage_{-1.0};
    std::atomic<double> min_cpu_usage_{101.0};
    std::atomic<double> cpu_sum_{-1.0};
    std::atomic<uint64_t> cpu_samples_{0};
    
    mutable std::mutex mutex_;
};

#endif // METRICS_HPP
