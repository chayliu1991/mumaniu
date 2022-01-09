#pragma once

#include "noncopyable.h"
#include "string_piece.h"
#include <assert.h>
#include <string.h>

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template <int SIZE>
class FixedBuffer : NonCopyable
{
public:
    FixedBuffer() : cur_(data_)
    {
        set_cookie(cookie_start);
    }

    ~FixedBuffer()
    {
        set_cookie(cookie_end);
    }

    void append(const char *buf, size_t len)
    {
        if (static_cast<size_t>(avail()) > len)
        {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

    const char *data() const { return data_; }
    int length() const { return static_cast<int>(cur_ - data_); }

    char *current() { return cur_; }
    int avail() const { return static_cast<int>(end() - cur_); }
    void add(size_t len) { cur_ += len; }

    void reset() { cur_ = data_; }
    void bzero()
    {
        memset(data_, 0, sizeof(data_));
    }

    const char *debug_string();
    void set_cookie(void (*cookie)()) { cookie_ = cookie; }

    std::string to_string() const { return string(data_, length()); }
    StringPiece to_string_piece() const { return StringPiece(data_, length()); }

private:
    const char *end() const { return data_ + sizeof(data_); }
    static void cookie_start();
    static void cookie_end();

    void (*cookie_)();
    char data_[SIZE];
    char *cur_;
};

class LogStream : NonCopyable
{
    typedef LogStream self;

public:
    typedef FixedBuffer<kSmallBuffer> Buffer;

    self &operator<<(bool v)
    {
        buffer_.append(v ? "1" : "0", 1);
        return *this;
    }

    self &operator<<(short);
    self &operator<<(unsigned short);
    self &operator<<(int);
    self &operator<<(unsigned int);
    self &operator<<(long);
    self &operator<<(unsigned long);
    self &operator<<(long long);
    self &operator<<(unsigned long long);
    self &operator<<(const void *);

    self &operator<<(float v)
    {
        *this << static_cast<double>(v);
        return *this;
    }

    self &operator<<(double);

    self &operator<<(char v)
    {
        buffer_.append(&v, 1);
        return *this;
    }

    self &operator<<(const char *str)
    {
        if (str)
        {
            buffer_.append(str, strlen(str));
        }
        else
        {
            buffer_.append("(null)", 6);
        }
        return *this;
    }

    self &operator<<(const unsigned char *str)
    {
        return operator<<(reinterpret_cast<const char *>(str));
    }

    self &operator<<(const std::string &v)
    {
        buffer_.append(v.c_str(), v.size());
        return *this;
    }

    self &operator<<(const StringPiece &v)
    {
        buffer_.append(v.data(), v.size());
        return *this;
    }

    self &operator<<(const Buffer &v)
    {
        *this << v.to_string_piece();
        return *this;
    }

    void append(const char *data, int len) { buffer_.append(data, len); }
    const Buffer &buffer() const { return buffer_; }
    void resetBuffer() { buffer_.reset(); }

private:
    void static_check();

    template <typename T>
    void format_integer(T);

    Buffer buffer_;

    static const int kMaxNumericSize = 48;
};

class Fmt
{
public:
    template <typename T>
    Fmt(const char *fmt, T val);

    const char *data() const { return buf_; }
    int length() const { return length_; }

private:
    char buf_[32];
    int length_;
};

inline LogStream &operator<<(LogStream &s, const Fmt &fmt)
{
    s.append(fmt.data(), fmt.length());
    return s;
}

std::string format_si(int64_t n);

std::string format_iec(int64_t n);