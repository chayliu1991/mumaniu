#include "log_file.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

LogFile::LogFile(const std::string &basename, off_t roll_size, bool thread_safe, int flush_interval, int check_every_n)
    : basename_(basename), roll_size_(roll_size), flush_interval_(flush_interval), check_every_n_(check_every_n),
      count_(0), mutex_(thread_safe ? new MutexLock : NULL), start_of_period_(0), last_roll_(0), last_flush_(0)
{
    assert(basename.find('/') == string::npos);
    rollFile();
}

LogFile::~LogFile() = default;

void LogFile::append(const char *logline, int len)
{
    if (mutex_)
    {
        MutexLockGuard lock(*mutex_);
        append_unlocked(logline, len);
    }
    else
    {
        append_unlocked(logline, len);
    }
}

void LogFile::flush()
{
    if (mutex_)
    {
        MutexLockGuard lock(*mutex_);
        file_->flush();
    }
    else
    {
        file_->flush();
    }
}

void LogFile::append_unlocked(const char *logline, int len)
{
    file_->append(logline, len);

    if (file_->written_bytes() > roll_size_)
    {
        rollFile();
    }
    else
    {
        ++count_;
        if (count_ >= check_every_n_)
        {
            count_ = 0;
            time_t now = ::time(NULL);
            time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;
            if (thisPeriod_ != start_of_period_)
            {
                rollFile();
            }
            else if (now - last_flush_ > flush_interval_)
            {
                last_flush_ = now;
                file_->flush();
            }
        }
    }
}

bool LogFile::rollFile()
{
    time_t now = 0;
    std::string filename = getLogFileName(basename_, &now);
    time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

    if (now > last_roll_)
    {
        last_roll_ = now;
        last_flush_ = now;
        start_of_period_ = start;
        file_.reset(new FileUtil::AppendFile(filename));
        return true;
    }
    return false;
}

std::string LogFile::get_log_file_name(const std::string &basename, time_t *now)
{
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm;
    *now = time(NULL);
    gmtime_r(now, &tm);
    strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;

    filename += ProcessInfo::hostname();

    char pidbuf[32];
    snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
    filename += pidbuf;

    filename += ".log";

    return filename;
}