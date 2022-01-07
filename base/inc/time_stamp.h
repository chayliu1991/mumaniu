#pragma once

#include <algorithm>
#include <stdint.h>

class Timestamp
{
public:
    Timestamp() : micro_seconds_since_epoch_(0)
    {
    }

    explicit Timestamp(int64_t micro_seconds_since_epoch_arg) : micro_seconds_since_epoch_(micro_seconds_since_epoch_arg)
    {
    }

    void swap(Timestamp &that)
    {
        std::swap(micro_seconds_since_epoch_, that.micro_seconds_since_epoch_);
    }

    std::string to_string() const;
    std::string to_formatted_string(bool show_micro_seconds = true) const;

    bool valid() const { return micro_seconds_since_epoch_ > 0; }

    int64_t micro_seconds_since_epoch() const { return micro_seconds_since_epoch_; }
    time_t seconds_since_epoch() const
    {
        return static_cast<time_t>(micro_seconds_since_epoch_ / kMicroSecondsPerSecond);
    }

    static Timestamp now();
    static Timestamp invalid()
    {
        return Timestamp();
    }

    static Timestamp from_unix_time(time_t t)
    {
        return from_unix_time(t, 0);
    }

    static Timestamp from_unix_time(time_t t, int micro_seconds)
    {
        return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + micro_seconds);
    }

    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t micro_seconds_since_epoch_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
    return lhs.micro_seconds_since_epoch() < rhs.micro_seconds_since_epoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
    return lhs.micro_seconds_since_epoch() == rhs.micro_seconds_since_epoch();
}

inline double timeDifference(Timestamp high, Timestamp low)
{
    int64_t diff = high.micro_seconds_since_epoch() - low.micro_seconds_since_epoch();
    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

inline Timestamp add_time(Timestamp timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.micro_seconds_since_epoch() + delta);
}