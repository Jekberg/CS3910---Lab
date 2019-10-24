#ifndef CS390__ACO_H_
#define CS390__ACO_H_

#include "Graph.h"
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

    auto capacity = std::distance(first, last) - 1;
    auto xs{std::make_unique<double[]>(capacity)};
    auto xFirst = xs.get();
    auto const xLast = xs.get() + capacity;

    while(first + 1 != last)
    {
        auto current{*first};
        std::transform(first + 1, last, xFirst, [&](auto& x)
        {
            //std::cout << Pheromone(graph, current, x) << '\n';
            return std::pow(Pheromone(graph, current, x), a)
                * std::pow(1/Weight(graph, current, x), b);
        });

        //std::cout << "ARRR ";
        //std::for_each(xFirst, xLast, [](auto w)
        //    {
        //        std::cout << w << ' ';
        //    });
        //std::cout << "\n";

        for(auto i = xFirst + 1; i != xLast; ++i)
            *i += i[-1];


        // Pick the next element to visit at random and then
        // place it as next in the visit list.
        auto const r = std::uniform_real_distribution<>{ 0.0, xLast[-1] }(rng);
        auto xIt = std::find_if(xFirst, xLast, [=](auto val)
        {
            return r <= val;
        });

        ++first;

        //std::cout << "ACCC ";
        //std::for_each(xFirst, xLast, [](auto w)
        //    {
        //        std::cout << w << ' ';
        //    });
        //std::cout << "\n";
        //std::cout << "R = " << r << '\n';
        //std::cout << "DISTANCE = " << std::distance(xFirst, xIt) << '\n';
        std::swap(*first, first[std::distance(xFirst, xIt)]);
        ++xFirst;
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
    for (std::size_t i{}; i < graph.Count(); ++i)
        for (auto j{ i + 1 }; j < graph.Count(); ++j)
            Pheromone(graph, i, j) *= p_;
}

template<typename T>
void CS3910<T>::Update(T& graph, value_type& value)
{
    auto length = costOfCycle(graph, nullptr, nullptr);
    IncreasePheromone(graph, value.get(), value.get() + graph.Count(), q_ / length);
}

#endif // !CS390__ACO_H_