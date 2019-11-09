#include "TravlingSalesman.h"
#include "CS3910/Evolution.h"
#include "CS3910/Simulation.h"
#include "CS3910/Graph.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <random>

template<typename T>
struct CS3910EvolutionPolicy : private TravlingSalesman<T>
{
public:
    using value_type = struct
    {
        T cost;
        std::unique_ptr<std::size_t[]> route;
    };

    struct Parameters
    {
        std::size_t k;
        std::size_t populationSize;
        std::size_t eliteSize;
        std::size_t iterations;
        double randomGenerationProbabillity;
        double mutationProbabillity;
    };

    explicit CS3910EvolutionPolicy(
        char const* fileName,
        Parameters const& params);

    void Initialise();

    void Step();

    void Complete(){}

    bool Terminate();
private:

    template<typename RandomIt>
    struct Selection
    {
        value_type& firstParent;
        value_type& secondParent;
        RandomIt nextIterator;
    };

    Parameters params_;

    std::unique_ptr<value_type[]> population_;

    double best_;

    std::size_t iteration_;

    std::minstd_rand rng_{};

    template<typename RandomIt>
    Selection<RandomIt> Select(
        RandomIt first,
        RandomIt last)
    {
        if(first + params_.k != last)
        {
            // Find the first parent
            auto it = SampleGroup(first, last, params_.k, rng_);
            auto minIt = std::min_element(
                first,
                it,
                [](auto& a, auto& b){ return a.cost < b.cost; });
            std::swap(first[0], *minIt);

            auto& parentA = first[0];

            // Find the second parent
            it = SampleGroup(first + 1, last, params_.k, rng_);
            minIt = std::min_element(
                first + 1,
                it,
                [](auto& a, auto& b)
                {
                    return a.cost < b.cost;
                });
            std::swap(first[1], *minIt);
        }

        return Selection<RandomIt>{first[0], first[1], first + 2};
    }

    std::pair<value_type, value_type> Crossover(
        value_type& parentA,
        value_type& parentB)
    {
        value_type tempA {
            0.0,
            std::make_unique<std::size_t[]>(this->Env().Count())};

        std::uniform_int_distribution<std::size_t> d{
            0,
            this->Env().Count() - 1 };
        auto const Offset = d(rng_);
        auto const Length = d(rng_);

        std::uniform_real_distribution<> realDis{0, 100};

        Order1Crossover(
            parentA.route.get(),
            parentA.route.get() + this->Env().Count(),
            parentB.route.get(),
            Offset,
            Length,
            tempA.route.get());

        if(realDis(rng_) <= params_.randomGenerationProbabillity)
            std::shuffle(
                tempA.route.get(),
                tempA.route.get() + this->Env().Count(),
                rng_);

        value_type tempB{
            0.0,
            std::make_unique<std::size_t[]>(this->Env().Count())};
        Order1Crossover(
            parentB.route.get(),
            parentB.route.get() + this->Env().Count(),
            parentA.route.get(),
            Offset,
            Length,
            tempB.route.get());

        if (realDis(rng_) <= params_.randomGenerationProbabillity)
            std::shuffle(
                tempB.route.get(),
                tempB.route.get() + this->Env().Count(),
                rng_);

        return {std::move(tempA), std::move(tempB)};
    }

    void Mutate(value_type& value)
    {
        std::uniform_real_distribution<> dis{0.0, 100.0};
        if(dis(rng_) <= params_.mutationProbabillity)
            Opt2RandomSwap(
                value.route.get(),
                value.route.get() + this->Env().Count(),
                rng_);
    }

    void Evaluate(value_type& value)
    {
        value.cost =  CostOf(
            this->Env(),
            value.route.get(),
            value.route.get() + this->Env().Count());
    }

    template<typename RandomIt>
    void SelectNext(
        RandomIt first,
        RandomIt last)
    {
        auto const PopulationEnd = population_.get() + params_.populationSize;
        for(auto i = population_.get(); i != population_.get() + params_.eliteSize; ++i)
        {
            auto it = Roulette(
                i,
                PopulationEnd,
                rng_,
                [](auto& path){return 1 / path.cost;});
            std::swap(*i, *it);
        }

        MoveRandom(first, last, population_.get() + params_.eliteSize, PopulationEnd, rng_);
    }
};

int main(int argc, char const** argv)
{
    char const* fileName = "sample/ulysses16.csv";
    if(1 < argc)
        fileName = argv[1];
    else
        std::cout << "No input file provided as argument 1\n"
            << "running the evolutionary algorithm using " << fileName << '\n';

    using EvolutionPolicy = CS3910EvolutionPolicy<double>;
    typename EvolutionPolicy::Parameters params{};
    params.k = 2;
    params.populationSize = 100;
    params.eliteSize = 99; // Stable
    params.iterations = 100000;
    params.randomGenerationProbabillity = 5;
    params.mutationProbabillity = 70;

    std::cout << "Running...\n";
    Simulate(EvolutionPolicy{fileName, params});
}

template<typename T>
CS3910EvolutionPolicy<T>::CS3910EvolutionPolicy(
    char const* fileName,
    Parameters const& params)
    : TravlingSalesman<T>{ fileName }
    , params_{params}
{
}

template<typename T>
void CS3910EvolutionPolicy<T>::Initialise()
{
    best_ = std::numeric_limits<double>::infinity();
    iteration_ = 0;
    population_ = std::make_unique<value_type[]>(params_.populationSize);

    rng_.seed(std::random_device{}());

    std::generate(
        population_.get(),
        population_.get() + params_.populationSize,
        [&]()
        {
            value_type temp{
                0.0,
                std::make_unique<std::size_t[]>(this->Env().Count())};
            std::iota(
                temp.route.get(),
                temp.route.get() + this->Env().Count(),
                0);
            std::shuffle(
                temp.route.get(),
                temp.route.get() + this->Env().Count(),
                rng_);
            temp.cost = CostOf(
                this->Env(),
                temp.route.get(),
                temp.route.get() + this->Env().Count());
            return temp;
        });
}

template<typename T>
void CS3910EvolutionPolicy<T>::Step()
{
    std::vector<value_type> nextGen{};
    for (auto it{ population_.get() }; it != population_.get() + params_.populationSize;)
    {
        auto [parentA, parentB, next] = Select(it, population_.get() + params_.populationSize);
        it = next;

        auto [childA, childB] = Crossover(parentA, parentB);
        Mutate(childA);
        Mutate(childB);

        Evaluate(childA);
        Evaluate(childB);

        nextGen.emplace_back(std::move(childA));
        nextGen.emplace_back(std::move(childB));
    }

    SelectNext(nextGen.begin(), nextGen.end());

    auto it = std::min_element(
        population_.get(),
        population_.get() + params_.populationSize,
        [](auto& a, auto& b)
        {
            return a.cost < b.cost;
        });

    if (it != population_.get() + params_.populationSize && it->cost < best_)
    {
        best_ = it->cost;
        std::cout << iteration_ << ": " << it->cost << " ";
        this->Show(
            std::cout,
            it->route.get(),
            it->route.get() + this->Env().Count());
    }
}

template<typename T>
bool CS3910EvolutionPolicy<T>::Terminate()
{
    return params_.iterations < iteration_++;
}
