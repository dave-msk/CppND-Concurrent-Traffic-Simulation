#ifndef CONCURRENT_TRAFFIC_SIMULATION_VEHICLE_H_
#define CONCURRENT_TRAFFIC_SIMULATION_VEHICLE_H_

#include "traffic_object.h"

// Forward declarations to avoid include cycle
class Street;
class Intersection;

class Vehicle 
    : public TrafficObject,
      public std::enable_shared_from_this<Vehicle> {
 public:
  // Constructor
  Vehicle();

  // Getters & setters
  void SetCurrentStreet(std::shared_ptr<Street> street) {
    curr_street_ = street;
  }

  void SetCurrentDestination(std::shared_ptr<Intersection> destination);

  // Typical behaviour methods
  void Simulate();

  // miscellaneous
  std::shared_ptr<Vehicle> GetSharedThis() { return shared_from_this(); }

 private:
  // Typical behaviour methods
  void Drive();

  // Street the vehicle is currently on
  std::shared_ptr<Street> curr_street_;
  // Destination to which the vehicle is currently driving
  std::shared_ptr<Intersection> curr_destination_;
  double pos_street_;  // position on current street
  double speed_;  // ego speed in m/s
};

#endif  // CONCURRENT_TRAFFIC_SIMULATION_VEHICLE_H_
