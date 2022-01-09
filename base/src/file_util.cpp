#include "file_util.h"

#include "logging.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

FileUtil::AppendFile::AppendFile(StringArg filename) : fp_(::fopen(filename.c_str(), "ae")), // 'e' for O_CLOEXEC
                                                       written_bytes_(0)
{
    assert(fp_);
    ::setbuffer(fp_, buffer_, sizeof(buffer_));
}

FileUtil::AppendFile::~AppendFile()
{
    ::fclose(fp_);
}

void FileUtil::AppendFile::append(const char *logline, const size_t len)
{
    size_t written = 0;

    while (written != len)
    {
        size_t remain = len - written;
        size_t n = write(logline + written, remain);
        if (n != remain)
        {
            int err = ferror(fp_);
            if (err)
            {
                fprintf(stderr, "AppendFile::append() failed %s\n", strerror_tl(err));
                break;
            }
        }
        written += n;
    }

    written_bytes_ += written;
}

FileUtil::ReadSmallFile::ReadSmallFile(StringArg filename) : fd_(::open(filename.c_str(), O_RDONLY | O_CLOEXEC)), err_(0)
{
    buf_[0] = '\0';
    if (fd_ < 0)
    {
        err_ = errno;
    }
}

FileUtil::ReadSmallFile::~ReadSmallFile()
{
    if (fd_ >= 0)
    {
        ::close(fd_); // FIXME: check EINTR
    }
}

// return errno
template <typename String>
int FileUtil::ReadSmallFile::read_to_string(int max_size, String *content, int64_t *file_size, int64_t *modify_time, int64_t *create_time)
{
    static_assert(sizeof(off_t) == 8, "_FILE_OFFSET_BITS = 64");
    assert(content != NULL);
    int err = err_;
    if (fd_ >= 0)
    {
        content->clear();

        if (file_size)
        {
            struct stat statbuf;
            if (::fstat(fd_, &statbuf) == 0)
            {
                if (S_ISREG(statbuf.st_mode))
                {
                    *file_size = statbuf.st_size;
                    content->reserve(static_cast<int>(std::min(static_cast<int64_t>(max_size), *file_size)));
                }
                else if (S_ISDIR(statbuf.st_mode))
                {
                    err = EISDIR;
                }
                if (modify_time)
                {
                    *modify_time = statbuf.st_mtime;
                }
                if (create_time)
                {
                    *create_time = statbuf.st_ctime;
                }
            }
            else
            {
                err = errno;
            }
        }

        while (content->size() < static_cast<size_t>(max_size))
        {
            size_t to_read = std::min(static_cast<size_t>(max_size) - content->size(), sizeof(buf_));
            ssize_t n = ::read(fd_, buf_, to_read);
            if (n > 0)
            {
                content->append(buf_, n);
            }
            else
            {
                if (n < 0)
                {
                    err = errno;
                }
                break;
            }
        }
    }
    return err;
}

int FileUtil::ReadSmallFile::read_to_buffer(int *size)
{
    int err = err_;
    if (fd_ >= 0)
    {
        ssize_t n = ::pread(fd_, buf_, sizeof(buf_) - 1, 0);
        if (n >= 0)
        {
            if (size)
            {
                *size = static_cast<int>(n);
            }
            buf_[n] = '\0';
        }
        else
        {
            err = errno;
        }
    }
    return err;
}

//@ specialization
template int FileUtil::read_file(StringArg filename, int max_size, std::string *content, int64_t *, int64_t *, int64_t *);
template int FileUtil::ReadSmallFile::read_to_string(int maxSize, std::string *content, int64_t *, int64_t *, int64_t *);
