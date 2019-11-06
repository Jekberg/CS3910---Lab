#include "Extract.h"
#include "TravlingSalesman.h"
#include "CS3910/Mutation.h"
#include "CS3910/Selection.h"
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
struct CS3910EvolutionPolicy : private TravlingSalesman
{
public:
    using value_type = struct
    {
        T cost;
        std::unique_ptr<std::size_t[]> route;
    };

    explicit CS3910EvolutionPolicy(AdjacencyMatrix<T>& env, std::string_view* nameIndex) noexcept;

    void Initialise();

    void Step();

    void Complete(){}

    bool Terminate();
private:
    AdjacencyMatrix<T>& env_;

    std::size_t populationSize_;

    std::unique_ptr<value_type[]> population_;

    double best_;

    std::size_t iteration_;

    std::minstd_rand0 rng_{};

    std::string_view* nameIndex_;

    template<typename RandomIt>
    void CreateOffspring(
        RandomIt firstA,
        RandomIt lastA,
        RandomIt firstB,
        std::size_t offset,
        std::size_t length,
        RandomIt outIt)
    {
        auto it = std::rotate_copy(
            firstA,
            firstA + offset,
            lastA,
            outIt);

        auto i = offset + length;
        for(; i < env_.Count() && it != outIt + env_.Count(); ++i)
            if(std::find(outIt, outIt + length, firstB[i]) == outIt + length)
                *(it++) = firstB[i];
        
        i %= env_.Count();
        for(; i < offset && it != outIt + env_.Count(); ++i)
            if (std::find(outIt, outIt + length, firstB[i]) == outIt + length)
                *(it++) = firstB[i];
    }
};

int main(int argc, char const** argv)
{
    char const* fileName = "sample/ulysses16.csv";
    if (argc == 2)
        fileName = argv[1];
    auto data = ExtractDataFrom<CS3910ExtractTraits>(fileName);
    auto nodeNames{ std::make_unique<std::string_view[]>(data.size()) };
    std::transform(data.begin(), data.end(), nodeNames.get(), [](auto&& node)
        {
            return std::string_view{ node.id };
        });

    AdjacencyMatrix<double> graph{ data.size() };
    for (auto i = data.begin(); i != data.end(); ++i)
        for (auto j = i + 1; j != data.end(); ++j)
            graph(std::distance(data.begin(), i),
                std::distance(data.begin(), j)) =
            std::hypot(i->x - j->x, i->y - j->y);

    Simulate(CS3910EvolutionPolicy<double>{graph, nodeNames.get()});
}

template<typename T>
CS3910EvolutionPolicy<T>::CS3910EvolutionPolicy(
    AdjacencyMatrix<T>& env,
    std::string_view* nameIndex)
    noexcept
    : env_{env}
    , nameIndex_{nameIndex}
{
}

template<typename T>
void CS3910EvolutionPolicy<T>::Initialise()
{
    best_ = std::numeric_limits<double>::infinity();
    iteration_ = 0;
    populationSize_ = 10;
    population_ = std::make_unique<value_type[]>(populationSize_);

    std::generate(
        population_.get(),
        population_.get() + populationSize_,
        [&]()
        {
            value_type temp{0.0, std::make_unique<std::size_t[]>(env_.Count())};
            std::iota(temp.route.get(), temp.route.get() + env_.Count(), 0);
            std::shuffle(temp.route.get(), temp.route.get() + env_.Count(), rng_);
            temp.cost = CostOf(env_, temp.route.get(), temp.route.get() + env_.Count());
            return temp;
        });
}

template<typename RandomIt>
void Recombine(RandomIt first, RandomIt last)
{
}

template<typename T>
void CS3910EvolutionPolicy<T>::Step()
{
    // Select
    TournamentSelection(
        population_.get(),
        population_.get() + populationSize_,
        2,
        rng_,
        [](auto& a, auto& b)
        {
            return a.cost < b.cost; 
        });

    std::vector<value_type> nextGen{};

    // Recombine
    for(auto i = population_.get() + 1; i < population_.get() + populationSize_; i += 2)
    {
        std::uniform_int_distribution<std::size_t> d{ 0, env_.Count() - 1 };
        auto const Offset = d(rng_);
        auto const Length = d(rng_);
        auto tempA {std::make_unique<std::size_t[]>(env_.Count())};
        auto tempB {std::make_unique<std::size_t[]>(env_.Count())};

        CreateOffspring(
            i[-1].route.get(),
            i[-1].route.get() + env_.Count(),
            i[0].route.get(),
            Offset,
            Length,
            tempA.get());

        CreateOffspring(
            i[0].route.get(),
            i[0].route.get() + env_.Count(),
            i[-1].route.get(),
            Offset,
            Length,
            tempB.get());

        // Mutation
        Opt2RandomSwap(tempA.get(), tempA.get() + env_.Count(), rng_);
        Opt2RandomSwap(tempB.get(), tempB.get() + env_.Count(), rng_);

        // Evaluate
        nextGen.emplace_back(value_type{
            CostOf(env_, tempA.get(), tempA.get() + env_.Count()),
            std::move(tempA) });
        nextGen.emplace_back(value_type{
            CostOf(env_, tempB.get(), tempB.get() + env_.Count()),
            std::move(tempB) });
    }
    
    // New generation
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
        std::cout << iteration_ << ": " << it->cost << " [";
        std::cout << nameIndex_[it->route[0]];
        std::for_each(it->route.get() + 1, it->route.get() + env_.Count(), [&](auto& x)
            {
                std::cout << ' ' << nameIndex_[x];
            });
        std::cout << "]\n";
    }
}

template<typename T>
bool CS3910EvolutionPolicy<T>::Terminate()
{
    return 1000000 < iteration_++;
}