#include <chrono>
#include <random>
#include <thread>

#include "traffic_light.h"
#include "traffic_object.h"

TrafficLight::TrafficLight()
    : TrafficObject(ObjectType::TRAFFIC_LIGHT),
      current_phase_(TrafficLightPhase::RED) {}

void TrafficLight::WaitForGreen() {
  while (phases_.Receive() != TrafficLightPhase::GREEN);
}

void TrafficLight::Simulate() {
  threads_.emplace_back(std::thread(&TrafficLight::CycleThroughPhases, this));
}

// FP.2a: Implement the function with an infinite loop that measures the time
//   between two loop cycles and toggles the current phase of the traffic light
//   between red and green and sends an update method to the message queue using
//   move semantics. The cycle duration should be a random value between 4 and 6
//   seconds. Also, the while-loop should use std::this_thread::sleep_for to
//   wait 1ms between two cycles.
void TrafficLight::CycleThroughPhases() {
  auto prev_toggle_time = std::chrono::system_clock::now();
  std::random_device rd;
  std::mt19937 eng(rd());
  std::uniform_int_distribution<> distr(4000, 6000);
  int duration = distr(eng);
  while (true) {
    auto curr_time = std::chrono::system_clock::now();
    long delta = std::chrono::duration_cast<std::chrono::milliseconds>(
        curr_time - prev_toggle_time).count();
    if (delta >= duration) {
      current_phase_ = (current_phase_ == TrafficLightPhase::RED) ?
                       TrafficLightPhase::GREEN : TrafficLightPhase::RED;
      phases_.Send(TrafficLightPhase(current_phase_));

      duration = distr(eng);
      prev_toggle_time = std::chrono::system_clock::now();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}