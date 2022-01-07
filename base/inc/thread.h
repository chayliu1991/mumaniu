#pragma once

#include "atomic.h"
#include "countdown_latch.h"
#include "noncopyable.h"

class Thread : NonCopyable
{
public:
    typedef std::function<void()> ThreadFunc;

    explicit Thread(ThreadFunc, const std::string &name = std::string());

    ~Thread();

    void start();
    int join();
    bool started() const { return started_; }

    pid_t tid() const { return tid_; }
    const std::string &name() const { return name_; }

    static int num_created() { return s_num_reated_.get(); }

private:
    void set_default_name();

    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
    CountDownLatch latch_;

    static AtomicInt32 s_num_reated_;
};