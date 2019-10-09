#define _USE_MATH_DEFINES

#include "AntennaArray.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <exception>
#include <string>

const double AntennaArray::MIN_SPACING = 0.25;

AntennaArray::AntennaArray(unsigned int n_ant, double steering_ang)
  : n_antennae(n_ant), steering_angle(steering_ang)
{}

std::vector<std::vector<double> > AntennaArray::bounds()
{
  std::vector<std::vector<double> > bnds;
  std::vector<double> dim_bnd = {0,((double)n_antennae)/2};
  while(bnds.size()<n_antennae)
    bnds.push_back(dim_bnd);
  return bnds;
}

#include<iostream>

bool AntennaArray::is_valid(std::vector<double> design)
{
  if(design.size() != n_antennae) return false;
  std::sort(std::begin(design),std::end(design));
  //Aperture size is exactly n_antennae/2
  if(fabs(design.back()-((double)n_antennae/2))>1e-10)
    return false;
  //All antennae lie within the problem bounds
  for(size_t i = 0;i<design.size()-1;++i)
    if(design[i] < bounds()[i][0] || design[i] > bounds()[i][1] )
      return false;
  //All antennae are separated by at least MIN_SPACING
  for(size_t i = 0;i<design.size()-1;++i)
    if(design[i+1] - design[i] < MIN_SPACING)
      return false;
  return true;
}

struct PowerPeak{
  PowerPeak(const PowerPeak&) = default;

  PowerPeak(double e,double p) : elevation(e), power(p) {}
  double elevation;
  double power;
};

double AntennaArray::evaluate(std::vector<double> design)
{
  if(design.size() != n_antennae)
    throw std::runtime_error(
      "AntennaArray::evaluate called on design of the wrong size. Expected: "+
      std::to_string(n_antennae)+
      ". Actual: "+
      std::to_string(design.size())
    );
  if(!is_valid(design)) return std::numeric_limits<double>::max();

  std::vector<PowerPeak> peaks;
  
  PowerPeak prev(0.0,std::numeric_limits<double>::min());
  PowerPeak current(0.0,array_factor(design,0.0));
  for(double elevation = 0.01; elevation <= 180.0; elevation += 0.01){
    PowerPeak next(elevation,array_factor(design,elevation));
    if(current.power >= prev.power && current.power >= next.power)
      peaks.push_back(current);
    prev = current;
    current = next;
  }
  peaks.push_back({180.0,array_factor(design,180.0)});

  std::sort(
    std::begin(peaks),
    std::end(peaks),
    [](PowerPeak l,PowerPeak r){return l.power > r.power;}
  );

  //No side-lobes case
  if(peaks.size()<2) return std::numeric_limits<double>::min();
  //Filter out main lobe and then return highest lobe level
  const double distance_from_steering = abs(peaks[0].elevation - steering_angle);
  for(size_t i=1;i<peaks.size();++i)
    if(abs(peaks[i].elevation - steering_angle) < distance_from_steering)
      return peaks[0].power;
  return peaks[1].power;
}

double AntennaArray::array_factor(std::vector<double> design, double elevation)
{
    double steering = 2*M_PI*steering_angle/360;
    elevation = 2*M_PI*elevation/360;
    double sum = 0.0;
    for(const auto& x : design){
      sum += cos(2*M_PI*x*(cos(elevation)-cos(steering)));
    }
    return 20*log(fabs(sum));
}
