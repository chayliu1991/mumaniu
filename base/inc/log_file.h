#pragma once

#include "mutex.h"
#include "noncopyable.h"
#include "string_piece.h"

#include <memory>
#include <sys/types.h>

class LogFile : NonCopyable
{
public:
    LogFile(const std::string &basename, off_t rollSize, bool threadSafe = true, int flushInterval = 3, int checkEveryN = 1024);
    ~LogFile();

    void append(const char *logline, int len);
    void flush();
    bool rollFile();

private:
    void append_unlocked(const char *logline, int len);

    static std::string get_log_file_name(const std::string &basename, time_t *now);

    const std::string basename_;
    const off_t roll_size_;
    const int flush_interval_;
    const int check_every_n_;

    int count_;

    std::unique_ptr<MutexLock> mutex_;
    time_t start_of_period_;
    time_t last_roll_;
    time_t last_flush_;
    std::unique_ptr<FileUtil::AppendFile> file_;

    const static int kRollPerSeconds_ = 60 * 60 * 24;
};