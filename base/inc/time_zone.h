#pragma once

#include <memory>
#include <time.h>

#include "date.h"

class TimeZone
{
public:
    explicit TimeZone(const char *zone_file);
    TimeZone(int east_of_utc, const char *tzname); // a fixed timezone
    TimeZone() = default;                          // an invalid timezone

    // default copy ctor/assignment/dtor are Okay.

    bool valid() const
    {
        // 'explicit operator bool() const' in C++11
        return static_cast<bool>(data_);
    }

    struct tm to_local_time(time_t secondsSinceEpoch) const;
    time_t from_local_time(const struct tm &) const;

    // gmtime(3)
    static struct tm to_utc_time(time_t secondsSinceEpoch, bool yday = false);
    // timegm(3)
    static time_t from_utc_time(const struct tm &);
    // year in [1900..2500], month in [1..12], day in [1..31]
    static time_t from_utc_time(int year, int month, int day, int hour, int minute, int seconds);

    struct Data;

private:
    std::shared_ptr<Data> data_;
};
