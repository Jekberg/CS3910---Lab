#ifndef ANTENNA_ARRAY_H
#define ANTENNA_ARRAY_H

#define _USE_MATH_DEFINES


#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>
#include <limits>
#include <vector>

struct Bounds
{
    double min;
    double max;
};

//! Antenna array design problem
class AntennaArray{
public:
  //! Minimum spacing permitted between antennae.
  static const double MIN_SPACING;
  
  /*!
   * @brief Construct an antenna design problem.
   * @param n_ant Number of antennae in our array.
   * @param steering_ang Desired direction of the main beam in degrees.
   */
  AntennaArray(unsigned int n_ant,double steering_ang = 90);

  std::size_t count() const noexcept;

  /*!
   * @brief Rectangular bounds on the search space.
   * @return Vector b such that b[i][0] is the minimum permissible value of the
   * ith solution component and b[i][1] is the maximum.
   */
  std::vector<Bounds> bounds() const;
  /*!
   * @brief Check whether an antenna design lies within the problem's feasible
   * region.
   * A design is a vector of n_antennae anntena placements.
   * A placement is a distance from the left hand side of the antenna array.
   * A valid placement is one in which
   *   1) all antennae are separated by at least MIN_SPACING
   *   2) the aperture size (the maximum element of the array) is exactly
   *      n_antennae/2.
   */
  template<typename RandomIt>
  constexpr bool is_valid(RandomIt first, RandomIt last) const;
  /*!
   * @brief Evaluate an antenna design returning peak SSL.
   * Designs which violate problem constraints will be penalised with extremely
   * high costs.
   * @param design A valid antenna array design.
   */
  template<typename RandomIt>
  constexpr double evaluate(RandomIt first, RandomIt last);
private:
  const unsigned int n_antennae;
  const double steering_angle;

  template<typename RandomIt>
  constexpr double array_factor(RandomIt first, RandomIt last, double);
};

#include <iostream>

template<typename RandomIt>
constexpr bool AntennaArray::is_valid(RandomIt first, RandomIt last) const
{
    assert(std::is_sorted(first, last));
    assert(std::distance(first, last) == n_antennae);
    //Aperture size is exactly n_antennae/2
    if (fabs(last[-1] - ((double)n_antennae / 2)) > 1e-10)
        return false;
    //All antennae lie within the problem bounds
    for (size_t i = 0; i < n_antennae - 1; ++i)
    {
        if (first[i] < bounds()[i].min || first[i] > bounds()[i].max)
            return false;
    //All antennae are separated by at least MIN_SPACING
        if (first[i + 1] - first[i] < MIN_SPACING)
            return false;
    }
    return true;
}

namespace internal {
    struct PowerPeak {
        PowerPeak(double e, double p) : elevation(e), power(p) {}
        double elevation;
        double power;
    };
}

template<typename RandomIt>
constexpr double AntennaArray::evaluate(RandomIt first, RandomIt last)
{
    assert(std::distance(first, last) == n_antennae
         && "AntennaArray::evaluate called on design of the wrong size.");
    if (!is_valid(first, last)) return std::numeric_limits<double>::max();

    std::vector<internal::PowerPeak> peaks;

    internal::PowerPeak prev(0.0, std::numeric_limits<double>::min());
    internal::PowerPeak current(0.0, array_factor(first, last, 0.0));
    for (double elevation = 0.01; elevation <= 180.0; elevation += 0.01) {
        internal::PowerPeak next(elevation, array_factor(first, last, elevation));
        if (current.power >= prev.power && current.power >= next.power)
            peaks.push_back(current);
        prev = current;
        current = next;
    }
    peaks.push_back({ 180.0,array_factor(first, last,180.0) });

    std::sort(
        std::begin(peaks),
        std::end(peaks),
        [](auto&& l, auto&& r) {return l.power > r.power; }
    );

    //No side-lobes case
    if (peaks.size() < 2) return std::numeric_limits<double>::min();
    //Filter out main lobe and then return highest lobe level
    const double distance_from_steering = abs(peaks[0].elevation - steering_angle);
    for (size_t i = 1; i < peaks.size(); ++i)
        if (abs(peaks[i].elevation - steering_angle) < distance_from_steering)
            return peaks[0].power;
    return peaks[1].power;
}

template<typename RandomIt>
constexpr double AntennaArray::array_factor(RandomIt first, RandomIt last, double elevation)
{
    double steering = 2 * M_PI * steering_angle / 360;
    elevation = 2 * M_PI * elevation / 360;

    auto sum = std::accumulate(first, last, 0.0, [=](auto sum, auto x) -> auto {
        return sum + cos(2 * M_PI * x * (cos(elevation) - cos(steering)));
    });

    return 20 * log(fabs(sum));
}


#endif /* ANTENNA_ARRAY_H */
