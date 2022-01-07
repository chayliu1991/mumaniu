#include "exception.h"

#include "current_thread.h"

Exception::Exception(std::string msg) : message_(std::move(msg)), stack_(CurrentThread::stack_trace(/*demangle=*/false))
{
}
