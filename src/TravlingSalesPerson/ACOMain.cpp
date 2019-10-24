#include "ACO.h"
#include "Extract.h"
#include "Graph.h"
#include <algorithm>
#include <random>
#include <iostream>

template<typename RandomIt>
void Initialise(AdjacencyMatrix<double>& graph, RandomIt first, RandomIt last);

int main(int argc, char const** argv)
{
    char const* fileName = "sample/ulysses16.csv";
    if(argc == 2)
        fileName = argv[1];
    auto data = ExtractDataFrom<CS3910ExtractTraits>(fileName);
    auto nodeNames{ std::make_unique<std::string_view[]>(data.size())};
    std::transform(data.begin(), data.end(), nodeNames.get(), [](auto&& node)
    {
        return std::string_view{node.id};
    });

    AdjacencyMatrix<double> graph{data.size()};
    for(auto i = data.begin(); i != data.end(); ++i)
        for (auto j = i + 1; j != data.end(); ++j)
            graph(std::distance(data.begin(), i),
                std::distance(data.begin(), j)) =
                std::hypot(i->x - j->x, i->y - j->y);

    

    auto abc{std::make_unique<std::size_t[]>(16)};
    std::iota(abc.get(), abc.get() + 16, 0);
    Initialise(graph, abc.get(), abc.get() + 16);

    std::for_each(abc.get(), abc.get() + 16, [](auto a)
    {
        std::cout << a << '\n';
    });
}


template<typename RandomIt>
void Initialise(AdjacencyMatrix<double>& graph, RandomIt first, RandomIt last)
{
    assert(first != last);

    std::random_device rng{};
    std::uniform_real_distribution<> dist{0.0, 1.0};
    double a = 1;
    double b = 1;

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

    std::pow(Pheromone(graph, 0, 0), a) * std::pow(1/graph(0,0), b);
}

