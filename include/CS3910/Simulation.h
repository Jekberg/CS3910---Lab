#ifndef CS3910__SIMULATION_H_
#define CS3910__SIMULATION_H_

template<typename SimulationPolicy>
void Simulate(SimulationPolicy&& policy)
{
    policy.Initialise();
    while(!policy.Terminate())
        policy.Step();
    policy.Complete();
};

#endif // !CS3910__SIMULATION_H_
