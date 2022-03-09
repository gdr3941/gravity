#include <random>

namespace util {

float f_rand(float min, float max);

int i_rand(int min, int max);

template <class T, class BinaryOp>
void for_distinct_pairs(T& container, BinaryOp&& f)
{
    for (auto i = begin(container); i < (end(container) - 1); ++i) {
        for (auto j = i + 1; j < end(container); ++j) {
            f(*i, *j);
        }
    }
}

}  // namespace util
