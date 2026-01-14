#pragma once

#include <cstddef>

namespace bot_network {

struct SystemMetrics {
    double cpu_percent;
    size_t memory_bytes;
    size_t network_bytes_sent;
    size_t network_bytes_received;
    size_t io_operations;
};

// Get process memory usage (macOS)
size_t get_process_memory();

// Get CPU usage
double get_cpu_usage();

} // namespace bot_network
