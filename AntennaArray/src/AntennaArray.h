#ifndef ANTENNA_ARRAY_H
#define ANTENNA_ARRAY_H

#include <vector>

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
  /*!
   * @brief Rectangular bounds on the search space.
   * @return Vector b such that b[i][0] is the minimum permissible value of the
   * ith solution component and b[i][1] is the maximum.
   */
  std::vector<std::vector<double> > bounds();
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
  bool is_valid(std::vector<double> design);
  /*!
   * @brief Evaluate an antenna design returning peak SSL.
   * Designs which violate problem constraints will be penalised with extremely
   * high costs.
   * @param design A valid antenna array design.
   */
  double evaluate(std::vector<double> design);
private:
  const unsigned int n_antennae;
  const double steering_angle;

  double array_factor(std::vector<double>,double);
};

#endif /* ANTENNA_ARRAY_H */
