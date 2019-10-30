#ifndef CS3910__PHEROMONE_H_
#define CS3910__PHEROMONE_H_

#include "Graph.h"

template<typename T>
T& Pheromone(
    AdjacencyMatrix<T>& graph,
    std::size_t x,
    std::size_t y)
    noexcept
{
    if (x < y)
        std::swap(x, y);
    return graph(x, y);
}

template<typename T>
void DecayPheromone(AdjacencyMatrix<T>& graph, T rate)
{
    for (std::size_t i{ 1 }; i < graph.Count(); ++i)
        for (auto j{ i + 1 }; j < graph.Count(); ++j)
            Pheromone(graph, i, j) *= rate;
}

template<
    typename T,
    typename RandomIt>
void IncreasePheromone(
    AdjacencyMatrix<T>& graph,
    double amount,
    RandomIt first,
    RandomIt last)
    noexcept
{
    Pheromone(graph, *first, last[-1]) += amount;
    for (; first + 1 != last; ++first)
        Pheromone(graph, first[0], first[1]) += amount;
}

#endif // !CS3910__PHEROMONE_H_
