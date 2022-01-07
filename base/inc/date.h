#pragma once

#include <algorithm>

class Date
{
public:
    struct YearMonthDay
    {
        int year;  // [1900..2500]
        int month; // [1..12]
        int day;   // [1..31]
    };

    static const int kDaysPerWeek = 7;
    static const int kJulianDayOf1970_01_01;

    Date() : julian_day_number_(0)
    {
    }

    Date(int year, int month, int day);

    explicit Date(int julian_day_num) : julian_day_number_(julian_day_num)
    {
    }

    explicit Date(const struct tm &);

    void swap(Date &that)
    {
        std::swap(julian_day_number_, that.julian_day_number_);
    }

    bool valid() const { return julian_day_number_ > 0; }

    std::string to_iso_string() const;

    struct YearMonthDay year_month_day() const;

    int year() const
    {
        return year_month_day().year;
    }

    int month() const
    {
        return year_month_day().month;
    }

    int day() const
    {
        return year_month_day().day;
    }

    int week_day() const
    {
        return (julian_day_number_ + 1) % kDaysPerWeek;
    }

    int julian_day_number() const { return julian_day_number_; }

private:
    int julian_day_number_;
};

inline bool operator<(Date x, Date y)
{
    return x.julian_day_number() < y.julian_day_number();
}

inline bool operator==(Date x, Date y)
{
    return x.julian_day_number() == y.julian_day_number();
}
