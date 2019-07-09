#ifndef CONCURRENT_TRAFFIC_SIMULATION_STREET_H_
#define CONCURRENT_TRAFFIC_SIMULATION_STREET_H_

#include <memory>

#include "traffic_object.h"

// Forward declaration to avoid include cycle
class Intersection;

class Street : public TrafficObject,
               public std::enable_shared_from_this<Street> {
 public:
  // Constructor
  Street();

  // Getters & setters
  double GetLength() { return length_; }
  void SetInIntersection(std::shared_ptr<Intersection>);
  void SetOutIntersection(std::shared_ptr<Intersection>);
  std::shared_ptr<Intersection> GetInIntersection() { return inter_in_; }
  std::shared_ptr<Intersection> GetOutIntersection() { return inter_out_; }

  // typical behaviour methods

  // miscellaneous
  std::shared_ptr<Street> GetSharedThis() { return shared_from_this(); }

 private:
  double length_;  // length of this street in m

  // Intersections from which a vehicle can enter
  // (one-way streets are always from 'in' to 'out')
  std::shared_ptr<Intersection> inter_in_;
  std::shared_ptr<Intersection> inter_out_;
};

#endif  // CONCURRENT_TRAFFIC_SIMULATION_STREET_H_
