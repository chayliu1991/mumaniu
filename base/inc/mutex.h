#pragma once
#include <assert.h>
#include <memory>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>

#include "current_thread.h"
#include "noncopyable.h"

#ifdef CHECK_PTHREAD_RETURN_VALUE

#ifdef NDEBUG
__BEGIN_DECLS
extern void __assert_perror_fail(int errnum,
                                 const char *file,
                                 unsigned int line,
                                 const char *function) noexcept __attribute__((__noreturn__));
__END_DECLS
#endif

#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret);         \
                       if (__builtin_expect(errnum != 0, 0))    \
                         __assert_perror_fail (errnum, __FILE__, __LINE__, __func__); })

#else
#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret);         \
                       assert(errnum == 0); (void) errnum; })
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MutexLock : NonCopyable
{
public:
    MutexLock() : holder_(0)
    {
        MCHECK(pthread_mutex_init(&mutex_, NULL));
    }

    ~MutexLock()
    {
        assert(holder_ == 0);
        MCHECK(pthread_mutex_destroy(&mutex_));
    }

    bool is_locked_by_this_thread() const
    {
        return holder_ == CurrentThread::tid();
    }

    void assert_locked()
    {
        assert(is_locked_by_this_thread());
    }

    void lock()
    {
        MCHECK(pthread_mutex_lock(&mutex_));
        assign_holder();
    }

    void unlock()
    {
        unassign_holder();
        MCHECK(pthread_mutex_unlock(&mutex_));
    }

    pthread_mutex_t *get_pthread_mutex()
    {
        return &mutex_;
    }

private:
    friend class Condition;

    class UnassignGuard : NonCopyable
    {
    public:
        explicit UnassignGuard(MutexLock &owner) : owner_(owner)
        {
            owner_.unassign_holder();
        }

        ~UnassignGuard()
        {
            owner_.assign_holder();
        }

    private:
        MutexLock &owner_;
    };

    void unassign_holder()
    {
        holder_ = 0;
    }

    void assign_holder()
    {
        holder_ = CurrentThread::tid();
    }

    pthread_mutex_t mutex_;
    pid_t holder_;
};

class MutexLockGuard : NonCopyable
{
public:
    explicit MutexLockGuard(MutexLock &mutex) : mutex_(mutex)
    {
        mutex_.lock();
    }

    ~MutexLockGuard()
    {
        mutex_.unlock();
    }

private:
    MutexLock &mutex_;
};

#define MutexLockGuard(x) error "Missing guard object name"