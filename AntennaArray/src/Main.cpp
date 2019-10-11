#include "AntennaArray.h"
#include <algorithm>
#include <iostream>
#include <random>

static std::minstd_rand rng{};

struct Solution
{
    double score;
    std::vector<double> position;
};

struct Position
{
    Solution personalBest;
    Solution current;
    std::vector<double> velocity;
};

std::vector<double> generateAAS(std::size_t count, double length);

template<typename ForwardIt>
void dotProduct(
    ForwardIt first1, 
    ForwardIt end1,
    ForwardIt first2,
    ForwardIt end2);

template<typename ForwardIt>
void runPSO(AntennaArray& aa, ForwardIt first, ForwardIt last);

int main()
{
    AntennaArray aa{3, 90.0};

    std::vector<Position> solutions(10);
    std::generate(solutions.begin(), solutions.end(), [&]() -> auto
    {
        auto x = generateAAS(3, 1.5);
        auto score = aa.evaluate(x);
        Solution s{score, x};
        return Position{s, s, std::vector<double>(3)};
    });

    runPSO(aa, solutions.begin(), solutions.end());
}


std::vector<double> generateAAS(std::size_t count, double length)
{
    const auto padding = (length - AntennaArray::MIN_SPACING * (count)) / count;
    std::uniform_real_distribution<> dis{0.0, padding};
    std::vector<double> temp(count);
    temp.back() = length;

    std::accumulate(
        temp.begin(),
        temp.end() - 1,
        double{},
        [&](double base, double& now) {
            
            now = dis(rng) + base;
            return now + AntennaArray::MIN_SPACING;
        });
    return temp;
}


void update(AntennaArray& aa, Position& pos, Solution const& best)
{
    const double inertia {0.5};
    const double cognitiveInertia{0.5};
    const double socialInertia{0.5};

    auto& s = pos.current;
    
    // n * v(t)
    std::for_each(
        pos.velocity.begin(),
        pos.velocity.end(),
        [=](auto& x){x *= inertia;});

    std::uniform_real_distribution<> dis{0.0, 1.0};

    // o1 * r1 * (p(t) - x(t))
    auto t1 = pos.personalBest.position;
    {
        std::vector<double> r1(3);
        std::generate(
            r1.begin(),
            r1.end(),
            [&]() -> auto {
                return cognitiveInertia * dis(rng);
            });
        std::transform(
            t1.begin(),
            t1.end(),
            pos.current.position.begin(),
            t1.begin(),
            std::minus<double>{});
        std::transform(
            t1.begin(),
            t1.end(),
            r1.begin(),
            t1.begin(),
            std::multiplies<double>{});
    }

    // o2 * r2 * (g(t) - x(t))
    std::vector<double> r2(3);  
    auto t2 = best.position;
    {
        std::generate(
            r2.begin(),
            r2.end(),
            [&]() -> auto {
                return socialInertia * dis(rng);
            });
        std::transform(
            t2.begin(),
            t2.end(),
            pos.current.position.begin(),
            t2.begin(),
            std::minus<double>{});
        std::transform(
            t2.begin(),
            t2.end(),
            r2.begin(),
            t2.begin(),
            std::multiplies<double>{});
    }
    
    // Move to the new position..
    std::transform(
        pos.current.position.begin(),
        pos.current.position.end(),
        pos.velocity.begin(),
        pos.current.position.begin(),
        std::plus<double>{});
    std::transform(
        pos.current.position.begin(),
        pos.current.position.end(),
        t1.begin(),
        pos.current.position.begin(),
        std::plus<double>{});
    std::transform(
        pos.current.position.begin(),
        pos.current.position.end(),
        t1.begin(),
        pos.current.position.begin(),
        std::plus<double>{});
        
    if(pos.current.score < pos.personalBest.score)
        pos.personalBest = pos.current;
}

template<typename ForwardIt>
void runPSO(AntennaArray& aa, ForwardIt first, ForwardIt last)
{
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

        std::cout << "Best: " << globalBest.score << '\n';
        std::for_each(first, last, [&](auto& s)
        {
            update(aa, s, globalBest);
        });
    }
}