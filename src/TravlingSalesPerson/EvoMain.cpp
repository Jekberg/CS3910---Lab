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

    explicit CS3910EvolutionPolicy(char const* fileName)
        : TravlingSalesman{ fileName }
    {
    }

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

    constexpr static std::size_t K = 2;

    constexpr static double MutationProbabillity = 70.0;

    std::size_t populationSize_;

    std::unique_ptr<value_type[]> population_;

    double best_;

    std::size_t iteration_;

    std::minstd_rand0 rng_{};

    template<typename RandomIt>
    Selection<RandomIt> Select(
        RandomIt first,
        RandomIt last)
    {
        if(first + K != last)
        { 
            // Find the first parent
            auto it = SampleGroup(first, last, K, rng_);
            auto minIt = std::min_element(
                first,
                it,
                [](auto& a, auto& b)
                {
                   return a.cost < b.cost;
                });
            std::swap(first[0], *minIt);

            auto& parentA = first[0];

            // Find the second parent
            it = SampleGroup(first + 1, last, K, rng_);
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
        value_type tempA {0.0, std::make_unique<std::size_t[]>(Env().Count())};

        std::uniform_int_distribution<std::size_t> d{ 0, Env().Count() - 1 };
        auto const Offset = d(rng_);
        auto const Length = d(rng_);

        std::uniform_real_distribution<> realDis{0, 100};

        Order1Crossover(
            parentA.route.get(),
            parentA.route.get() + Env().Count(),
            parentB.route.get(),
            Offset,
            Length,
            tempA.route.get());

        if(realDis(rng_) <= 5)
            std::shuffle(
                tempA.route.get(),
                tempA.route.get() + Env().Count(),
                rng_);

        value_type tempB{ 0.0, std::make_unique<std::size_t[]>(Env().Count())};
        Order1Crossover(
            parentB.route.get(),
            parentB.route.get() + Env().Count(),
            parentA.route.get(),
            Offset,
            Length,
            tempB.route.get());

        if (realDis(rng_) <= 5)
            std::shuffle(
                tempB.route.get(),
                tempB.route.get() + Env().Count(),
                rng_);

        return {std::move(tempA), std::move(tempB)};
    }

    void Mutate(value_type& value)
    {
        std::uniform_real_distribution<> dis{0.0, 100.0};
        if(dis(rng_) <= MutationProbabillity)
            Opt2RandomSwap(
                value.route.get(),
                value.route.get() + Env().Count(),
                rng_);
    }

    void Evaluate(value_type& value)
    {
        value.cost =  CostOf(
            Env(),
            value.route.get(),
            value.route.get() + Env().Count());
    }

    //template<typename RanomItA, typename RandomItB>
    //void SelectNext(RandomItA firstA, RandomItA lastB)
};

int main(int argc, char const** argv)
{
    char const* fileName = "sample/ulysses16.csv";
    if (argc == 2)
        fileName = argv[1];

    Simulate(CS3910EvolutionPolicy<double>{fileName});
}

template<typename T>
void CS3910EvolutionPolicy<T>::Initialise()
{
    best_ = std::numeric_limits<double>::infinity();
    iteration_ = 0;
    populationSize_ = 100;
    population_ = std::make_unique<value_type[]>(populationSize_);

    std::generate(
        population_.get(),
        population_.get() + populationSize_,
        [&]()
        {
            value_type temp{0.0, std::make_unique<std::size_t[]>(Env().Count())};
            std::iota(temp.route.get(), temp.route.get() + Env().Count(), 0);
            std::shuffle(temp.route.get(), temp.route.get() + Env().Count(), rng_);
            temp.cost = CostOf(Env(), temp.route.get(), temp.route.get() + Env().Count());
            return temp;
        });
}

template<typename T>
void CS3910EvolutionPolicy<T>::Step()
{
    std::vector<value_type> nextGen{};
    for (auto it{ population_.get() }; it != population_.get() + populationSize_;)
    {
        auto [parentA, parentB, next] = Select(it, population_.get() + populationSize_);
        it = next;

        auto [childA, childB] = Crossover(parentA, parentB);
        Mutate(childA);
        Mutate(childB);

        Evaluate(childA);
        Evaluate(childB);

        nextGen.emplace_back(std::move(childA));
        nextGen.emplace_back(std::move(childB));
    }

    
    // New generation
    //std::sort(
    //    population_.get(),
    //    population_.get() + populationSize_,
    //    [](auto& a, auto&b)
    //    {
    //        return a < b;
    //    });
    std::move(nextGen.begin(), nextGen.end(), population_.get());

    auto it = std::min_element(
        population_.get(),
        population_.get() + populationSize_,
        [](auto& a, auto& b)
        {
            return a.cost < b.cost;
        });

    if (it != population_.get() + populationSize_ && it->cost < best_)
    {
        best_ = it->cost;
        std::cout << iteration_ << ": " << it->cost << " ";
        Show(std::cout, it->route.get(), it->route.get() + Env().Count());
    }
}

template<typename T>
bool CS3910EvolutionPolicy<T>::Terminate()
{
    return 1000000 < iteration_++;
}