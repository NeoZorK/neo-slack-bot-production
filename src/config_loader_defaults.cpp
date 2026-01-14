#include "config_loader.hpp"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

bool ConfigLoader::create_default_config(const std::string& path) {
    try {
        fs::path config_file(path);
        fs::path config_dir = config_file.parent_path();

        if (!fs::exists(config_dir)) {
            fs::create_directories(config_dir);
        }

        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }

        const char* default_config =
            "{"
            "\"notifications\":{"
                "\"enabled\":true,"
                "\"sound_enabled\":true,"
                "\"dnd_hours\":{"
                    "\"enabled\":false,"
                    "\"start\":\"22:00\","
                    "\"end\":\"08:00\""
                "},"
                "\"channel_filter\":{"
                    "\"mode\":\"all\""
                "}"
            "},"
            "\"connection\":{"
                "\"max_retries\":10,"
                "\"timeout_seconds\":30,"
                "\"ping_interval_seconds\":30,"
                "\"retry_delay_seconds\":1"
            "},"
            "\"events\":{"
                "\"keyword_filter\":{"
                    "\"enabled\":false"
                "},"
                "\"user_filter\":{"
                    "\"mode\":\"all\""
                "}"
            "}"
            "}";

        file << default_config << '\n';
        file.close();

        fs::permissions(config_file, fs::perms::owner_read | fs::perms::owner_write);

        return true;
    } catch (...) {
        return false;
    }
}

