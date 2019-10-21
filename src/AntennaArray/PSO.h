#ifndef PSO_H_
#define PSO_H_

#include <algorithm>
#include <charconv>
#include <numeric>
#include <iostream>
#include <iterator>
#include <random>
#include <valarray>
#include <memory>

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

struct DefaultParticlePolicy
{
    using value_type = std::valarray<double>;
    using generator_type = std::minstd_rand;

    static generator_type createGenerator()
    {
        return generator_type{};
    }

    static std::size_t populationSize(AntennaArray& arr)
    {
        return arr.count();
    }

    static value_type initialVelocity(AntennaArray& arr)
    {
        return value_type(arr.count() - 1);
    }

    static value_type initialise(AntennaArray& arr, generator_type& rng)
    {
        auto const bounds = arr.bounds();
        auto const count = bounds.size() - 1;
        auto data = std::make_unique<double[]>(count + 1);
        data[count] = arr.bounds().back().max;

        do
        {
            std::transform(std::begin(bounds), std::end(bounds) - 1,
                data.get(),
                [&](auto&& bound)
                {
                    return std::uniform_real_distribution<>{ bound.min, bound.max }(rng);
                });
            std::sort(data.get(), data.get() + count);
        } while (arr.is_valid(data.get(), data.get() + count + 1));
        return std::valarray<double>{data.get(), count};
    }

    static value_type nextVelocity(
        AntennaArray& arr,
        value_type const& pos,
        value_type const& pBest,
        value_type const& gBest,
        value_type const& velocity,
        generator_type& rng)
    {
        auto const count = arr.count() - 1;
        double const inertia{ 0.721 };
        double const cognitiveInertia{ 1.1193 };
        double const socialInertia{ 1.1193 };

        auto randVec = [&]() -> auto {
            auto temp = std::make_unique<double[]>(count);
            std::uniform_real_distribution<>{ 0.0, 1.0 };
            std::generate(temp.get(), temp.get() + count,
                [&]() -> auto {
                return std::uniform_real_distribution<>{ 0.0, 1.0 }(rng);
            });
            return std::valarray<double>{ temp.get(), count };
        };

        // Move to the new position..
        return inertia * velocity
            + cognitiveInertia * randVec() * (pBest - pos)
            + socialInertia * randVec() * (gBest - pos);
    }

    static double evaluate(AntennaArray& arr, value_type const& solution)
    {
        std::vector<double> temp{ std::begin(solution), std::end(solution) };
        temp.emplace_back(arr.bounds().back().max);
        return arr.evaluate(temp.begin(), temp.end());
    }

    static std::ostream& print(std::ostream& outs, AntennaArray& arr, value_type const& solution)
    {
        std::for_each(std::begin(solution), std::end(solution), [&](auto a) {
            std::cout << ' ' << a;
            });

        std::cout << ' ' << arr.bounds().back().max;
        return outs;
    }
};

template<typename ParticlePolicy>
void particleSwarmOptimization(AntennaArray& arr)
{
    auto const size = ParticlePolicy::populationSize(arr);
    auto rng = ParticlePolicy::createGenerator();
    auto population{ std::make_unique<Position[]>(size) };

    std::generate(population.get(), population.get() + size, [&]() -> auto
    {
        auto temp = ParticlePolicy::initialise(arr, rng);
        Solution score{ ParticlePolicy::evaluate(arr, temp), temp };
        return Position{ score, score, ParticlePolicy::initialVelocity(arr) };
    });


    double overallBest = std::numeric_limits<double>::infinity();
    auto i{ 1000 };
    while (i-- > 0)
    {
        auto it = std::min_element(
            population.get(), population.get() + size,
            [](auto& lhs, auto& rhs) -> bool
            {
                return lhs.personalBest.score < rhs.personalBest.score;
            });

        auto globalBest = it->personalBest;
        if (globalBest.score < overallBest)
        {
            overallBest = globalBest.score;
            std::cout << "Best: " << globalBest.score << ' ';
            ParticlePolicy::print(std::cout, arr, globalBest.position) << '\n';
        }

        std::for_each(population.get(), population.get() + size, [&](auto& s) {
            s.velocity = ParticlePolicy::nextVelocity(
                arr,
                s.current.position,
                s.personalBest.position,
                globalBest.position,
                s.velocity,
                rng);
            s.current.position += s.velocity;

            std::vector<double> temp{ std::begin(s.current.position), std::end(s.current.position) };
            std::sort(temp.begin(), temp.end());
            s.current.position = std::valarray<double>(temp.data(), temp.size());

            if(auto max = arr.bounds().back().max; max < temp.back())
                s.current.position /= max;
        
            s.current.score = ParticlePolicy::evaluate(arr, s.current.position);
            if (s.current.score < s.personalBest.score)
                s.personalBest = s.current;
            });
    }
}



#endif // !PSO_H_
