#ifndef CS390__ACO_H_
#define CS390__ACO_H_

#include "Graph.h"
#include <utility>
#include <cstddef>
#include <random>
#include <memory>

double& Pheromone(AdjacencyMatrix<double>& graph, std::size_t x, std::size_t y)
{
    if(y < x)
        std::swap(x, y);
    return graph(x, y);
}

template<typename RandomIt>
void IncreasePheromone(
    AdjacencyMatrix<double>& graph,
    RandomIt first,
    RandomIt last,
    double amount)
{
    Pheromone(graph, *first, last[-1]) += amount;
    for(; first[1] != last; ++first)
        Pheromone(graph, first[0], first[0]) += amount;
}

template<typename T>
class CS3910
{
public:
    using value_type = std::unique_ptr<std::size_t[]>;

    void Decay(T& graph);

    void Update(T& graph, value_type&);
private:

    double q_;

    std::minstd_rand0 rng_{};
};

template<typename T>
void CS3910<T>::Decay(T& graph)
{
    std::uniform_real_distribution<> distribution{ 0.0, 1.0 };
    for (std::size_t i{}; i < graph.Count(); ++i)
        for (auto j{ i + 1 }; j < graph.Count(); ++j)
            graph(i, j) *= distribution(rng_);
}

template<typename T>
void CS3910<T>::Update(T& graph, value_type& value)
{
    auto length = costOfCycle(graph, nullptr, nullptr);
    IncreasePheromone(graph, value.get(), value.get() + graph.Count(), q_ / length);
}

#endif // !CS390__ACO_H_