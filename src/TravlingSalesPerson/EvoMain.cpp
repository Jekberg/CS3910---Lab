#include "CS3910/Simulation.h"
#include "CS3910/Graph.h"
#include <cstddef>
#include <memory>
#include <random>

template<typename T>
struct CS3910EvolutionPolicy
{
public:
    using value_type = struct
    {
        T cost;
        std::unique_ptr<std::size_t[]> route;
    };

    void Initialise();

    void Step();

    void Complete();

    bool Terminate()
    {
        return false;
    }
private:
    AdjacencyMatrix<T> env_;

    std::size_t populationSize_;

    std::unique_ptr<value_type[]> population_;

    double best_;

    std::minstd_rand0 rng_{};
};

int main(int argc, char const** argv)
{
    //Simulation<CS3910EvolutionPolicy<double>>{}.Run();
}

template<typename T>
void CS3910EvolutionPolicy<T>::Initialise()
{
    best_ = std::numeric_limits<double>::infinity();
    populationSize_ = 10;
    population_ = std::make_unique<value_type[]>(populationSize_);

    std::generate(
        population_.get(),
        population_.get() + populationSize_,
        [&]()
        {
            auto temp = {0.0, std::make_unique<std::size_t[]>(env_.Count())};
            std::iota(temp.route.get(), temp.route.get() + env_.Count(), 0);
            std::shuffle(temp.route.get(), temp.route.get() + env_.Count(), rng_);
            temp.cost = CostOf(env_, temp.route.get(), temp.route.get() + env_.Count());
            return temp;
        });
}

//template<typename RandomIt>
//void f(RandomIt first, RandomIt last)
//{
//    auto const K = 2;
//    // Torunament
//    for(auto i = first ; i != last; i++)
//    {
//        // Select K members
//        for(auto j = 0; j < K; ++j)
//        {
//            auto c = std::uniform_int_distribution<std::size_t>{
//                j,
//                std::distance(i, last)}(rng_);
//            std::swap(i[j], i[c]);
//        }
//
//        // Get the best fitness
//        auto winnerIt = std::max_element(i, k, [=](auto& a, auto& b)
//        {
//            return a > b;
//        });
//
//        // Set the ...
//        std::swap(*i, *winnerIt);
//    }
//}

template<typename T>
void CS3910EvolutionPolicy<T>::Step()
{
    // Select
    // Recombine
    // Mutate
    // Evaluate
    // Next gen
}