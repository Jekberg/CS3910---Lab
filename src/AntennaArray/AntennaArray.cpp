#define _USE_MATH_DEFINES

#include "AntennaArray.h"
#include <limits>

const double AntennaArray::MIN_SPACING = 0.25;

AntennaArray::AntennaArray(unsigned int n_ant, double steering_ang)
  : n_antennae(n_ant), steering_angle(steering_ang)
{}

std::size_t AntennaArray::count() const noexcept
{
    return n_antennae;
}

std::vector<Bounds> AntennaArray::bounds() const
{
  std::vector<Bounds> bnds{};
  while(bnds.size()<n_antennae)
    bnds.emplace_back(Bounds{0, ((double)n_antennae) / 2});
  return bnds;
}


