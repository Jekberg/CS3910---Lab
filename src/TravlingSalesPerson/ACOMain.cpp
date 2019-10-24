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

    


    std::vector<std::unique_ptr<std::size_t>> pop(10);
    std::for_each(pop.begin(), pop.end(), [](auto& oPtr){
        auto temp{std::make_unique<std::size_t[]>(16)};
        std::iota(temp.get(), temp.get() + 16, 0);
        oPtr = std::move(temp);s
    });

    for(auto i{0}; i < 1000; ++i)
    {
        std::for_each(pop.begin(), pop.end(), [&](auto& ant)
        {
            Initialise(graph, ant.get(), ant.get() + graph.Count(), 1.0, 1.0);
        });

        CS3910 c{};
        c.Decay(graph);

        std::for_each(pop.begin(), pop.end(), [&](auto& ant)
        {
            auto const length = costOfCycle(graph, ant.get(), ant.get() + graph.Count());
            IncreasePheromone(graph, ant.get(), ant.get() + graph.Count());
        });

    }
}
