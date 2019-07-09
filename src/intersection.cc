#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <utility>

#include "intersection.h"
#include "street.h"
#include "traffic_light.h"
#include "traffic_object.h"
#include "vehicle.h"

// Implementation of class "WaitingVehicles"

int WaitingVehicles::GetSize() {
  std::lock_guard<std::mutex> lock(mtx_);
  return vehicles_.size();
}

void WaitingVehicles::PushBack(std::shared_ptr<Vehicle> vehicle,
                               std::promise<void> &&promise) {
  std::lock_guard<std::mutex> lock(mtx_);
  vehicles_.push_back(vehicle);
  promises_.push_back(std::move(promise));
}

void WaitingVehicles::PermitEntryToFirstInQueue() {
  std::lock_guard<std::mutex> losck(mtx_);

  // Get entries from the front of both queues
  auto p = promises_.begin();
  auto v = vehicles_.end();

  // Fulfill promise
  p->set_value();
  
  // Remove front elements from both queues
  vehicles_.erase(v);
  promises_.erase(p);
}

// Implementation of class "Intersection"

Intersection::Intersection()
    : TrafficObject(ObjectType::INTERSECTION),
      is_blocked_(false) {}

void Intersection::AddStreet(std::shared_ptr<Street> street) {
  streets_.push_back(street);
}

std::vector<std::shared_ptr<Street>> Intersection::QueryStreets(
    std::shared_ptr<Street> incoming) {
  // Store all outgoing streets in a vector ...
  std::vector<std::shared_ptr<Street>> outgoings;
  for (auto it : streets_) {
    if (incoming->GetID() != it->GetID()) {  // exclude incoming street
      outgoings.push_back(it);
    }
  }

  return outgoings;
}

// Adds a new vehicle to the queue and returns once the
// vehicle is allowed to enter
void Intersection::AddVehicleToQueue(std::shared_ptr<Vehicle> vehicle) {
  std::unique_lock<std::mutex> lock(mtx_cout_);
  std::cout << "Intersection #" << id_ << "::AddVehicleToQueue: thread id = "
      << std::this_thread::get_id() << std::endl;
  lock.unlock();
  
  std::promise<void> p_vehicle_allowed;
  auto ftr_vehicle_allowed = p_vehicle_allowed.get_future();
  waiting_vehicles_.PushBack(vehicle, std::move(p_vehicle_allowed));

  ftr_vehicle_allowed.wait();
  lock.lock();
  std::cout << "Intersection #" << id_ << ": Vehicle #" << vehicle->GetID()
      << " is granted entry." << std::endl;
  lock.unlock();

  while (!TrafficLightIsGreen())
    traffic_light_.WaitForGreen();
}

void Intersection::VehicleHasLeft(std::shared_ptr<Vehicle> vehicle) {
  // std::cout << "Intersection #" << id_ << ": Vehicle #"
  //     << vehicle->GetID() << " has left." << std::endl;
  
  // Unblock queue processing
  this->SetIsBlocked(false);
}

void Intersection::SetIsBlocked(bool is_blocked) {
  is_blocked_ = is_blocked;
  // std::cout << "Intersection #" << id_ << " is_blocked_="
  //     << is_blocked << std::endl;
}

// Virtual function which is executed in a thread
// Using threads + promises/futures + exceptions
void Intersection::Simulate() {
  // Start the simulation of TrafficLight
  traffic_light_.Simulate();

  // Launch vehicle queue processing in a thread
  threads_.emplace_back(std::thread(&Intersection::ProcessVehicleQueue, this));
}

void Intersection::ProcessVehicleQueue() {
  // Print id of the current thread
  // std::cout << "Intersection # " << id_ 
  //   << "::ProcessVehicleQueue: thread id = " << std::this_thread::get_id()
  //   << std::endl;

  // Continuously process the vehicle queue
  while (true) {
    // Sleep at every iteration to reduce CPU usage
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // Only proceed when at least one vehicle is waiting in the queue
    if (waiting_vehicles_.GetSize() > 0 && !is_blocked_) {
      // Set interation to "blocked" to prevent other vehicles from entering
      this->SetIsBlocked(true);

      // Permit entry to first vehicle in the queue (FIFO)
      waiting_vehicles_.PermitEntryToFirstInQueue();
    }
  }
}

bool Intersection::TrafficLightIsGreen() {
  return traffic_light_.GetCurrentPhase() == TrafficLightPhase::GREEN;
}