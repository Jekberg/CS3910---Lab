#include "TravlingSalesman.h"
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
class CS3910HillClimbPolicy final : private TravlingSalesman<T>
{
public:
    using value_type = struct
    {
        typename AdjacencyMatrix<T>::value_type cost;
        std::unique_ptr<std::size_t[]> route;
    };

    explicit CS3910HillClimbPolicy(char const* fileName)
        : TravlingSalesman{ fileName }
    {
    }

    void Initialise();
    
    void Step();

    void Complete() noexcept {}

    bool Terminate();
private:
    
    value_type x_;

    std::random_device rng_{};

    double best_;

    std::size_t iteration_{};
};

int main(int argc, char const** argv)
{
    char const* fileName = "sample/ulysses16.csv";
    if(argc == 2)
        fileName = argv[1];

    Simulate(CS3910HillClimbPolicy<double>{fileName});
}

template<typename T>
void CS3910HillClimbPolicy<T>::Initialise()
{
    best_ = std::numeric_limits<double>::infinity();
    x_ = {0.0, std::make_unique<std::size_t[]>(Env().Count())};
    std::iota(x_.route.get(), x_.route.get() + Env().Count(), 0);
}

template<typename T>
void CS3910HillClimbPolicy<T>::Step()
{
    std::size_t bestI;
    std::size_t bestJ;
    std::shuffle(x_.route.get() + 1, x_.route.get() + Env().Count(), rng_);
    
    T localBest = std::numeric_limits<T>::infinity();
    do
    {
        bestI = 0;
        bestJ = 0;
        for(std::size_t i{1}; i < Env().Count(); ++i)
            for (std::size_t j{ i + 1 }; j < Env().Count(); ++j)
            {
                std::swap(x_.route[i], x_.route[j]);
                x_.cost = CostOf(
                    Env(),
                    x_.route.get(),
                    x_.route.get() + Env().Count());
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
        std::cout << iteration_ << ": " << best_ << ' ';
        Show(std::cout, x_.route.get(), x_.route.get() + Env().Count());
    }
}

template<typename T>
bool CS3910HillClimbPolicy<T>::Terminate()
{
    return 100000 <= iteration_++;
}