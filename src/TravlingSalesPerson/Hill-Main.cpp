#include "TravlingSalesman.h"
#include "CS3910/Graph.h"
#include "CS3910/Simulation.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <string>

template<typename T>
class CS3910HillClimbPolicy final : private TravlingSalesman<T>
{
public:
    using value_type = struct
    {
        typename AdjacencyMatrix<T>::value_type cost;
        std::unique_ptr<std::size_t[]> route;
    };

    struct Parameters
    {
        std::size_t iterations;
    };

    explicit CS3910HillClimbPolicy(
        char const* fileName,
        Parameters const& params);

    void Initialise();

    void Step();

    void Complete() noexcept {}

    bool Terminate();
private:

    value_type x_;

    std::minstd_rand0 rng_{};

    std::size_t iteration_{};

    double best_;

    Parameters params_;
};

int main(int argc, char const** argv)
{
    char const* fileName = "sample/ulysses16.csv";
    if(1 < argc)
        fileName = argv[1];
    else
        std::cout << "No input file provided as argument 1\n"
            << "running local optimisation using " << fileName << '\n';

    using HillClimbingPolicy = CS3910HillClimbPolicy<double>;
    typename HillClimbingPolicy::Parameters params{};
    params.iterations = 100000;

    std::cout << "Running...\n";
    Simulate(HillClimbingPolicy{fileName, params});
}

template<typename T>
CS3910HillClimbPolicy<T>::CS3910HillClimbPolicy(
    char const* fileName,
    Parameters const& params)
    : TravlingSalesman<T>{ fileName }
    , params_{params}
{
}

template<typename T>
void CS3910HillClimbPolicy<T>::Initialise()
{
    rng_.seed(std::random_device{}());
    best_ = std::numeric_limits<double>::infinity();
    x_ = {0.0, std::make_unique<std::size_t[]>(this->Env().Count())};
    std::iota(x_.route.get(), x_.route.get() + this->Env().Count(), 0);
}

template<typename T>
void CS3910HillClimbPolicy<T>::Step()
{
    std::size_t bestI;
    std::size_t bestJ;
    std::shuffle(x_.route.get() + 1, x_.route.get() + this->Env().Count(), rng_);

    T localBest = std::numeric_limits<T>::infinity();
    do
    {
        bestI = 0;
        bestJ = 0;
        for(std::size_t i{1}; i < this->Env().Count(); ++i)
            for (std::size_t j{ i + 1 }; j < this->Env().Count(); ++j)
            {
                std::swap(x_.route[i], x_.route[j]);
                x_.cost = CostOf(
                    this->Env(),
                    x_.route.get(),
                    x_.route.get() + this->Env().Count());
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
        this->Show(
            std::cout,
            x_.route.get(),
            x_.route.get() + this->Env().Count());
    }
}

template<typename T>
bool CS3910HillClimbPolicy<T>::Terminate()
{
    return params_.iterations <= iteration_++;
}
