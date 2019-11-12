#ifndef TRAVLINGSALESMAN_H_
#define TRAVLINGSALESMAN_H_

#include "CS3910/Graph.h"
#include <cmath>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace internal
{
    template<typename OutputIt, typename Extraction>
    void ReadTravlingSalesmanData(
        char const* fileName,
        OutputIt outIt,
        Extraction&& extract)
    {
        assert(fileName != nullptr);
        std::ifstream file{};
        file.open(fileName);

        if (!file.is_open())
            /* Error */;

        while (extract(file, *outIt))
            ++outIt;
    }
}

template<typename T>
class TravlingSalesman
{
public:
    struct NodeInfo
    {
        std::string name;
        T x;
        T y;
    };

    explicit TravlingSalesman(char const* fileName);

    template<typename ForwardIt>
    std::ostream& Show(std::ostream& outs, ForwardIt first, ForwardIt);

    constexpr AdjacencyMatrix<T>& Env() noexcept;

    constexpr NodeInfo const* Nodes() const noexcept;

    constexpr NodeInfo const& Node(std::size_t id) const noexcept;

private:
    std::vector<NodeInfo> nodeIndex_;

    AdjacencyMatrix<T> env_;

    template<typename Container>
    static inline AdjacencyMatrix<T> ReadGraphFromFile(
        char const* fileName,
        Container&& container);
};

template<typename T>
TravlingSalesman<T>::TravlingSalesman(char const* fileName)
    : nodeIndex_{}
    , env_{ReadGraphFromFile(fileName, nodeIndex_)}
{
}

template<typename T>
template<typename ForwardIt>
std::ostream& TravlingSalesman<T>::Show(
    std::ostream& outs,
    ForwardIt first,
    ForwardIt last)
{
    outs << '[' << Node(*(first++)).name;
    std::for_each(
        first,
        last,
        [&](auto& x)
        {
            outs << ' ' << Node(x).name;
        });
    return outs << "]\n";
}

template<typename T>
constexpr AdjacencyMatrix<T>& TravlingSalesman<T>::Env() noexcept
{
    return env_;
}

template<typename T>
constexpr typename TravlingSalesman<T>::NodeInfo const&
TravlingSalesman<T>::Node(std::size_t id)
    const
    noexcept
{
    return nodeIndex_[id];
}

template<typename T>
constexpr typename TravlingSalesman<T>::NodeInfo const*
TravlingSalesman<T>::Nodes()
    const
    noexcept
{
    return nodeIndex_.data();
}

template<typename T>
template<typename Container>
AdjacencyMatrix<T> TravlingSalesman<T>::ReadGraphFromFile(
    char const* fileName,
    Container&& container)
{
    internal::ReadTravlingSalesmanData(
        fileName,
        std::back_inserter(container),
        [](std::istream& ins, auto& node)
        {
            std::string line;
            if (!(ins >> line))
                return false;

            auto first = line.begin();
            auto last = std::find(first, line.end(), ',');
            std::string name{ first, last };

            first = last + 1;
            last = std::find(first, line.end(), ',');
            std::string x { first, last };

            first = last + 1;
            last = std::find(first, line.end(), ',');
            std::string y { first, last };

            node = NodeInfo{name, std::stod(x), std::stod(y)};
            return true;
        });

    AdjacencyMatrix<double> graph{ container.size() };
    for (auto i = container.begin(); i != container.end(); ++i)
        for (auto j = i + 1; j != container.end(); ++j)
            graph(std::distance(container.begin(), i),
                std::distance(container.begin(), j)) =
            std::hypot(i->x - j->x, i->y - j->y);

    return graph;
}

#endif // !TRAVLINGSALESMAN_H_
