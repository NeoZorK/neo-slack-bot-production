#include <atomic>

// Global running flag for examples and tests that don't define their own
std::atomic<bool> g_running(true);
