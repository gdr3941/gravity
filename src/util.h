#include <random>
#include <chrono>
#include <iostream>

namespace util {

float f_rand(float min, float max);

int i_rand(int min, int max);

// Performance timer that measures from object creation to destruction
class Timer {
public:
    Timer() { m_StartTimepoint = std::chrono::high_resolution_clock::now(); }

    ~Timer() { Stop(); }

    void Stop()
    {
        auto stopTimepoint = std::chrono::high_resolution_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_StartTimepoint).time_since_epoch();
        auto end = std::chrono::time_point_cast<std::chrono::nanoseconds>(stopTimepoint).time_since_epoch();
        auto duration = end - start;
        double us = duration.count() * 0.001;
        std::cout << duration.count() << " ns (" << us << " us)\n";
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
};

/// calls f for all distinct pairs from collection
/// ex: {1,2,3} => (1,2) (1,3) (2,3)
template <class Iterable, class BinaryOp>
void for_distinct_pairs(Iterable& container, BinaryOp&& f)
{
    for (auto i = begin(container); i < (end(container) - 1); ++i) {
        for (auto j = i + 1; j < end(container); ++j) {
            f(*i, *j);
        }
    }
}

}  // namespace util
