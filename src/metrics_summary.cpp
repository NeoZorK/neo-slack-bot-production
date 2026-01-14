#include "metrics.hpp"
#include <sstream>
#include <iomanip>

std::string Metrics::get_summary() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    oss << "Metrics Summary:\n";
    
    size_t mem_usage = current_memory_usage_.load();
    if (mem_usage == 0) {
        oss << "  Memory Usage: Not tracked\n";
    } else {
        oss << "  Memory Usage: " << mem_usage << " bytes";
        if (mem_usage >= 1024 * 1024) {
            oss << " (" << std::fixed << std::setprecision(2) << static_cast<double>(mem_usage) / (1024 * 1024) << " MB)";
        } else if (mem_usage >= 1024) {
            oss << " (" << std::fixed << std::setprecision(2) << static_cast<double>(mem_usage) / 1024 << " KB)";
        }
        oss << "\n";
    }
    double cpu_usage = current_cpu_usage_.load();
    if (cpu_usage < 0.0) {
        oss << "  CPU Usage: Not tracked\n";
    } else {
        oss << "  CPU Usage: " << std::fixed << std::setprecision(2) << cpu_usage << "%\n";
    }
    oss << "  Events:\n";
    if (event_counts_.empty()) {
        oss << "    (none)\n";
    } else {
        for (const auto& [type, count] : event_counts_) {
            oss << "    " << type << ": " << count.load() << "\n";
        }
    }
    oss << "  Errors:\n";
    if (error_counts_.empty()) {
        oss << "    (none)\n";
    } else {
        for (const auto& [type, count] : error_counts_) {
            oss << "    " << type << ": " << count.load() << "\n";
        }
    }
    
    // Latencies
    oss << "  Latencies (min/avg/max ms):\n";
    if (latency_sums_.empty()) {
        oss << "    (none)\n";
    } else {
        for (const auto& [op, sum] : latency_sums_) {
            auto count_it = latency_counts_.find(op);
            auto min_it = latency_mins_.find(op);
            auto max_it = latency_maxs_.find(op);
            if (count_it != latency_counts_.end()) {
                uint64_t count = count_it->second.load();
                double avg = (count > 0) ? static_cast<double>(sum.load()) / count : 0.0;
                double min_val = (min_it != latency_mins_.end() && min_it->second.load() > 0) 
                    ? static_cast<double>(min_it->second.load()) : 0.0;
                double max_val = (max_it != latency_maxs_.end()) 
                    ? static_cast<double>(max_it->second.load()) : 0.0;
                oss << "    " << op << ": ";
                if (min_val > 0 || max_val > 0) {
                    oss << std::fixed << std::setprecision(2) << min_val << "/" << avg << "/" << max_val;
                } else {
                    oss << std::fixed << std::setprecision(2) << avg;
                }
                oss << "\n";
            }
        }
    }
    return oss.str();
}
