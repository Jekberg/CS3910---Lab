#ifndef CS3910__MUTATION_H_
#define CS3910__MUTATION_H_

#include <iterator>
#include <random>

template<typename RandomIt, typename RngT>
void Opt2RandomSwap(RandomIt first, RandomIt last, RngT& rng)
{
    auto const Length {static_cast<std::size_t>(std::distance(first, last))};
    std::uniform_int_distribution<std::size_t> dis{0, Length - 1};
    std::swap(first[dis(rng)], first[dis(rng)]);
}


#endif // !CS3910__MUTATION_H_