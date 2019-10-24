#ifndef CS390__ACO_H_
#define CS390__ACO_H_

#include "Graph.h"
#include <algorithm>
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

double& Weight(AdjacencyMatrix<double>& graph, std::size_t x, std::size_t y)
{
    if(x < y)
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

void Initialise(
    AdjacencyMatrix<double>& graph,
    std::unique_ptr<std::size_t[]>& value,
    double a = 1.0,
    double b = 1.0);

template<typename RandomIt>
void Initialise(
    AdjacencyMatrix<double>& graph,
    RandomIt first,
    RandomIt last,
    double a = 1.0,
    double b = 1.0)
{
    assert(first != last);

    std::random_device rng{};
    std::uniform_real_distribution<> dist{0.0, 1.0};

    auto capacity = std::distance(first, last) - 1;
    auto xs{std::make_unique<double[]>(capacity)};
    auto xFirst = xs.get();
    auto const xLast = xs.get() + capacity;

    while(first + 1 != last)
    {
        auto current{*first};
        std::transform(first + 1, last, xFirst, [&](auto&& x)
        {
            return std::pow(Pheromone(graph, current, x), a)
                * std::pow(1/graph(current, x), b);
        });

        // Make the array into a cummulative array
        std::transform(
            xFirst, xLast - 1,
            xFirst + 1, xFirst + 1, std::plus<double>{});


        // Pick the next element to visit at random and then
        // place it as next in the visit list.
        auto const r = dist(rng) * xLast[-1];
        auto xIt = std::find_if(xFirst, xLast, [=](auto val)
        {
            return r <= val;
        });

        ++first;
        std::swap(*first, first[std::distance(xFirst, xIt)]);
        ++xFirst;
    }
}

void Initialise(
    AdjacencyMatrix<double>& graph,
    std::unique_ptr<std::size_t[]>& value,
    double a = 1.0,
    double b = 1.0)
{
    Initialise(graph, value.get(), value.get() + graph.Count(), a, b);
}

template<typename T>
class CS3910
{
public:

    explicit CS3910() = default;

    using value_type = std::unique_ptr<std::size_t[]>;

    void Decay(T& graph);

    void Update(T& graph, value_type&);
private:

    double q_ = 1.0;

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