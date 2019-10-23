#ifndef CS390__ACO_H_
#define CS390__ACO_H_

#include "Graph.h"
#include <utility>
#include <random>

double& Pheromone(AdjacencyMatrix<double>& graph, std::size_t x, std::size_t y)
{
    if(y < x)
        std::swap(x, y);
    return graph(x, y);
}

template<typename T>
class CS3910
{
public:
    void Decay(T& graph);
private:

    std::minstd_rand0 rng_{};
};

template<typename T>
void CS3910<T>::Decay(T& graph)
{
    std::uniform_real_distribution<> distribution{0.0, 1.0};
    for(std::size_t i{}; i < graph.Count(); ++i)
        for(auto j{i + 1}; j < graph.Count(); ++j)
            graph(i, j) += distribution(rng_);
}

#endif // !CS390__ACO_H_