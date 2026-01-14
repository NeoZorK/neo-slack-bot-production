#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>
#include "metrics.hpp"
#include "system_metrics_factory.hpp"

void test_metrics_event_counting() {
    Metrics& metrics = Metrics::get_instance();
    metrics.reset();
    
    metrics.increment_event_count("test_event");
    metrics.increment_event_count("test_event");
    metrics.increment_event_count("other_event");
    
    assert(metrics.get_event_count("test_event") == 2);
    assert(metrics.get_event_count("other_event") == 1);
    assert(metrics.get_event_count("nonexistent") == 0);
    
    std::cout << "  ✓ Metrics event counting test passed" << std::endl;
}

void test_metrics_latency() {
    Metrics& metrics = Metrics::get_instance();
    metrics.reset();
    
    metrics.record_latency("test_op", std::chrono::milliseconds(100));
    metrics.record_latency("test_op", std::chrono::milliseconds(200));
    metrics.record_latency("test_op", std::chrono::milliseconds(300));
    
    double avg = metrics.get_average_latency("test_op");
    assert(avg >= 199.0 && avg <= 201.0);
    
    std::cout << "  ✓ Metrics latency test passed" << std::endl;
}

void test_metrics_percentile() {
    Metrics& metrics = Metrics::get_instance();
    metrics.reset();
    
    // Add 100 samples from 1 to 100
    for (int i = 1; i <= 100; ++i) {
        metrics.record_latency("test_perc", std::chrono::milliseconds(i));
    }
    
    double p50 = metrics.get_percentile_latency("test_perc", 50.0);
    double p90 = metrics.get_percentile_latency("test_perc", 90.0);
    double p99 = metrics.get_percentile_latency("test_perc", 99.0);
    
    // Expected: 50, 90, 99 (approx)
    assert(std::abs(p50 - 50.0) < 1.0);
    assert(std::abs(p90 - 90.0) < 1.0);
    assert(std::abs(p99 - 99.0) < 1.0);
    
    std::cout << "  ✓ Metrics percentile test passed" << std::endl;
}

void test_metrics_error_counting() {
    Metrics& metrics = Metrics::get_instance();
    metrics.reset();
    
    metrics.record_error("test_error");
    metrics.record_error("test_error");
    metrics.record_error("other_error");
    
    assert(metrics.get_error_count("test_error") == 2);
    assert(metrics.get_error_count("other_error") == 1);
    
    std::cout << "  ✓ Metrics error counting test passed" << std::endl;
}

void test_metrics_memory_usage() {
    Metrics& metrics = Metrics::get_instance();
    metrics.reset();
    
    metrics.record_memory_usage(1024 * 1024);
    assert(metrics.get_current_memory_usage() == 1024 * 1024);
    
    metrics.record_memory_usage(2048 * 1024);
    assert(metrics.get_current_memory_usage() == 2048 * 1024);
    
    std::cout << "  ✓ Metrics memory usage test passed" << std::endl;
}

void test_metrics_reset() {
    Metrics& metrics = Metrics::get_instance();
    
    metrics.increment_event_count("test");
    metrics.record_error("test_error");
    metrics.record_memory_usage(1000);
    
    metrics.reset();
    
    assert(metrics.get_event_count("test") == 0);
    assert(metrics.get_error_count("test_error") == 0);
    assert(metrics.get_current_memory_usage() == 0);
    
    std::cout << "  ✓ Metrics reset test passed" << std::endl;
}

void test_metrics_summary_comprehensive() {
    Metrics& metrics = Metrics::get_instance();
    
    // 1. Test empty/initial state
    metrics.reset();
    // Manually ensure CPU is in "not tracked" state if possible, 
    // but Metrics::reset() usually sets cpu usage to -1.0 or 0.0 depending on impl.
    // Looking at metrics.cpp (not provided but inferred), usually reset clears everything.
    // Let's check output for "Not tracked" or empty lists.
    std::string summary = metrics.get_summary();
    assert(summary.find("Memory Usage: Not tracked") != std::string::npos);
    // CPU usage might be 0.0 or -1.0 depending on implementation of reset/init. 
    // If reset sets it to 0, we might see "0.00%". If -1, "Not tracked".
    // Let's assume reset makes it "Not tracked" or 0. We'll check for absence of specific values if unsure, 
    // but based on metrics_summary.cpp:23 (cpu < 0.0), let's assume it starts at -1.0 or 0.0.
    // Actually, let's just test what we can control.
    assert(summary.find("Events:\n    (none)") != std::string::npos);
    assert(summary.find("Errors:\n    (none)") != std::string::npos);
    assert(summary.find("Latencies (min/avg/max ms):\n    (none)") != std::string::npos);

    // 2. Test Memory Formatting
    
    // Case A: Bytes (< 1024)
    metrics.reset();
    metrics.record_memory_usage(500);
    summary = metrics.get_summary();
    assert(summary.find("Memory Usage: 500 bytes") != std::string::npos);
    assert(summary.find(" KB)") == std::string::npos);
    assert(summary.find(" MB)") == std::string::npos);

    // Case B: KB (>= 1024 && < 1MB)
    metrics.record_memory_usage(2048);
    summary = metrics.get_summary();
    assert(summary.find("Memory Usage: 2048 bytes") != std::string::npos);
    assert(summary.find(" (2.00 KB)") != std::string::npos);

    // Case C: MB (>= 1MB)
    metrics.record_memory_usage(1024 * 1024 * 3 / 2); // 1.5 MB
    summary = metrics.get_summary();
    assert(summary.find("1572864 bytes") != std::string::npos);
    assert(summary.find(" (1.50 MB)") != std::string::npos);

    // 3. Test Lists (Events, Errors, Latencies)
    metrics.reset();
    metrics.increment_event_count("evt1");
    metrics.record_error("err1");
    metrics.record_latency("lat1", std::chrono::milliseconds(10));
    summary = metrics.get_summary();
    assert(summary.find("    evt1: 1") != std::string::npos);
    assert(summary.find("    err1: 1") != std::string::npos);
    assert(summary.find("    lat1:") != std::string::npos);
    assert(summary.find("(none)") == std::string::npos); // Should not have (none) anywhere now

    // 4. Test Latency details
    metrics.reset();
    metrics.record_latency("lat1", std::chrono::milliseconds(100));
    summary = metrics.get_summary();
    // Min/Avg/Max should be populated
    assert(summary.find("lat1: 100.00/100.00/100.00") != std::string::npos);

    std::cout << "  ✓ Metrics comprehensive summary test passed" << std::endl;
}

void test_system_metrics_factory() {
    auto system_metrics = SystemMetricsFactory::create();
    assert(system_metrics != nullptr);
    
    (void)system_metrics->get_process_memory_usage();
    (void)system_metrics->get_process_cpu_usage();
    (void)system_metrics->is_memory_available();
    (void)system_metrics->is_cpu_available();
    
    std::cout << "  ✓ SystemMetricsFactory test passed" << std::endl;
}

void test_metrics() {
    std::cout << "Testing Metrics..." << std::endl;
    
    test_metrics_event_counting();
    test_metrics_latency();
    test_metrics_percentile();
    test_metrics_error_counting();
    test_metrics_memory_usage();
    test_metrics_reset();
    test_metrics_summary_comprehensive();
    test_system_metrics_factory();
    
    std::cout << "Metrics tests passed!" << std::endl;
}
