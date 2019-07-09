#ifndef CONCURRENT_TRAFFIC_SIMULATION_TRAFFIC_LIGHT_H_
#define CONCURRENT_TRAFFIC_SIMULATION_TRAFFIC_LIGHT_H_

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <utility>

#include "message_queue.h"
#include "traffic_object.h"

class Vehicle;

enum class TrafficLightPhase {
  RED,
  GREEN
};

class TrafficLight : TrafficObject {
 public:
  TrafficLight();
  
  void WaitForGreen();
  void Simulate();

  TrafficLightPhase GetCurrentPhase() { return current_phase_; }

 private:
  void CycleThroughPhases();

  MessageQueue<TrafficLightPhase> phases_;
  TrafficLightPhase current_phase_;

  std::condition_variable cond_;
  std::mutex mtx_;
};

#endif  // CONCURRENT_TRAFFIC_SIMULATION_TRAFFIC_LIGHT_H_
