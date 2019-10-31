#include "CS3910/AntennaArray.h"
#include "CS3910/Simulation.h"
#include <cmath>
#include <memory>
#include <random>
#include <iterator>
#include <valarray>

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
    };

    explicit CS3910ParticleSwarmPolicy(AntennaArray& env) noexcept;

    void Initialise();

    void Step();

    void Complete()
    {
    }

    bool Terminate();
private:
    // Inertia
    inline static double const n = 1.0 / (2.0 * std::log(2));

    // 
    inline static double const o1 = 1.0 / 2.0 + std::log(2);

    // 
    inline static double const o2 = 1.0 / 2.0 + std::log(2);

    AntennaArray& env_;

    std::unique_ptr<value_type[]> population_;

    double bestSLL_;

    Vector bestPosition_;

    std::size_t const populationSize_;

    std::size_t iteration_;

    std::minstd_rand0 rng_{};

    void Update(
        double* position,
        double* velocity,
        double const* personalBest,
        double const* globalBest);

    void UpdateBest()
    {
        auto it = std::find_if(
            population_.get(),
            population_.get() + populationSize_,
            [=](auto& particle) noexcept
            {
                return particle.sll < bestSLL_;
            });

        if (it != population_.get() + populationSize_)
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
    void Place(RandomIt first, RandomIt last);

    std::valarray<double> RandomVec();

};

int main(int argc, char const** argv)
{
    if(argc < 3)
        std::cout
            << "Minimum number of arguments is 2.\n"
            << "The first argument is the number of antennae\n"
            << "The second argument is the steering angle\n"
            << "\n\n"
            << "Running PSO with 3 antennae and 90.0 steering angle...\n";
    AntennaArray arr{20, 90.0};


    std::cout << "Running...\n";
    Simulation<CS3910ParticleSwarmPolicy>{arr}.Run();
}

CS3910ParticleSwarmPolicy::CS3910ParticleSwarmPolicy(AntennaArray& env)
    noexcept
    : env_{env}
    , populationSize_{static_cast<std::size_t>(20 + std::sqrt(env.count()))}
{
}

void CS3910ParticleSwarmPolicy::Initialise()
{
    iteration_ = 0;
    bestSLL_ = std::numeric_limits<double>::infinity();
    bestPosition_ = std::make_unique<double[]>(env_.count());

    population_ = std::make_unique<value_type[]>(populationSize_);
    std::for_each_n(population_.get(), populationSize_, [&](auto& particle)
    {
        particle.position = std::make_unique<double[]>(env_.count());
        Place(particle.position.get(), particle.position.get() + env_.count());

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
    std::for_each_n(population_.get(), populationSize_, [&](auto& particle)
    {
        // Move particle
        Update(
            particle.position.get(),
            particle.velocity.get(),
            particle.bestPosition.get(),
            bestPosition_.get());

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

void CS3910ParticleSwarmPolicy::Update(
    double* position,
    double* velocity,
    double const* personalBest,
    double const* globalBest)
{
    std::valarray<double> const v{velocity, env_.count() - 1};
    std::valarray<double> const x{position, env_.count() - 1};
    std::valarray<double> const p{personalBest, env_.count() - 1};
    std::valarray<double> const g{globalBest, env_.count() - 1};
    auto const r1 = RandomVec();
    auto const r2 = RandomVec();

    auto newV = n * v + o1 * r1 *(p - x) + o2 * r2 * (g - x);
    std::copy(std::begin(newV), std::end(newV), velocity);
    std::transform(
        std::begin(newV),
        std::end(newV),
        position,
        position,
        std::plus<double>{});

    std::sort(position, position + env_.count() - 1);

    auto [min, max] = env_.bounds().back();

    if (max <= position[env_.count() - 2])
        std::for_each_n(position, env_.count() - 1, [=](auto& x)
            {
                x /= max;
            });
}

template<typename RandomIt>
void CS3910ParticleSwarmPolicy::Place(RandomIt first, RandomIt last)
{
    assert(first != last);
    assert(std::distance(first, last) == env_.count());

    first;
    *(--last) = env_.bounds().back().max;

    do
    {
        double lowerBound{ (env_.count() - 1)  * AntennaArray::MIN_SPACING };
        for (auto i{ last - 1}; first != i; --i)
        {
            *i = std::uniform_real_distribution<>{lowerBound, i[1] - AntennaArray::MIN_SPACING }(rng_);
            lowerBound -= AntennaArray::MIN_SPACING;
        }
    }
    while (!env_.is_valid(first, last + 1));
}

bool CS3910ParticleSwarmPolicy::Terminate()
{
    return 10000 < iteration_++;
}

std::valarray<double> CS3910ParticleSwarmPolicy::RandomVec()
{
    auto temp{std::make_unique<double[]>(env_.count() - 1)};
    std::uniform_real_distribution<> dist{0.0, 1.0};
    std::for_each_n(temp.get(), env_.count() - 1, [&](auto& x)
    {
        x = dist(rng_);
    });
    return std::valarray<double>{temp.get(), env_.count() - 1};
}