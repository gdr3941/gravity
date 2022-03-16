#include "util.h"
#include <random>
#include <vector>
#include <iostream>

namespace util {

float f_rand(float min, float max)
{
    std::random_device r;
    std::default_random_engine gen(r());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

int i_rand(int min, int max)
{
    std::random_device r;
    std::default_random_engine gen(r());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

Timer::Timer() { m_StartTimepoint = std::chrono::high_resolution_clock::now(); }

Timer::~Timer() { Stop(); }

void Timer::Stop()
{
    auto stopTimepoint = std::chrono::high_resolution_clock::now();
    auto start =
        std::chrono::time_point_cast<std::chrono::nanoseconds>(m_StartTimepoint)
            .time_since_epoch();
    auto end =
        std::chrono::time_point_cast<std::chrono::nanoseconds>(stopTimepoint)
            .time_since_epoch();
    auto duration = end - start;
    double us = duration.count() * 0.001;
    std::cout << duration.count() << " ns (" << us << " us)\n";
}

}  // namespace util
