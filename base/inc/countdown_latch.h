#pragma once

#include "condition.h"
#include "mutex.h"
#include "noncopyable.h"

class CountDownLatch : NonCopyable
{
public:
    explicit CountDownLatch(int count);

    void wait();

    void count_down();

    int get_count() const;

private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};
