#include "metrics.hpp"
#include "logger.hpp"
#include "system_metrics_factory.hpp"
#include <sstream>
#include <iomanip>
#include <memory>
#include <climits>
#include <algorithm>
#include <vector>
#include <cmath>

void Metrics::increment_event_count(const std::string& event_type) {
    std::lock_guard<std::mutex> lock(mutex_);
    event_counts_[event_type]++;
}

void Metrics::record_latency(const std::string& operation, 
                             std::chrono::milliseconds duration) {
    std::lock_guard<std::mutex> lock(mutex_);
    uint64_t ms = duration.count();
    latency_sums_[operation] += ms;
    latency_counts_[operation]++;
    
    // Update samples
    auto& samples = latency_samples_[operation];
    samples.push_back(ms);
    if (samples.size() > MAX_LATENCY_SAMPLES) {
        samples.pop_front();
    }
    
    // Update min/max
    auto& min_val = latency_mins_[operation];
    auto& max_val = latency_maxs_[operation];
    uint64_t current_min = min_val.load();
    uint64_t current_max = max_val.load();
    
    if (current_min == 0 || ms < current_min) {
        min_val.store(ms);
    }
    if (ms > current_max) {
        max_val.store(ms);
    }
}

void Metrics::record_error(const std::string& error_type) {
    std::lock_guard<std::mutex> lock(mutex_);
    error_counts_[error_type]++;
}

void Metrics::record_memory_usage(size_t bytes) {
    current_memory_usage_.store(bytes);
    
    // Update historical metrics
    size_t current_peak = peak_memory_usage_.load();
    if (bytes > current_peak) {
        peak_memory_usage_.store(bytes);
    }
    
    size_t current_min = min_memory_usage_.load();
    if (bytes < current_min || current_min == SIZE_MAX) {
        min_memory_usage_.store(bytes);
    }
    
    // Update average calculation
    memory_sum_.fetch_add(bytes);
    memory_samples_.fetch_add(1);
}

namespace {
    // Platform-specific system metrics instance
    std::unique_ptr<ISystemMetrics> system_metrics = nullptr;
    
    ISystemMetrics* get_system_metrics() {
        if (!system_metrics) {
            system_metrics = SystemMetricsFactory::create();
        }
        return system_metrics.get();
    }
}

void Metrics::update_memory_usage() {
    auto* metrics = get_system_metrics();
    if (metrics && metrics->is_memory_available()) {
        size_t mem_usage = metrics->get_process_memory_usage();
        record_memory_usage(mem_usage);
    }
}

void Metrics::update_cpu_usage() {
    auto* metrics = get_system_metrics();
    if (metrics && metrics->is_cpu_available()) {
        double cpu_usage = metrics->get_process_cpu_usage();
        if (cpu_usage >= 0.0) {
            record_cpu_usage(cpu_usage);
        }
    }
}

void Metrics::record_cpu_usage(double cpu_percent) {
    if (cpu_percent >= 0.0 && cpu_percent <= 100.0) {
        current_cpu_usage_.store(cpu_percent);
        
        // Update historical metrics
        double current_peak = peak_cpu_usage_.load();
        if (cpu_percent > current_peak || current_peak < 0.0) {
            peak_cpu_usage_.store(cpu_percent);
        }
        
        double current_min = min_cpu_usage_.load();
        if (cpu_percent < current_min || current_min > 100.0) {
            min_cpu_usage_.store(cpu_percent);
        }
        
        // Update average calculation
        double current_sum = cpu_sum_.load();
        if (current_sum < 0.0) {
            cpu_sum_.store(cpu_percent);
        } else {
            cpu_sum_.store(current_sum + cpu_percent);
        }
        cpu_samples_.fetch_add(1);
    }
}

double Metrics::get_current_cpu_usage() const {
    return current_cpu_usage_.load();
}

uint64_t Metrics::get_event_count(const std::string& event_type) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = event_counts_.find(event_type);
    return (it != event_counts_.end()) ? it->second.load() : 0;
}

uint64_t Metrics::get_error_count(const std::string& error_type) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = error_counts_.find(error_type);
    return (it != error_counts_.end()) ? it->second.load() : 0;
}

