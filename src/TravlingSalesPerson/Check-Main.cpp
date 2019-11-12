#include "TravlingSalesman.h"
#include <iostream>

int main(int argc, char const** argv)
{
    auto fileName = "sample/ulysses16.csv";
    if(2 < argc)
        fileName = argv[1];
    else
    {
        std::cout << "Pass at least 2 arguments\n"
            << "The first argument is the TSP data and the following are the "
            << "names of the nodes to check.\n";

        return 0;
    }

    TravlingSalesman<double> tsp{fileName};
    argc -= 2;
    argv += 2;

    std::vector<std::size_t> path{};
    path.reserve(argc);
    for(; argc != 0; --argc, ++argv)
    {
        auto it = std::find_if(
            tsp.Nodes(),
            tsp.Nodes() + tsp.Env().Count(),
            [=](auto const& info){return info.name == *argv;});
        
        if(it == tsp.Nodes() + tsp.Env().Count())
        {
            std::cout << *argv << " Is not a valid node!\n";
            return 0;
        }

        path.push_back(std::distance(tsp.Nodes(), it));
    }

    if (path.size() != tsp.Env().Count()
        || std::unique(path.begin(), path.end()) != path.end())
    {
        std::cout << "Invalid path\n";
        return 0;
    }

    std::cout << CostOf(tsp.Env(), path.begin(), path.end()) << '\n';
}