#ifndef CS3910__EVOLUTION_H_
#define CS3910__EVOLUTION_H_

#include <cassert>
#include <cstddef>
#include <iterator>
#include <random>

template<typename RandomIt, typename RngT>
void Opt2RandomSwap(RandomIt first, RandomIt last, RngT& rng)
{
    auto const Length{ static_cast<std::size_t>(std::distance(first, last)) };
    std::uniform_int_distribution<std::size_t> dis{ 0, Length - 1 };
    std::swap(first[dis(rng)], first[dis(rng)]);
}

template<typename RandomIt>
void Order1Crossover(
    RandomIt firstA,
    RandomIt lastA,
    RandomIt firstB,
    std::size_t offset,
    std::size_t length,
    RandomIt outIt)
{
    auto const Count = static_cast<std::size_t>(std::distance(firstA, lastA));
    auto it = std::rotate_copy(
        firstA,
        firstA + offset,
        lastA,
        outIt);

    auto i = offset + length;
    for (; i < Count && it != outIt + Count; ++i)
        if (std::find(outIt, outIt + length, firstB[i]) == outIt + length)
            *(it++) = firstB[i];

    i %= Count;
    for (; i < offset && it != outIt + Count; ++i)
        if (std::find(outIt, outIt + length, firstB[i]) == outIt + length)
            *(it++) = firstB[i];
}

template<typename RandomIt, typename RngT>
RandomIt SampleGroup(RandomIt first, RandomIt last, std::size_t k, RngT& rng)
{
    // Move k elements selected at random to the front of the range.
    assert(first != last);
    assert(k != 0);
    assert(first + k <= last);

    for (auto i = first; i != first + k; ++i)
    {
        auto c = std::uniform_int_distribution<std::size_t>{
            0,
            static_cast<std::size_t>(std::distance(i, last) - 1) }(rng);
        std::swap(*i, i[c]);
    }

    // The end of the sample group
    return first + k;
}

#endif // !CS3910__EVOLUTION_H_