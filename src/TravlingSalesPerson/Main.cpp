#include "Graph.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iterator>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <tuple>

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

template<typename T, typename RandomIt>
T cost(AdjacencyMatrix<T> const& m, RandomIt first, RandomIt last);

int main(int argc, char const** argv)
{
    auto data = ReadDataFrom<CS3910InputTraits>("sample/ulysses16.csv");
    std::vector<std::tuple<std::int64_t, std::int64_t, double>> edges{};

    AdjacencyMatrix<double> m{data.size()};
    for(auto i = data.begin(); i != data.end(); ++i)
        for (auto j = i + 1; j != data.end(); ++j)
                m(std::distance(data.begin(), i),
                std::distance(data.begin(), j)) =
                std::hypot(i->x - j->x, i->y - j->y);

    std::minstd_rand rng{};
    std::vector<std::size_t> path(m.Count());
    std::iota(path.begin(), path.end(), 0);
    auto i{0};

    double best = std::numeric_limits<double>::infinity();

    while(true)
    { 
        ++i;
        std::shuffle(std::begin(path), std::end(path), rng);
        auto c = cost(m, std::begin(path), std::end(path));

        if( c < best)
        { 
            best = c;
            std::cout << i << ": " << c << " | ";
            for(auto&& n: path)
                std::cout << ' ' << n;
            std::cout << '\n';
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