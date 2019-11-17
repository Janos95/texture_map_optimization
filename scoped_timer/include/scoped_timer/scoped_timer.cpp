//
// Created by janos on 19.10.19.
//

#include "scoped_timer.hpp"

#include <mutex>
#include <unordered_map>


std::unordered_map<std::string, ScopedTimer::TimingInfo> ScopedTimer::log_;
std::mutex ScopedTimer::mutex_;