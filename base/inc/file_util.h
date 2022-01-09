#pragma once

#include "noncopyable.h"
#include "string_piece.h"
#include <sys/types.h> // for off_t

namespace FileUtil
{
    class ReadSmallFile : NonCopyable
    {
    public:
        ReadSmallFile(StringArg filename);
        ~ReadSmallFile();

        template <typename String>
        int read_to_string(int max_size, String *content, int64_t *file_size, int64_t *modify_time, int64_t *create_time);
        int read_to_buffer(int *size);

        const char *buffer() const { return buf_; }

        static const int kBufferSize = 64 * 1024;

    private:
        int fd_;
        int err_;
        char buf_[kBufferSize];
    };

    template <typename String>
    int read_file(StringArg filename, int max_size, String *content, int64_t *file_size = NULL, int64_t *modify_time = NULL, int64_t *create_time = NULL)
    {
        ReadSmallFile file(filename);
        return file.readToString(max_size, content, file_size, modify_time, create_time);
    }

    class AppendFile : NonCopyable
    {
    public:
        explicit AppendFile(StringArg filename);

        ~AppendFile();

        void append(const char *logline, size_t len);

        void flush();

        off_t written_bytes() const { return written_bytes_; }

    private:
        size_t write(const char *logline, size_t len);

        FILE *fp_;
        char buffer_[64 * 1024];
        off_t written_bytes_;
    };
}