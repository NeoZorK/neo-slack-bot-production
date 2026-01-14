#include "system_utils.hpp"

#ifdef __APPLE__
#include <sys/resource.h>
#include <mach/mach.h>
#elif defined(__linux__)
#include <sys/resource.h>
#include <unistd.h>
#include <fstream>
#elif defined(_WIN32)
#include <windows.h>
#include <psapi.h>
#endif

namespace bot_network {

// Get process memory usage
size_t get_process_memory() {
#ifdef __APPLE__
    struct task_basic_info info;
    mach_msg_type_number_t size = TASK_BASIC_INFO_COUNT;
    kern_return_t kerr = task_info(mach_task_self(), TASK_BASIC_INFO, 
                                   (task_info_t)&info, &size);
    if (kerr == KERN_SUCCESS) {
        return info.resident_size;
    }
#elif defined(__linux__)
    long rss = 0;
    std::ifstream statm("/proc/self/statm");
    if (statm >> rss >> rss) { // First is size, second is resident
        return rss * sysconf(_SC_PAGESIZE);
    }
#elif defined(_WIN32)
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
#endif
    return 0;
}

// Get CPU usage
double get_cpu_usage() {
    double result = 0.0;
#if defined(_WIN32)
    FILETIME creation_time;
    FILETIME exit_time;
    FILETIME kernel_time;
    FILETIME user_time;
    if (GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time, &kernel_time, &user_time)) {
        ULARGE_INTEGER k;
        ULARGE_INTEGER u;
        k.LowPart = kernel_time.dwLowDateTime;
        k.HighPart = kernel_time.dwHighDateTime;
        u.LowPart = user_time.dwLowDateTime;
        u.HighPart = user_time.dwHighDateTime;
        double total_seconds = static_cast<double>(k.QuadPart + u.QuadPart) / 10000000.0;
        result = total_seconds * 100.0;
    }
#else
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        result = (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) * 100.0 +
                 (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec) / 10000.0;
    }
#endif
    return result;
}

} // namespace bot_network
