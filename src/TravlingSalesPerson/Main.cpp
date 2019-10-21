#include "Extract.h"
#include "HillClimb.h"
#include "Graph.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>

template<typename ExtractTraits>
std::vector<typename ExtractTraits::value_type> ReadDataFrom(char const* fileName);

template<typename ClimbPolicy, typename GraphT>
void HillClimb(GraphT& graph, ClimbPolicy& policy);

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

    CS3910HillClimbPolicy<AdjacencyMatrix<double>> hcp{
        nodeNames.get(),
        data.size()};
    HillClimb(graph, hcp);
}

template<typename ClimbPolicy, typename GraphT>
void HillClimb(GraphT& graph, ClimbPolicy& policy)
{
    auto best = policy.StartValue();
    auto path = policy.Create(graph);
    while (!policy.Terminate())
    {
        policy.Initialise(graph, path);

        auto localBest = policy.Step(graph, path);

        if (policy.NewBest(best, localBest))
        {
            std::cout << best << " | ";
            policy.Present(graph, path, std::cout);
            std::cout << '\n';
        }
    }
}