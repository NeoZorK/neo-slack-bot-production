#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <thread>
#include "../src/websocket_client.hpp"
#include "../src/message_filter.hpp"
#include "../src/settings.hpp"
#include "../src/metrics.hpp"

using namespace std::chrono;

// Performance latency tests with detailed statistics

class LatencyProfiler {
public:
    void start() {
        start_time = high_resolution_clock::now();
    }
    
    void stop() {
        end_time = high_resolution_clock::now();
    }
    
    double elapsed_us() const {
        return duration_cast<microseconds>(end_time - start_time).count();
    }
    
    void record_sample(double value) {
        samples.push_back(value);
    }
    
    struct Statistics {
        double min;
        double max;
        double mean;
        double median;
        double p95;
        double p99;
        double stddev;
    };
    
    Statistics calculate_statistics() const {
        if (samples.empty()) {
            return {0, 0, 0, 0, 0, 0, 0};
        }
        
        std::vector<double> sorted = samples;
        std::sort(sorted.begin(), sorted.end());
        
        Statistics stats;
        stats.min = sorted.front();
        stats.max = sorted.back();
        
        double sum = 0;
        for (double s : sorted) {
            sum += s;
        }
        stats.mean = sum / sorted.size();
        
        size_t n = sorted.size();
        stats.median = (n % 2 == 0) ? 
            (sorted[n/2 - 1] + sorted[n/2]) / 2.0 : 
            sorted[n/2];
        
        stats.p95 = sorted[static_cast<size_t>(n * 0.95)];
        stats.p99 = sorted[static_cast<size_t>(n * 0.99)];
        
        double variance = 0;
        for (double s : sorted) {
            variance += (s - stats.mean) * (s - stats.mean);
        }
        stats.stddev = std::sqrt(variance / sorted.size());
        
        return stats;
    }
    
    void reset() {
        samples.clear();
    }

private:
    high_resolution_clock::time_point start_time;
    high_resolution_clock::time_point end_time;
    std::vector<double> samples;
};

void performance_test_websocket_client_latency(int iterations) {
    LatencyProfiler profiler;
    
    std::cout << "\n--- WebSocket Client Latency Test ---" << std::endl;
    
    for (int i = 0; i < iterations; ++i) {
        profiler.start();
        WebSocketClient client("test-token");
        profiler.stop();
        profiler.record_sample(profiler.elapsed_us());
    }
    
    auto stats = profiler.calculate_statistics();
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "  Min: " << stats.min << " us" << std::endl;
    std::cout << "  Max: " << stats.max << " us" << std::endl;
    std::cout << "  Mean: " << stats.mean << " us" << std::endl;
    std::cout << "  Median: " << stats.median << " us" << std::endl;
    std::cout << "  P95: " << stats.p95 << " us" << std::endl;
    std::cout << "  P99: " << stats.p99 << " us" << std::endl;
    std::cout << "  StdDev: " << stats.stddev << " us" << std::endl;
    
    assert(stats.mean < 100.0); // Mean latency < 100 microseconds
    assert(stats.p95 < 200.0);   // P95 latency < 200 microseconds
}

void performance_test_message_filter_latency(int iterations) {
    Settings settings = Settings::get_defaults();
    settings.notifications.channel_filter.mode = Settings::NotificationSettings::ChannelFilter::WHITELIST;
    settings.notifications.channel_filter.channels.insert("C12345678");
    settings.events.keyword_filter.enabled = true;
    settings.events.keyword_filter.mode = Settings::EventSettings::KeywordFilter::INCLUDE;
    settings.events.keyword_filter.keywords = {"urgent", "important"};
    
    MessageFilter filter(settings);
    LatencyProfiler profiler;
    
    std::cout << "\n--- Message Filter Latency Test ---" << std::endl;
    
    for (int i = 0; i < iterations; ++i) {
        profiler.start();
        filter.should_process("C12345678", "This is an urgent message", "U1234567890");
        profiler.stop();
        profiler.record_sample(profiler.elapsed_us());
    }
    
    auto stats = profiler.calculate_statistics();
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "  Min: " << stats.min << " us" << std::endl;
    std::cout << "  Max: " << stats.max << " us" << std::endl;
    std::cout << "  Mean: " << stats.mean << " us" << std::endl;
    std::cout << "  P95: " << stats.p95 << " us" << std::endl;
    
    assert(stats.mean < 10.0); // Mean latency < 10 microseconds
    assert(stats.p95 < 50.0);   // P95 latency < 50 microseconds
}

void performance_test_metrics_accuracy(int iterations) {
    Metrics& metrics = Metrics::get_instance();
    metrics.reset();
    
    std::cout << "\n--- Metrics Accuracy Test ---" << std::endl;
    
    // Generate linear distribution 1..100 ms repeated
    // Since MAX_LATENCY_SAMPLES is 1000, we should use at least 1000 iterations to fill buffer
    // and verify circular buffer behavior
    for (int i = 0; i < iterations; ++i) {
        // 1..100 cyclic
        int ms = (i % 100) + 1; 
        metrics.record_latency("test_accuracy", std::chrono::milliseconds(ms));
    }
    
    double p50 = metrics.get_percentile_latency("test_accuracy", 50.0);
    double p95 = metrics.get_percentile_latency("test_accuracy", 95.0);
    double p99 = metrics.get_percentile_latency("test_accuracy", 99.0);
    
    std::cout << "  Iterations: " << iterations << std::endl;
    std::cout << "  P50 (Expected ~50): " << p50 << std::endl;
    std::cout << "  P95 (Expected ~95): " << p95 << std::endl;
    std::cout << "  P99 (Expected ~99): " << p99 << std::endl;
    
    // Check accuracy within reasonable margin
    // With 1000 samples and 1..100 distribution, we have ~10 samples per value.
    // p50 should be 50 or 51.
    assert(std::abs(p50 - 50.0) < 2.0);
    assert(std::abs(p95 - 95.0) < 2.0);
    assert(std::abs(p99 - 99.0) < 2.0);
}

// Export for test_performance_advanced.cpp
void test_performance_latency() {
    performance_test_websocket_client_latency(10000);
    performance_test_message_filter_latency(100000);
    performance_test_metrics_accuracy(2000); // 2000 > 1000 buffer size
}
