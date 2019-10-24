#include "ACO.h"
#include "Extract.h"
#include "Graph.h"
#include <iostream>

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

    // 76.7097
    
    std::vector<std::unique_ptr<std::size_t>> pop(32);
    std::for_each(pop.begin(), pop.end(), [](auto& oPtr){
        auto temp{std::make_unique<std::size_t[]>(16)};
        std::iota(temp.get(), temp.get() + 16, 0);
        oPtr.reset(temp.release());
    });


    for(auto i{0}; i < graph.Count(); ++i)
        for (auto j{ 0 }; j < graph.Count(); ++j)
            Pheromone(graph, i, j) = 0.01;

    double best = std::numeric_limits<double>::infinity();

    for(auto i{0}; i < 1000; ++i)
    {
        std::for_each(pop.begin(), pop.end(), [&](auto& ant)
        {
            Initialise(graph, ant.get(), ant.get() + graph.Count(), 1, 5);
        });

        CS3910<AdjacencyMatrix<double>> c{};
        c.Decay(graph);

        std::for_each(pop.begin(), pop.end(), [&](auto& ant)
        {
            double q = 100.0;
            auto const length = costOfCycle(graph, ant.get(), ant.get() + graph.Count());

            if(length < best)
            {
                best = length;
                std::cout << i << ": " << length;
                std::for_each(ant.get(), ant.get() + graph.Count(), [](auto& aaa)
                {
                    std::cout << ' ' << aaa;
                });
                std::cout << '\n';
            }

            IncreasePheromone(graph, ant.get(), ant.get() + graph.Count(), q/length);
        });

    }
}
