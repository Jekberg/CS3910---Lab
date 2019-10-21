#include "AntennaArray.h"
#include "PSO.h"

template<typename ParticlePolicy>
void particleSwarmOptimization(AntennaArray& arr);

int main(int argc, char const** argv)
{
    if(argc < 3)
        std::cout
            << "Minimum number of arguments is 2.\n"
            << "The first argument is the number of antennae\n"
            << "The second argument is the steering angle\n"
            << "\n\n"
            << "Running PSO with 3 antennae and 90.0 steering angle...\n";
    AntennaArray arr{9, 90.0};

    std::cout << "Running...\n";
    particleSwarmOptimization<DefaultParticlePolicy>(arr);
}