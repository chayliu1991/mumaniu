#pragma once

#include "string_piece.h"
#include "time_stamp.h"
#include <sys/types.h>
#include <vector>

namespace ProcessInfo
{
    pid_t pid();
    std::string pid_string();
    uid_t uid();
    std::string user_name();
    uid_t euid();
    Timestamp start_time();
    int clock_ticks_per_second();
    int page_size();
    bool is_debug_build(); // constexpr

    std::string hostname();
    std::string procname();
    StringPiece procname(const std::string &stat);

    /// read /proc/self/status
    std::string proc_status();

    /// read /proc/self/stat
    std::string proc_stat();

    /// read /proc/self/task/tid/stat
    std::string thread_stat();

    /// readlink /proc/self/exe
    std::string exe_path();

    int opened_files();
    int max_open_files();

    struct CpuTime
    {
        double user_seconds;
        double system_seconds;

        CpuTime() : user_seconds(0.0), system_seconds(0.0) {}

        double total() const { return user_seconds + system_seconds; }
    };

    CpuTime cpuTime();

    int numThreads();
    std::vector<pid_t> threads();
} // namespace ProcessInfo
