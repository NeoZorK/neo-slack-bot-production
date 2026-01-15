#include "linux_system_metrics.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <sys/resource.h>

LinuxSystemMetrics::LinuxSystemMetrics() 
    : last_cpu_time_(0) {
    last_cpu_check_ = std::chrono::steady_clock::now();
}

size_t LinuxSystemMetrics::get_process_memory_usage() const {
    // Try /proc/self/status first (more accurate)
    size_t mem = read_memory_from_proc();
    if (mem > 0) {
        return mem;
    }
    
    // Fallback to getrusage
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        // ru_maxrss is in KB on Linux
        return static_cast<size_t>(usage.ru_maxrss) * 1024;
    }
    
    return 0;
}

size_t LinuxSystemMetrics::read_memory_from_proc() const {
    std::ifstream status_file("/proc/self/status");
    if (!status_file.is_open()) {
        return 0;
    }
    
    std::string line;
    while (std::getline(status_file, line)) {
        if (line.find("VmRSS:") == 0) {
            // Extract memory value (in KB)
            std::istringstream iss(line);
            std::string key, value, unit;
            iss >> key >> value >> unit;
            
            if (!value.empty()) {
                size_t mem_kb = std::stoull(value);
                return mem_kb * 1024; // Convert KB to bytes
            }
        }
    }
    
    return 0;
}

double LinuxSystemMetrics::get_process_cpu_usage() const {
    return calculate_cpu_usage();
}

double LinuxSystemMetrics::calculate_cpu_usage() const {
    // Read CPU time from /proc/self/stat
    std::ifstream stat_file("/proc/self/stat");
    if (!stat_file.is_open()) {
        return -1.0;
    }
    
    std::string line;
    std::getline(stat_file, line);
    
    std::istringstream iss(line);
    std::string token;
    size_t utime = 0, stime = 0;
    int field = 0;
    
    // Parse /proc/self/stat format
    // Fields: pid, comm, state, ppid, pgrp, session, tty_nr, ...
    // Field 14: utime, Field 15: stime
    while (iss >> token && field < 16) {
        if (field == 13) {
            utime = std::stoull(token);
        } else if (field == 14) {
            stime = std::stoull(token);
            break;
        }
        field++;
    }
    
    if (utime == 0 && stime == 0) {
        return -1.0;
    }
    
    size_t cpu_time = utime + stime;
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
    return -1.0;
}

bool LinuxSystemMetrics::is_memory_available() const {
    // Check if /proc/self/status exists
    std::ifstream status_file("/proc/self/status");
    return status_file.is_open();
}

bool LinuxSystemMetrics::is_cpu_available() const {
    // Check if /proc/self/stat exists
    std::ifstream stat_file("/proc/self/stat");
    return stat_file.is_open();
}
