#ifndef CS3910__SIMULATION_H_
#define CS3910__SIMULATION_H_

#include <utility>

template<typename SimulationPolicy>
class Simulation final: private SimulationPolicy
{
public:
    template<typename... Args>
    explicit Simulation(Args&&... args)
        noexcept(noexcept(SimulationPolicy{std::forward<Args>(args)...}));

    void Run();
};

template<typename SimulationPolicy>
template<typename... Args>
Simulation<SimulationPolicy>::Simulation(Args&&... args)
    noexcept(noexcept(SimulationPolicy{std::forward<Args>(args)...}))
    : SimulationPolicy{std::forward<Args>(args)...}
{
}

template<typename SimulationPolicy>
void Simulation<SimulationPolicy>::Run()
{
    SimulationPolicy::Initialise();
    while(!SimulationPolicy::Terminate())
        SimulationPolicy::Step();
    SimulationPolicy::Complete();
};

#endif // !CS3910__SIMULATION_H_
