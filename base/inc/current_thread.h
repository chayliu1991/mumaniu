#pragma once

#include <stdint.h>
#include <string>

namespace CurrentThread
{
    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread const char *t_threadName;
    void cache_tid();

    inline int tid()
    {
        if (__builtin_expect(t_cachedTid == 0, 0))
        {
            cache_tid();
        }
        return t_cachedTid;
    }

    inline const char *tid_string()
    {
        return t_tidString;
    }

    inline int tid_string_length()
    {
        return t_tidStringLength;
    }

    inline const char *name()
    {
        return t_threadName;
    }

    bool is_main_thread();

    void sleepUsec(int64_t usec);

    std::string stack_trace(bool demangle);
}