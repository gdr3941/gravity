#include <random>

namespace util {

float f_rand(float min, float max);

int i_rand(int min, int max);

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
