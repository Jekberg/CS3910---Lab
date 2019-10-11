#include "AntennaArray.h"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <iterator>
#include <random>
#include <valarray>

static std::minstd_rand rng{};

struct Solution
{
    double score;
    std::valarray<double> position;
};

struct Position
{
    Solution personalBest;
    Solution current;
    std::valarray<double> velocity;
};

std::valarray<double> generateAAS(std::size_t count, double length);

template<typename ForwardIt>
void runPSO(AntennaArray& aa, ForwardIt first, ForwardIt last);

int main()
{
    std::ios_base::sync_with_stdio(false);
    AntennaArray aa{3, 90.0};

    std::vector<Position> solutions(10);
    std::generate(solutions.begin(), solutions.end(), [&]() -> auto
    {
        auto x = generateAAS(3, 1.5);

        std::vector<double> temp{std::begin(x), std::end(x)};
        temp.emplace_back(1.5);

        auto score = aa.evaluate(temp);
        Solution s{score, x};
        return Position{s, s, std::valarray<double>(2)};
    });

    runPSO(aa, solutions.begin(), solutions.end());
}


std::valarray<double> generateAAS(std::size_t count, double length)
{
    const auto padding = (length - AntennaArray::MIN_SPACING * (count)) / count;
    std::uniform_real_distribution<> dis{0.0, padding};
    std::vector<double> temp(count - 1);

    std::accumulate(
        std::begin(temp),
        std::end(temp),
        double{},
        [&](double base, double& now)
        {
            now = dis(rng) + base;
            return now + AntennaArray::MIN_SPACING;
        });
    return std::valarray<double>{temp.data(), count - 1};
}

std::valarray<double> randomVec(std::uniform_real_distribution<>& dis, std::size_t count)
{
    std::vector<double> temp(count - 1);
    std::generate(temp.begin(), temp.end(),
        [&]() -> auto {return dis(rng);});
    return {temp.data(), count - 1};
}

void update(AntennaArray& aa, Position& pos, Solution const& best)
{
    const double inertia {0.721};
    const double cognitiveInertia{1.1193};
    const double socialInertia{ 1.1193 };
    
    std::uniform_real_distribution<> dis{0.0, 1.0};

    // Move to the new position..
    pos.velocity = inertia * pos.velocity
        + cognitiveInertia * randomVec(dis, 3) * (pos.personalBest.position - pos.current.position)
        + socialInertia * randomVec(dis, 3) * (best.position - pos.current.position);

    pos.current.position = pos.current.position + pos.velocity;
    std::vector<double> temp{
        std::begin(pos.current.position),
        std::end(pos.current.position)};
    temp.emplace_back(1.5);

    pos.current.score = aa.evaluate(temp);
    if(pos.current.score < pos.personalBest.score)
        pos.personalBest = pos.current;
}

template<typename ForwardIt>
void runPSO(AntennaArray& aa, ForwardIt first, ForwardIt last)
{
    double overallBest = std::numeric_limits<double>::max();
    while(1)
    {
        auto it = std::min_element(
            first,
            last,
            [](auto& lhs, auto& rhs) -> bool
        {
            return lhs.personalBest.score < rhs.personalBest.score;
        });

        auto globalBest = it->personalBest;
        if(globalBest.score < overallBest)
        {
            overallBest = globalBest.score;
            std::cout << "Best: " << globalBest.score << " [";
            std::for_each(std::begin(globalBest.position), std::end(globalBest.position),
                [](auto p) {std::cout << p << ' ';});
            std::cout << 1.5 << "]\n";
        }
        std::for_each(first, last, [&](auto& s)
        {
            update(aa, s, globalBest);
        });
    }
}