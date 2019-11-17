//
// Created by Janos Meny on 9/4/19.
//

#pragma once

#include <fmt/core.h>

#include <cmath>
#include <chrono>
#include <string>
#include <unordered_map>
#include <mutex>




template<class Ratio>
std::string toSI()
{
    if constexpr(std::is_same_v<std::nano, Ratio>)
        return "nano seconds";
    if constexpr(std::is_same_v<std::micro, Ratio>)
        return "micro seconds";
    if constexpr(std::is_same_v<std::milli, Ratio>)
        return "milli seconds";
    if constexpr(std::is_same_v<std::ratio<1>, Ratio>)
        return "seconds";
    if constexpr(std::is_same_v<std::ratio<60>, Ratio>)
        return "minutes";
    if constexpr(std::is_same_v<std::ratio<3600>, Ratio>)
        return "hours";
}


class ScopedTimer
{
public:
    using my_clock = std::chrono::steady_clock;
    using my_duration = std::chrono::duration<long double, std::nano>;
    using time_point_t = std::chrono::time_point<my_clock>;

    explicit ScopedTimer(std::string name): name_(std::move(name)), start_(my_clock::now()){}

    ~ScopedTimer()
    {
        const auto end = my_clock::now();
        my_duration time = end - start_;

        std::lock_guard l(mutex_);


        auto& [mean, M2, count] = log_[name_];
        ++count;
        auto delta1 = time - mean;
        mean += delta1 / count;
        auto delta2 = time - mean;
        M2 += delta1.count() * delta2.count();
    }

    template<class Ratio = std::ratio<1>>
    static void printStatistics()
    {
        using user_dur = std::chrono::duration<long double, Ratio>;
        std::lock_guard l(mutex_);
        for(const auto& [name, timingInfo]: log_)
        {
            const auto& [mean, M2, count] = timingInfo;
            user_dur standardDeviation = my_duration{std::sqrt(M2/(count - 1))};
            user_dur meanUser = mean;
            auto unit = toSI<Ratio>();
            fmt::print("{3}: Mean {0} {2}, Standard Deviation {1} {2}\n", meanUser.count(), standardDeviation.count(), unit, name);
        }
    }

private:

    struct TimingInfo
    {
        my_duration mean{0};
        long double M2 = 0;
        std::size_t count = 0;
    };

    static std::unordered_map<std::string, TimingInfo> log_;
    static std::mutex mutex_;

    std::string name_;
    time_point_t start_;
};
