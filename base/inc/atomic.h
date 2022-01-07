#pragma once

#include <stdint.h>

#include "noncopyable.h"

template <typename T>
class AtomicIntegerT : NonCopyable
{
public:
    AtomicIntegerT() : value_(0)
    {
    }

    T get()
    {
        return __sync_val_compare_and_swap(&value_, 0, 0);
    }

    T get_and_add(T x)
    {
        return __sync_fetch_and_add(&value_, x);
    }

    T add_and_get(T x)
    {
        return get_and_add(x) + x;
    }

    T increment_and_get()
    {
        return add_and_get(1);
    }

    T decrement_and_get()
    {
        return add_and_get(-1);
    }

    void add(T x)
    {
        get_and_add(x);
    }

    void increment()
    {
        increment_and_get();
    }

    void decrement()
    {
        decrement_and_get();
    }

    T get_and_set(T newValue)
    {
        return __sync_lock_test_and_set(&value_, newValue);
    }

private:
    volatile T value_;
};

typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;