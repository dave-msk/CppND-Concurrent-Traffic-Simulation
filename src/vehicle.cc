#include <cmath>
#include <iostream>
#include <mutex>
#include <random>

#include "street.h"
#include "intersection.h"
#include "traffic_object.h"
#include "vehicle.h"

Vehicle::Vehicle() : TrafficObject(ObjectType::VEHICLE),
                     curr_street_(nullptr),
                     pos_street_(0.0),
                     speed_(400) {}

void Vehicle::SetCurrentDestination(std::shared_ptr<Intersection> destination) {
  // Update destination
  curr_destination_ = destination;

  // Reset simulation parameters
  pos_street_ = 0.0;
}

void Vehicle::Simulate() {
  // Launch drive function in a thread
  threads_.emplace_back(std::thread(&Vehicle::Drive, this));
}

// Virtual function which is executed in a thread
void Vehicle::Drive() {
  // Print id of the current thread
  std::unique_lock<std::mutex> lck(mtx_cout_);
  std::cout << "Vehicle #" << id_ << "::Drive: thread id = "
      << std::this_thread::get_id() << std::endl;
  lck.unlock();
  
  // Initialize variables
  bool has_entered_intersection = false;
  double cycle_duration = 1;  // duration of a single simulation cycle in ms
  std::chrono::time_point<std::chrono::system_clock> last_update;

  // Init stop watch
  last_update = std::chrono::system_clock::now();
  while (true) {
    // Sleep at every iteration to reduce CPU usage.
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // Compute time different to stop watch
    long time_since_last_update =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - last_update).count();
    if (time_since_last_update >= cycle_duration) {
      // Update position with a constant velocity motion model
      pos_street_ += speed_ * time_since_last_update / 1000;

      // Compute completion rate of current street
      double completion = pos_street_ / curr_street_->GetLength();

      // Compute current pixel position on street base on driving direction
      std::shared_ptr<Intersection> i1, i2;
      i2 = curr_destination_;
      i1 = (i2->GetID() == curr_street_->GetInIntersection()->GetID()) ?
           curr_street_->GetOutIntersection() :
           curr_street_->GetInIntersection();
      double x1, y1, x2, y2, xv, yv, dx, dy, l;
      i1->GetPosition(x1, y1);
      i2->GetPosition(x2, y2);
      dx = x2 - x1;
      dy = y2 - y1;
      l = std::sqrt(dx*dx + dy*dy);
      // new position based on line equation in parameter form
      xv = x1 + completion * dx;
      yv = y1 + completion * dy;
      this->SetPosition(xv, yv);

      // Check whether halting position in front of destination has been reached
      if (completion >= 0.9 && !has_entered_intersection) {
        auto ftr_entry_granted = std::async(&Intersection::AddVehicleToQueue,
                                            curr_destination_,
                                            GetSharedThis());
        ftr_entry_granted.get();

        // Slow down and set intersection flag
        speed_ /= 10.0;
        has_entered_intersection = true;
      }

      if (completion >= 1.0 && has_entered_intersection) {
        // Choose next street and destination
        auto street_options = curr_destination_->QueryStreets(curr_street_);
        std::shared_ptr<Street> next_street;
        if (street_options.size() > 0) {
          // Pick one street at random and query intersection
          // to enter this street
          std::random_device rd;
          std::mt19937 eng(rd());
          std::uniform_int_distribution<> distr(0, street_options.size() - 1);
          next_street = street_options.at(distr(eng));
        } else {
          // This street is a dead-end, so drive back the same way
          next_street = curr_street_;
        }

        // Pick the one intersection at which the vehicle is currently not
        auto next_intersection = (next_street->GetInIntersection()->GetID() ==
                                  curr_destination_->GetID()) ?
                                 next_street->GetOutIntersection() :
                                 next_street->GetInIntersection();
        // Send signal to intersection that vehicle has left the intersection
        curr_destination_->VehicleHasLeft(GetSharedThis());

        // Assign new street and destination
        this->SetCurrentDestination(next_intersection);
        this->SetCurrentStreet(next_street);

        // Reset speed and intersection flag
        speed_ *= 10.0;
        has_entered_intersection = false;
      }

      // Reset stop watch for next cycle.
      last_update = std::chrono::system_clock::now();
    } // eof simulation loop
  }
}
