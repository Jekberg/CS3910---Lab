#ifndef CS3910__HILLCLIMB_H_
#define CS3910__HILLCLIMB_H_

#include "Graph.h"
#include <cstddef>
#include <limits>
#include <memory>
#include <ostream>
#include <random>
#include <string_view>

template<typename GraphT>
class CS3910HillClimbPolicy
{
public:
    using value_type = std::unique_ptr<std::size_t[]>;

    explicit CS3910HillClimbPolicy(std::string_view* nameArray, std::size_t count)
        : nameCount_{ count }
        , nameArray_{ nameArray }
    {
        assert(nameArray != nullptr);
    }

    bool Terminate()
    {
        return false;
    }

    void Initialise(GraphT const& graph, value_type& value) noexcept;

    void Present(GraphT const& graph, value_type& value, std::ostream& outs)
    {
        if(graph.Count() != 0)
            outs << '[' << nameArray_[value[0]];
        std::for_each(value.get() + 1, value.get() + graph.Count(), [&](auto id)
        {
            outs << ' ' << nameArray_[id];
        });

        outs << ']';
    }

    constexpr bool NewBest(double& oldBest, double newBest)
    {
        if(newBest < oldBest)
        {
            oldBest = newBest;
            return true;
        }
        else
            return false;
    }

    value_type Create(GraphT const& graph)
    {
        auto ptr{ std::make_unique<std::size_t[]>(graph.Count()) };
        std::iota(ptr.get(), ptr.get() + graph.Count(), 0);
        return std::move(ptr);
    }

    constexpr double StartValue()
    {
        return std::numeric_limits<double>::infinity();
    }

    double Step(GraphT const& graph, value_type& value);

    double Evaluate(GraphT const& graph, value_type const& value);
private:

    std::minstd_rand rng_{};
    std::size_t nameCount_;
    std::string_view* nameArray_;
};

template<typename GraphT>
void CS3910HillClimbPolicy<GraphT>::Initialise(
    GraphT const& graph,
    value_type& value)
    noexcept
{
    std::shuffle(value.get() + 1, value.get() + graph.Count(), rng_);
}

template<typename GraphT>
double CS3910HillClimbPolicy<GraphT>::Step(GraphT const& graph, value_type& value)
{
    double bestCost = StartValue();
    std::size_t bestI;
    std::size_t bestJ;
    do
    {
        bestI = 0;
        bestJ = 0;
        for(std::size_t i{1}; i < graph.Count(); ++i)
            for (std::size_t j{ i + 1 }; j < graph.Count(); ++j)
            {
                std::swap(value[i], value[j]);
                if (auto const cost = Evaluate(graph, value); NewBest(bestCost, cost))
                {
                    bestI = i;
                    bestJ = j;
                }
                std::swap(value[i], value[j]);
            }

        // Use the best swap
        std::swap(value[bestI], value[bestJ]);
    }
    while(bestI != bestJ);


    return bestCost;
}

template<typename GraphT>
double CS3910HillClimbPolicy<GraphT>::Evaluate(
    GraphT const& graph,
    value_type const& value)
{
    return costOfCycle(graph, value.get(), value.get() + graph.Count());
}

#endif // !CS3910__HILLCLIMB_H_