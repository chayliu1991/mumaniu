#include "thread.h"

#include <type_traits>

#include <errno.h>
#include <linux/unistd.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

pid_t get_tid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

void after_fork()
{
    CurrentThread::t_cachedT_tid = 0;
    CurrentThread::t_thread_name = "main";
    CurrentThread::tid();
}

class ThreadNameInitializer
{
public:
    ThreadNameInitializer()
    {
        CurrentThread::t_thread_name = "main";
        CurrentThread::tid();
        pthread_atfork(NULL, NULL, &afterFork);
    }
};

ThreadNameInitializer init;

struct ThreadData
{
    typedef muduo::Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    std::string name_;
    pid_t *tid_;
    CountDownLatch *latch_;

    ThreadData(ThreadFunc func, const string &name, pid_t *tid, CountDownLatch *latch) : func_(std::move(func)), name_(name), tid_(tid), latch_(latch)
    {
    }

    void run_in_thread()
    {
        *tid_ = CurrentThread::tid();
        tid_ = NULL;
        latch_->count_down();
        latch_ = NULL;

        CurrentThread::t_thread_name = name_.empty() ? "Thread" : name_.c_str();
        ::prctl(PR_SET_NAME, CurrentThread::t_thread_name);
        try
        {
            func_();
            CurrentThread::t_thread_name = "finished";
        }
        catch (const Exception &ex)
        {
            CurrentThread::t_thread_name = "crashed";
            fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            fprintf(stderr, "stack trace: %s\n", ex.stack_trace());
            abort();
        }
        catch (const std::exception &ex)
        {
            CurrentThread::t_thread_name = "crashed";
            fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            abort();
        }
        catch (...)
        {
            CurrentThread::t_thread_name = "crashed";
            fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
            throw; // rethrow
        }
    }
};

void *start_thread(void *obj)
{
    ThreadData *data = static_cast<ThreadData *>(obj);
    data->run_in_thread();
    delete data;
    return NULL;
}

void CurrentThread::cache_tid()
{
    if (t_cached_tid == 0)
    {
        t_cached_tid = get_tid();
        t_cached_tid = snprintf(t_tid_string, sizeof t_tid_string, "%5d ", t_cached_tid);
    }
}

bool CurrentThread::is_main_thread()
{
    return tid() == ::getpid();
}

void CurrentThread::sleep_usec(int64_t usec)
{
    struct timespec ts = {0, 0};
    ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
    ::nanosleep(&ts, NULL);
}

AtomicInt32 Thread::s_num_created_;

Thread::Thread(ThreadFunc func, const std::string &n) : started_(false), joined_(false), pthread_id_(0), tid_(0), func_(std::move(func)), name_(n), latch_(1)
{
    set_default_name();
}

Thread::~Thread()
{
    if (started_ && !joined_)
    {
        pthread_detach(pthread_id_);
    }
}

void Thread::set_default_name()
{
    int num = s_num_reated_.increment_and_get();
    if (name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d", num);
        name_ = buf;
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;

    ThreadData *data = new ThreadData(func_, name_, &tid_, &latch_);
    if (pthread_create(&pthread_id_, NULL, &start_thread, data))
    {
        started_ = false;
        delete data; // or no delete?
        LOG_SYSFATAL << "Failed in pthread_create";
    }
    else
    {
        latch_.wait();
        assert(tid_ > 0);
    }
}

int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}