#include "Extract.h"
#include "TravlingSalesman.h"
#include "CS3910/Graph.h"
#include "CS3910/Pheromone.h"
#include "CS3910/Simulation.h"
#include <algorithm>
#include <execution>
#include <iostream>
#include <numeric>
#include <random>
#include <string_view>

template<typename T>
class CS3910AntSystemPolicy: private TravlingSalesman
{
public:
    using value_type = struct
    {
        typename AdjacencyMatrix<T>::value_type cost;
        std::unique_ptr<std::size_t[]> route;
        std::minstd_rand0 rng{};
    };

    explicit CS3910AntSystemPolicy(
        AdjacencyMatrix<T>& env,
        std::string_view* nameIndex)
        noexcept;
    
    void Initialise();

    void Step();

    void Complete()
    {
    }

    bool Terminate() noexcept;
private:
    // Initial Pheromone Level
    constexpr static double t0_ = 0.001;

    // Rate of evaporation
    constexpr static double p_ = 0.5;

    // Rate of deposition
    constexpr static double q_ = 100.0;

    // Relative importance of phermonone
    constexpr static double a_ = 1.0;

    // Relative importance of edge weight
    constexpr static double b_ = 5.0;

    AdjacencyMatrix<T>& env_;

    std::unique_ptr<value_type[]> population_;

    std::size_t const populationSize_ = 100;

    std::size_t iteration_;

    T best_;

    std::string_view* nameIndex_;

    template<typename RandomIt, typename RngT>
    void Construct(RandomIt first, RandomIt last, RngT& rng);
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
    

    Simulate(CS3910AntSystemPolicy<double>{graph, nodeNames.get()});
}

template<typename T>
CS3910AntSystemPolicy<T>::CS3910AntSystemPolicy(
    AdjacencyMatrix<T>& env,
    std::string_view* nameIndex)
    noexcept
    : env_{env}
    , nameIndex_{nameIndex}
{
}

template<typename T>
void CS3910AntSystemPolicy<T>::Initialise()
{
    best_ = std::numeric_limits<T>::infinity();
    iteration_ = 0;
    population_ = std::make_unique<value_type[]>(populationSize_);
    std::random_device rng{};
    std::for_each(
        population_.get(),
        population_.get() + populationSize_,
        [&](auto& ant)
        {
            ant.cost = 0.0;
            ant.route = std::make_unique<std::size_t[]>(env_.Count());
            ant.rng.seed(rng());
            std::iota(ant.route.get(), ant.route.get() + env_.Count(), 0);
        });

    for (auto i{1}; i < env_.Count(); ++i)
        for (auto j{i + 1}; j < env_.Count(); ++j)
            Pheromone(env_, i, j) = t0_;
}

template<typename T>
void CS3910AntSystemPolicy<T>::Step()
{
    std::for_each(
        std::execution::par,
        population_.get(),
        population_.get() + populationSize_,
        [&](auto& ant)
    {
        auto& [cost, route, rng] = ant;
        Construct(route.get(), route.get() + env_.Count(), rng);
        cost = CostOf(env_, route.get(), route.get() + env_.Count());
    });

    DecayPheromone(env_, p_);
    
    std::for_each(
        population_.get(),
        population_.get() + populationSize_,
        [&](auto& ant)
        {
            auto& [cost, route , rng] = ant;
            IncreasePheromone(env_, q_/cost, route.get(), route.get() + env_.Count());
        });


    auto it = std::min_element(
        population_.get(),
        population_.get() + populationSize_,
        [=](auto& a, auto& b)
        {
            return a.cost < b.cost;
        });

    if(it != population_.get() + populationSize_ && it->cost < best_)
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
template<typename RandomIt, typename RngT>
void CS3910AntSystemPolicy<T>::Construct(RandomIt first, RandomIt last, RngT& rng)
{
    assert(first != last);
    auto edgeDesire{ std::make_unique<double[]>(env_.Count()) };

    using IntDistribution = std::uniform_int_distribution<std::size_t>;

    std::swap(*first, first[IntDistribution{0, env_.Count() - 1}(rng)]);
    while (first + 1 != last)
    {
        auto const pivot{ *(first++) };
        std::for_each(
            first,
            last,
            [&](auto const next) noexcept
            {
                edgeDesire[next] = std::pow(Pheromone(env_, pivot, next), a_)
                    * std::pow(Weight(env_, pivot, next), -b_);
            });

        auto const total = std::accumulate(
            first,
            last,
            T{},
            [&](auto total, auto next)
            {
                return total + edgeDesire[next];
            });

        auto r = std::uniform_real_distribution<>{ 0.0, total }(rng);
        for (auto i{ first }; i != last; ++i)
            if (total <= (r += edgeDesire[*i]))
            {
                std::swap(*first, first[std::distance(first, i)]);
                break;
            }
    }
}

template<typename T>
bool CS3910AntSystemPolicy<T>::Terminate() noexcept
{
    return 100000 < iteration_++;
}