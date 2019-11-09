#include "CS3910/AntennaArray.h"
#include "CS3910/Simulation.h"
#include <cmath>
#include <execution>
#include <memory>
#include <random>
#include <iterator>
#include <sstream>

class CS3910ParticleSwarmPolicy
{
    using Vector = std::unique_ptr<double[]>;
public:
    using value_type = struct
    {
        double sll;
        double bestSLL;
        Vector bestPosition;
        Vector position;
        Vector velocity;
        std::minstd_rand0 rng{};
    };

    struct Parameters
    {
        std::size_t populationSize;
        std::size_t iterations;
        double n;
        double o1;
        double o2;
    };

    explicit CS3910ParticleSwarmPolicy(
        AntennaArray& env,
        Parameters const& params) noexcept;

    void Initialise();

    void Step();

    void Complete()
    {
    }

    bool Terminate();
private:
    AntennaArray& env_;

    std::unique_ptr<value_type[]> population_;

    double bestSLL_;

    Vector bestPosition_;

    std::size_t iteration_;

    Parameters params_;

    template<typename RngT>
    void Update(
        double* position,
        double* velocity,
        double const* personalBest,
        double const* globalBest,
        RngT& rng);

    void UpdateBest()
    {
        auto it = std::min_element(
            std::execution::par,
            population_.get(),
            population_.get() + params_.populationSize,
            [=](auto& a, auto& b) noexcept
            {
                return a.sll < b.sll;
            });


        if (it != population_.get() + params_.populationSize && it->sll < bestSLL_)
        {
            bestSLL_ = it->sll;
            std::copy_n(it->position.get(), env_.count(), bestPosition_.get());

            std::cout << iteration_ << ": " << bestSLL_;
            std::cout << " [" << bestPosition_[0];
            std::for_each(
                bestPosition_.get() + 1,
                bestPosition_.get() + env_.count(),
                [](auto x)
                {
                    std::cout << ' ' << x;
                });
            std::cout << "]\n";
        }
    }

    template<typename RandomIt>
    void Fix(RandomIt first, RandomIt last)
    {
        assert(first != last);
        std::sort(first, last - 1);

        if(auto min = env_.bounds().back().min; *first < min)
            *first = min;

        for(auto i = first + 1; i != last - 1; ++i)
        {
            if(i[0] < i[-1] + AntennaArray::MIN_SPACING)
                i[0] = i[-1] + AntennaArray::MIN_SPACING;
        }

        for(auto i = last - 2; i != first - 1; --i)
        {
            if(i[0] > i[1] - AntennaArray::MIN_SPACING)
                i[0] = i[1] - AntennaArray::MIN_SPACING;
        }
    }

    template<typename RandomIt, typename RngT>
    void Place(RandomIt first, RandomIt last, RngT& rng);
};

int main(int argc, char const** argv)
{
    unsigned int arrayCount = 3;
    double angle = 90.0;
    if(argc < 3)
        std::cout
            << "Minimum number of arguments is 2.\n"
            << "The first argument is the number of antennae\n"
            << "The second argument is the steering angle\n"
            << "\n\n"
            << "Running PSO with 3 antennae and 90.0 steering angle...\n";
    else
    {
        std::istringstream ss{};
        ss.str(argv[1]);
        ss >> arrayCount;
        ss.str(argv[2]);
        ss >> angle;
    }

    AntennaArray arr{arrayCount, angle};

    using ParticleSwarmPolicy = CS3910ParticleSwarmPolicy;
    typename ParticleSwarmPolicy::Parameters params{};
    params.populationSize = 20 + std::sqrt(arr.count());
    params.iterations = 10000;
    params.n = 1.0 / (2.0 * std::log(2));
    params.o1 = 1.0 / 2.0 + std::log(2);
    params.o2 = 1.0 / 2.0 + std::log(2);

    std::cout << "Running...\n";
    Simulate(CS3910ParticleSwarmPolicy{arr, params});
}

CS3910ParticleSwarmPolicy::CS3910ParticleSwarmPolicy(
    AntennaArray& env,
    Parameters const& params)
    noexcept
    : env_{env}
    , params_{params}
{
}

void CS3910ParticleSwarmPolicy::Initialise()
{
    iteration_ = 0;
    bestSLL_ = std::numeric_limits<double>::infinity();
    bestPosition_ = std::make_unique<double[]>(env_.count());

    population_ = std::make_unique<value_type[]>(params_.populationSize);
    std::random_device rng{};
    std::for_each(
        population_.get(),
        population_.get() + params_.populationSize,
        [&](auto& particle)
    {
        particle.rng.seed(rng());
        particle.position = std::make_unique<double[]>(env_.count());
        Place(particle.position.get(), particle.position.get() + env_.count(), particle.rng);

        particle.velocity = std::make_unique<double[]>(env_.count());
        std::fill_n(particle.velocity.get(), env_.count(), 0.0);

        particle.bestPosition = std::make_unique<double[]>(env_.count());
        std::copy_n(particle.position.get(), env_.count(), particle.bestPosition.get());

        particle.sll = env_.evaluate(
            particle.position.get(),
            particle.position.get() + env_.count());
        particle.bestSLL = particle.sll;
    });

}

void CS3910ParticleSwarmPolicy::Step()
{
    UpdateBest();
    std::for_each(
        std::execution::par,
        population_.get(),
        population_.get() + params_.populationSize,
        [&](auto& particle)
    {
        // Move particle
        Update(
            particle.position.get(),
            particle.velocity.get(),
            particle.bestPosition.get(),
            bestPosition_.get(),
            particle.rng);

        particle.sll = env_.evaluate(
            particle.position.get(),
            particle.position.get() + env_.count());

        if(particle.sll < particle.bestSLL)
        {
            particle.bestSLL = particle.sll;
            std::copy_n(
                particle.position.get(),
                env_.count(),
                particle.bestPosition.get());
        }
    });
}

template<typename RngT>
void CS3910ParticleSwarmPolicy::Update(
    double* position,
    double* velocity,
    double const* personalBest,
    double const* globalBest,
    RngT& rng)
{
    std::uniform_real_distribution<> d{0.0, 1.0};

    for (auto i = 0; i < env_.count() - 1; ++i)
    {
        velocity[i] = params_.n * velocity[i]
            + params_.o1 * d(rng) * (globalBest[i] - position[i])
            + params_.o2 * d(rng) * (personalBest[i] - position[i]);
        position[i] += velocity[i];
    }

    Fix(position, position + env_.count());
}

template<typename RandomIt, typename RngT>
void CS3910ParticleSwarmPolicy::Place(RandomIt first, RandomIt last, RngT& rng)
{
    assert(first != last);
    assert(std::distance(first, last) == env_.count());

    auto const [Min, Max] = env_.bounds().back();
    *(--last) = Max;

    do
    {
        std::for_each(first, last, [&](auto& x)
        {
            x = std::uniform_real_distribution<>{Min, Max}(rng);
        });

        Fix(first, last + 1);
    }
    while (!env_.is_valid(first, last + 1));
}

bool CS3910ParticleSwarmPolicy::Terminate()
{
    return params_.iterations < iteration_++;
}
