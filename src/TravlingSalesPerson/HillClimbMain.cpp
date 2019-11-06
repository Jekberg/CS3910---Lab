#include "Extract.h"
#include "CS3910/Graph.h"
#include "CS3910/Simulation.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <string>
#include <string_view>

template<typename T>
class CS3910HillClimbPolicy
{
public:
    using value_type = struct
    {
        typename AdjacencyMatrix<T>::value_type cost;
        std::unique_ptr<std::size_t[]> route;
    };

    explicit CS3910HillClimbPolicy(
        AdjacencyMatrix<T>& env,
        std::string_view* nameIndex);

    void Initialise();
    
    void Step();

    void Complete() noexcept {}

    bool Terminate();
private:
    AdjacencyMatrix<T>& env_;
    
    value_type x_;

    std::random_device rng_{};

    double best_;

    std::size_t iteration_{};

    std::string_view* nameIndex_;
};

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

    Simulate(CS3910HillClimbPolicy<double>{graph, nodeNames.get()});
}

template<typename T>
CS3910HillClimbPolicy<T>::CS3910HillClimbPolicy(
    AdjacencyMatrix<T>& env,
    std::string_view* nameIndex)
    : env_{env}
    , nameIndex_{nameIndex}
{
}

template<typename T>
void CS3910HillClimbPolicy<T>::Initialise()
{
    best_ = std::numeric_limits<double>::infinity();
    x_ = {0.0, std::make_unique<std::size_t[]>(env_.Count())};
    std::iota(x_.route.get(), x_.route.get() + env_.Count(), 0);
}

template<typename T>
void CS3910HillClimbPolicy<T>::Step()
{
    std::size_t bestI;
    std::size_t bestJ;
    std::shuffle(x_.route.get() + 1, x_.route.get() + env_.Count(), rng_);
    
    T localBest = std::numeric_limits<T>::infinity();
    do
    {
        bestI = 0;
        bestJ = 0;
        for(std::size_t i{1}; i < env_.Count(); ++i)
            for (std::size_t j{ i + 1 }; j < env_.Count(); ++j)
            {
                std::swap(x_.route[i], x_.route[j]);
                x_.cost = CostOf(
                    env_,
                    x_.route.get(),
                    x_.route.get() + env_.Count());
                if (x_.cost < localBest)
                {
                    localBest = x_.cost;
                    bestI = i;
                    bestJ = j;
                }
                std::swap(x_.route[i], x_.route[j]);
            }

        // Use the best swap
        std::swap(x_.route[bestI], x_.route[bestJ]);
    }
    while(bestI != bestJ);


    if (localBest < best_)
    {
        best_ = localBest;
        std::cout << iteration_;
        std::cout << ": " << best_ << ' ';
        std::cout << '[' << nameIndex_[x_.route[0]];
        std::for_each(
            x_.route.get() + 1,
            x_.route.get() + env_.Count(),
            [&](auto id)
            {
                std::cout << ' ' << nameIndex_[id];
            });

        std::cout << "]\n";
    }
}

template<typename T>
bool CS3910HillClimbPolicy<T>::Terminate()
{
    return 100000 <= iteration_++;
}