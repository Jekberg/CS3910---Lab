#ifndef CS390__ACO_H_
#define CS390__ACO_H_

#include "CS3910/Graph.h"
#include <algorithm>
#include <utility>
#include <cstddef>
#include <random>
#include <memory>
#include <numeric>

double& Pheromone(AdjacencyMatrix<double>& graph, std::size_t x, std::size_t y)
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
    for(; first + 1 != last; ++first)
        Pheromone(graph, first[0], first[1]) += amount;
}

void Initialise(
    AdjacencyMatrix<double>& graph,
    std::unique_ptr<std::size_t[]>& value,
    double a = 1.0,
    double b = 1.0);

std::minstd_rand0 rng{};

template<typename RandomIt>
void Initialise(
    AdjacencyMatrix<double>& graph,
    RandomIt first,
    RandomIt last,
    double a,
    double b)
{
    assert(first != last);
    auto edgeDesire{std::make_unique<double[]>(graph.Count())};

    // 1 or 2 elements are the same...
    if(first == last)
        return;

    std::swap(*first, first[std::uniform_int_distribution<std::size_t>{0, graph.Count() - 1}(rng)]);
    while(first + 2 != last)
    {
        auto const pivot{*(first++)};
        std::for_each(first, last, [&](auto const next) noexcept
        {
            edgeDesire[next] = std::pow(Pheromone(graph, pivot, next), a)
                * std::pow(Weight(graph, pivot, next), -b);
        });

        auto const total = std::accumulate(first, last, double{},[&](auto total, auto next)
        {
            return total + edgeDesire[next];
        });

        auto r = std::uniform_real_distribution<>{ 0.0, total }(rng);
        for(auto i{first}; i != last; ++i)
            if(total <= (r += edgeDesire[*i]))
            {
                std::swap(*first, first[std::distance(first, i)]);
                break;
            }
    }
}

void Initialise(
    AdjacencyMatrix<double>& graph,
    std::unique_ptr<std::size_t[]>& value,
    double a,
    double b)
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

    double p_ = 0.5;
    double q_ = 1.0;

    std::minstd_rand0 rng_{};
};

template<typename T>
void CS3910<T>::Decay(T& graph)
{
    for (std::size_t i{1}; i < graph.Count(); ++i)
        for (auto j{ i + 1 }; j < graph.Count(); ++j)
            Pheromone(graph, i, j) *= p_;
}

template<typename T>
void CS3910<T>::Update(T& graph, value_type& value)
{
    auto length = costOfCycle(graph, nullptr, nullptr);
    IncreasePheromone(graph, value.get(), value.get() + graph.Count(), q_/length);
}

#endif // !CS390__ACO_H_