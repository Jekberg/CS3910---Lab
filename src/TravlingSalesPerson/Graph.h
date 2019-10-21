#ifndef CS3910__GRAPH_H_
#define CS3910__GRAPH_H_

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>

template<typename T>
class AdjacencyMatrix final
{
public:
    explicit AdjacencyMatrix(std::size_t count);

    constexpr T& operator()(std::size_t x, std::size_t y) noexcept;

    constexpr T operator()(std::size_t x, std::size_t y) const noexcept;

    constexpr std::size_t Count() const noexcept;
private:
    std::unique_ptr<T[]> data_;

    std::size_t count_;
};

template<typename T>
AdjacencyMatrix<T>::AdjacencyMatrix(std::size_t count)
 : data_{std::make_unique<T[]>(count * count)}
 , count_{count}
{
}

template<typename T>
constexpr T& AdjacencyMatrix<T>::operator()(std::size_t x, std::size_t y)
    noexcept
{
    assert(x < count_ && "The x position must be less than the vertex count.");
    assert(y < count_ && "The y position must be less than the vertex count.");
    return data_.get()[y + count_ * x];
}

template<typename T>
constexpr T AdjacencyMatrix<T>::operator()(std::size_t x, std::size_t y)
    const noexcept
{
    assert(x < count_ && "The x position must be less than the vertex count.");
    assert(y < count_ && "The y position must be less than the vertex count.");
    return data_.get()[y + count_ * x];
}

template<typename T>
constexpr std::size_t AdjacencyMatrix<T>::Count() const noexcept
{
    return count_;
}

template<typename T>
struct Edge
{
    std::size_t from;
    std::size_t to;
    T weight;
};

template<typename T, typename RandomIt>
T costOfCycle(AdjacencyMatrix<T> const& m, RandomIt first, RandomIt last)
{
    assert(first != last && "No empty ranges allowed");
    assert(std::distance(first, last) == m.Count() && "Not all nodes visited");
    assert(std::unique(first, last) == last && "Visiting duplicate nodes");

    T totalCost{ *first > last[-1]
        ? m(last[-1], *first)
        : m(*first, last[-1]) };
    for (; first + 1 != last; ++first)
        totalCost += first[0] > first[1]
            ? m(first[1], first[0])
            : m(first[0], first[1]);

    return totalCost;
}

#endif // !CS3910__GRAPH_H_