size_t Metrics::get_current_memory_usage() const {
    return current_memory_usage_.load();
}

double Metrics::get_average_latency(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto sum_it = latency_sums_.find(operation);
    auto count_it = latency_counts_.find(operation);
    if (sum_it != latency_sums_.end() && count_it != latency_counts_.end()) {
        uint64_t count = count_it->second.load();
        return (count > 0) ? static_cast<double>(sum_it->second.load()) / static_cast<double>(count) : 0.0;
    }
    return 0.0;
}

double Metrics::get_min_latency(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = latency_mins_.find(operation);
    return (it != latency_mins_.end() && it->second.load() > 0) ? static_cast<double>(it->second.load()) : 0.0;
}

double Metrics::get_max_latency(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = latency_maxs_.find(operation);
    return (it != latency_maxs_.end()) ? static_cast<double>(it->second.load()) : 0.0;
}

uint64_t Metrics::get_total_latency(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = latency_sums_.find(operation);
    return (it != latency_sums_.end()) ? it->second.load() : 0;
}

uint64_t Metrics::get_latency_count(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = latency_counts_.find(operation);
    return (it != latency_counts_.end()) ? it->second.load() : 0;
}

void Metrics::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    event_counts_.clear();
    error_counts_.clear();
    latency_sums_.clear();
    latency_counts_.clear();
    latency_mins_.clear();
    latency_maxs_.clear();
    latency_samples_.clear();
    current_memory_usage_.store(0);
    current_cpu_usage_.store(-1.0);
    
    // Reset historical metrics
    peak_memory_usage_.store(0);
    min_memory_usage_.store(SIZE_MAX);
    memory_sum_.store(0);
    memory_samples_.store(0);
    
    peak_cpu_usage_.store(-1.0);
    min_cpu_usage_.store(101.0);
    cpu_sum_.store(-1.0);
    cpu_samples_.store(0);
}

size_t Metrics::get_peak_memory_usage() const {
    return peak_memory_usage_.load();
}

size_t Metrics::get_min_memory_usage() const {
    size_t min_val = min_memory_usage_.load();
    return (min_val == SIZE_MAX) ? 0 : min_val;
}

double Metrics::get_average_memory_usage() const {
    uint64_t samples = memory_samples_.load();
    return (samples == 0) ? 0.0 : static_cast<double>(memory_sum_.load()) / static_cast<double>(samples);
}

double Metrics::get_peak_cpu_usage() const {
    return peak_cpu_usage_.load();
}

double Metrics::get_min_cpu_usage() const {
    double min_val = min_cpu_usage_.load();
    return (min_val > 100.0) ? -1.0 : min_val;
}

double Metrics::get_average_cpu_usage() const {
    uint64_t samples = cpu_samples_.load();
    if (samples == 0) return -1.0;
    double sum = cpu_sum_.load();
    return (sum < 0.0) ? -1.0 : sum / static_cast<double>(samples);
}

double Metrics::get_median_latency(const std::string& operation) const {
    return get_percentile_latency(operation, 50.0);
}

double Metrics::get_percentile_latency(const std::string& operation, double percentile) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = latency_samples_.find(operation);
    if (it == latency_samples_.end() || it->second.empty()) {
        return 0.0;
    }
    
    std::vector<uint64_t> samples(it->second.begin(), it->second.end());
    std::sort(samples.begin(), samples.end());
    
    if (percentile <= 0.0) return static_cast<double>(samples.front());
    if (percentile >= 100.0) return static_cast<double>(samples.back());
    
    double k = (samples.size() - 1) * (percentile / 100.0);
    size_t f = std::floor(k);
    size_t c = std::ceil(k);
    
    if (f == c) {
        return static_cast<double>(samples[static_cast<size_t>(k)]);
    }
    
    // Linear interpolation
    double d0 = static_cast<double>(samples[f]);
    double d1 = static_cast<double>(samples[c]);
    return d0 + (d1 - d0) * (k - f);
}

void Metrics::update_all_metrics() {
    update_memory_usage();
    update_cpu_usage();
}
