#include "macos_system_metrics.hpp"
#include <mach/mach.h>
#include <mach/task_info.h>
#include <sys/resource.h>
#include <chrono>

MacOSSystemMetrics::MacOSSystemMetrics() 
    : last_cpu_time_(0) {
    last_cpu_check_ = std::chrono::steady_clock::now();
}

size_t MacOSSystemMetrics::get_process_memory_usage() const {
    // Use Mach API for macOS (more accurate)
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    kern_return_t kerr = task_info(mach_task_self(), TASK_BASIC_INFO, 
                                   (task_info_t)&info, &size);
    if (kerr == KERN_SUCCESS) {
        return info.resident_size; // Resident memory size in bytes
    }
    
    // Fallback to getrusage
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        // ru_maxrss is in KB on macOS
        return static_cast<size_t>(usage.ru_maxrss) * 1024;
    }
    
    return 0;
}

double MacOSSystemMetrics::get_process_cpu_usage() const {
    // Use getrusage for CPU time
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        // Calculate CPU usage percentage
        // This is a simplified implementation
        // For more accurate CPU usage, need to track time intervals
        size_t cpu_time = usage.ru_utime.tv_sec * 1000 + usage.ru_utime.tv_usec / 1000;
        cpu_time += usage.ru_stime.tv_sec * 1000 + usage.ru_stime.tv_usec / 1000;
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - last_cpu_check_).count();
        
        if (elapsed > 0 && last_cpu_time_ > 0) {
            size_t cpu_delta = cpu_time - last_cpu_time_;
            double cpu_percent = (static_cast<double>(cpu_delta) / elapsed) * 100.0;
            last_cpu_time_ = cpu_time;
            last_cpu_check_ = now;
            return cpu_percent;
        }
        
        last_cpu_time_ = cpu_time;
        last_cpu_check_ = now;
    }
    
    return -1.0;
}

bool MacOSSystemMetrics::is_memory_available() const {
    return true; // Mach API is always available on macOS
}

bool MacOSSystemMetrics::is_cpu_available() const {
    return true; // getrusage is always available on macOS
}
