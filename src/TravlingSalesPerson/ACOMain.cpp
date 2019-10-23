#include "ACO.h"
#include "Extract.h"
#include "Graph.h"

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
}

void Initialise(AdjacencyMatrix& graph, std::unique_ptr<std::size_t>& value)
{
    double a;
    double b;

    std::pow(Pheromone(graph, 0, 0), a) * std::pow(graph(0,0), b);
}

template<typename RandomIt>
void Initialise(AdjacencyMatrix& graph, RandomIt first, RandomIt last, RandomIt cur)
{
    double a;
    double b;



    std::pow(Pheromone(graph, 0, 0), a) * std::pow(1/graph(0,0), b);
}

