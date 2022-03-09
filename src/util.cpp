#include "util.h"
#include <random>
#include <vector>

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

}  // namespace util
