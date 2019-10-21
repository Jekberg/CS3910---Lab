#include "Graph.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iterator>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <string>

struct CS3910InputTraits
    {
    using value_type = struct{
        int id;
        double x;
        double y;
    };

    static void Preamble(std::istream& ins)
    {
        // Skip the headers
        std::string header;
        std::getline(ins, header);
        std::getline(ins, header);
        std::getline(ins, header);
    }

    static bool Extract(std::istream& ins, value_type& data)
    {
        std::string line;
        if(!(ins >> line))
            return false;

        auto first = line.begin();
        auto last = std::find(first, line.end(), ',');

        data.id = std::stoi(std::string{first, last});
        
        first = last + 1;
        last = std::find(first, line.end(), ',');

        data.x = std::stod(std::string{ first, last });

        first = last + 1;
        last = std::find(first, line.end(), ',');
        data.y = std::stod(std::string{ first, last });
        return true;
    }
};

template<typename InputTraits>
std::vector<typename InputTraits::value_type> ReadDataFrom(char const* fileName);

template<typename GraphT>
struct CS3910TravlingSalesPerson
{
    using value_type = std::unique_ptr<std::size_t[]>;

    template<typename RNG>
    static value_type Initialise(GraphT const& graph, RNG&& rng)
    {
        auto ptr {std::make_unique<std::size_t[]>(graph.Count())};
        std::iota(ptr.get(), ptr.get() + graph.Count(), 0);
        std::shuffle(ptr.get() + 1, ptr.get() + graph.Count(), rng);
        return std::move(ptr);
    }

    template<typename RNG>
    static value_type Step(GraphT const& graph, value_type&& value, RNG&& rng)
    {
        return value;
    }
};

std::vector<std::unique_ptr<std::size_t[]>> Opt2Neighbours(
    std::size_t* array,
    std::size_t count);

template<typename T, typename RandomIt>
T cost(AdjacencyMatrix<T> const& m, RandomIt first, RandomIt last);

int main(int argc, char const** argv)
{
    using TSP = CS3910TravlingSalesPerson<AdjacencyMatrix<double>>;
    auto data = ReadDataFrom<CS3910InputTraits>("sample/ulysses16.csv");
    std::vector<std::tuple<std::int64_t, std::int64_t, double>> edges{};

    AdjacencyMatrix<double> m{data.size()};
    for(auto i = data.begin(); i != data.end(); ++i)
        for (auto j = i + 1; j != data.end(); ++j)
            m(std::distance(data.begin(), i),
                std::distance(data.begin(), j)) =
                std::hypot(i->x - j->x, i->y - j->y);

    std::minstd_rand0 rng{};

    std::vector<TSP::value_type> pop(10);
    std::generate(pop.begin(), pop.end(), [&](){
        return TSP::Initialise(m, rng);
    });

    double best = std::numeric_limits<double>::infinity();

    auto i{0};
    while(i < 1000000)
    {
        ++i;
        for(auto&& path: pop)
        {
            auto localBest = cost(m, path.get(), path.get() + m.Count());
            
            for(auto&& n: Opt2Neighbours(path.get(), m.Count()))
            {
                auto nCost = cost(m, n.get(), n.get() + m.Count());
                if(nCost < localBest)
                {
                    localBest = nCost;
                    path = std::move(n);
                }
            }
            
            if(localBest < best)
            { 
                best = localBest;
                std::cout << i << ": " << best << " | ";
                std::for_each(path.get(), path.get() + m.Count(), [](auto e)
                {
                    std::cout << e << ' ';
                });
                std::cout << '\n';
            }
        }
        
    }
}

template<typename InputTraits>
std::vector<typename InputTraits::value_type> ReadDataFrom(char const* fileName)
{
    assert(static_cast<bool>(fileName) && "The filename is nullptr");
    std::ifstream csv{};
    csv.open(fileName, std::ios_base::in);

    InputTraits::Preamble(csv);
    std::vector<typename InputTraits::value_type> data{};
    for (typename InputTraits::value_type value; InputTraits::Extract(csv, value);)
        data.emplace_back(std::move(value));

    return data;
};

std::vector<std::unique_ptr<std::size_t[]>> Opt2Neighbours(
    std::size_t* array,
    std::size_t count)
{
    assert(array != nullptr);
    assert(count != 0);

    std::vector<std::unique_ptr<std::size_t[]>> set{};
    for(auto i{array + 1}; i  != array + count; ++i)
        for(auto j{i + 1}; j != array + count; ++j)
        {
            set.emplace(set.end(), std::make_unique<std::size_t[]>(count));
            auto& ptr = set.back();
            std::copy(array, array + count, ptr.get());
            std::swap(ptr[std::distance(array, i)], ptr[std::distance(array, j)]);
        }

    return set;
}

template<typename T, typename RandomIt>
T cost(AdjacencyMatrix<T> const& m, RandomIt first, RandomIt last)
{
    assert(first != last && "No empty ranges allowed");
    assert(std::distance(first, last) == m.Count() && "Not all nodes visited");
    assert(std::unique(first, last) == last && "Visiting duplicate nodes");

    T totalCost{ *first > last[-1]
        ? m(last[-1], *first)
        : m(*first, last[-1]) };
    for (; first + 1 != last; ++first)
    {
        totalCost += first[0] > first[1]
            ? m(first[1], first[0])
            : m(first[0], first[1]);
    }

    return totalCost;
}