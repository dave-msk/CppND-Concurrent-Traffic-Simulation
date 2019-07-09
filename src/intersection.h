#ifndef CONCURRENT_TRAFFIC_SIMULATION_INTERSECTION_H_
#define CONCURRENT_TRAFFIC_SIMULATION_INTERSECTION_H_

#include <future>
#include <memory>
#include <mutex>
#include <vector>

#include "traffic_light.h"
#include "traffic_object.h"

// Forward declarations to avoid include cycle
class Street;
class Vehicle;

// Auxiliary class to queue and dequeue waiting vehicles in a thread-safe manner
class WaitingVehicles {
 public:
  // Getters / setters
  int GetSize();

  // Typical behavior methods
  void PushBack(std::shared_ptr<Vehicle> vehicle,
                std::promise<void> &&promise);
  void PermitEntryToFirstInQueue();

 private:
  // List of all vehicles waiting to enter this intersection
  std::vector<std::shared_ptr<Vehicle>> vehicles_;
  std::vector<std::promise<void>> promises_;  // List of associated promises
  std::mutex mtx_;
};

class Intersection : public TrafficObject {
 public:
  // Constructor
  Intersection();

  // Getters / setters
  void SetIsBlocked(bool is_blocked);

  // Typical behaviour methods
  void AddStreet(std::shared_ptr<Street> street);
  void AddVehicleToQueue(std::shared_ptr<Vehicle> vehicle);
  void Simulate();
  void VehicleHasLeft(std::shared_ptr<Vehicle> vehicle);
  bool TrafficLightIsGreen();
  // Return pointer to current list of all outgoing streets
  std::vector<std::shared_ptr<Street>> QueryStreets(
      std::shared_ptr<Street> incoming);
  
 private:
  // Typical behaviour methods
  void ProcessVehicleQueue();

  // List of all streets connected to this intersection
  std::vector<std::shared_ptr<Street>> streets_;
  // List of all vehicles and their associated promises waiting to enter
  WaitingVehicles waiting_vehicles_;
  // Traffic light
  TrafficLight traffic_light_;
  bool is_blocked_;  // whether the intersection is blocked by a vehicle
};

#endif  // CONCURRENT_TRAFFIC_SIMULATION_INTERSECTION_H_
