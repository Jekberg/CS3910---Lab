#ifndef GRAPH_H_
#define GRAPH_H_

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

class Graph final
{
public:
    constexpr double Edge(std::size_t nodeA, std::size_t nodeB) const noexcept;

private:
    AdjacencyMatrix<double> repr_;
};

constexpr double Graph::Edge(std::size_t nodeA, std::size_t nodeB)
    const noexcept
{
    return repr_(nodeA, nodeB);
}

#endif // !GRAPH_H_