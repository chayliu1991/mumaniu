#pragma once

#include "mutex.h"
#include <pthread.h>

class Condition : NonCopyable
{
public:
    explicit Condition(MutexLock &mutex) : mutex_(mutex)
    {
        MCHECK(pthread_cond_init(&pcond_, NULL));
    }

    ~Condition()
    {
        MCHECK(pthread_cond_destroy(&pcond_));
    }

    void wait()
    {
        MutexLock::UnassignGuard ug(mutex_);
        MCHECK(pthread_cond_wait(&pcond_, mutex_.get_pthread_mutex()));
    }

    bool wait_for_seconds(double seconds);

    void notify()
    {
        MCHECK(pthread_cond_signal(&pcond_));
    }

    void notify_all()
    {
        MCHECK(pthread_cond_broadcast(&pcond_));
    }

private:
    MutexLock &mutex_;
    pthread_cond_t pcond_;
};