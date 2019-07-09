#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "graphics.h"
#include "intersection.h"
#include "street.h"
#include "vehicle.h"


// Paris
void CreateTrafficObjects_Paris(
    std::vector<std::shared_ptr<Street>> &streets,
    std::vector<std::shared_ptr<Intersection>> &intersections,
    std::vector<std::shared_ptr<Vehicle>> &vehicles,
    std::string &filename,
    int n_vehicles) {
  // Assign filename of corresponding city map
  filename = "../data/paris.jpg";

  // Init traffic objects
  int n_intersections = 9;
  for (std::size_t ni = 0; ni < n_intersections; ++ni) {
    intersections.push_back(std::make_shared<Intersection>());
  }

  // Position intersections in pixel coordinates (counter-clockwise)
  intersections.at(0)->SetPosition(385, 270);
  intersections.at(1)->SetPosition(1240, 80);
  intersections.at(2)->SetPosition(1625, 75);
  intersections.at(3)->SetPosition(2110, 75);
  intersections.at(4)->SetPosition(2840, 175);
  intersections.at(5)->SetPosition(3070, 680);
  intersections.at(6)->SetPosition(2800, 1400);
  intersections.at(7)->SetPosition(400, 1100);
  intersections.at(8)->SetPosition(1700, 900);  // Central plaza

  // Create streets and connect traffic objects
  int n_streets = 8;
  for (std::size_t ns = 0; ns < n_streets; ++ns) {
    streets.push_back(std::make_shared<Street>());
    streets.at(ns)->SetInIntersection(intersections.at(ns));
    streets.at(ns)->SetOutIntersection(intersections.at(8));
  }

  // Add vehicles to streets
  for (std::size_t nv = 0; nv < n_vehicles; ++nv) {
    vehicles.push_back(std::make_shared<Vehicle>());
    vehicles.at(nv)->SetCurrentStreet(streets.at(nv));
    vehicles.at(nv)->SetCurrentDestination(intersections.at(8));
  }
}

// NYC
void CreateTrafficObjects_NYC(
    std::vector<std::shared_ptr<Street>> &streets,
    std::vector<std::shared_ptr<Intersection>> &intersections,
    std::vector<std::shared_ptr<Vehicle>> &vehicles,
    std::string &filename,
    int n_vehicles) {
  // Assign filename of corresponding city map
  filename = "../data/nyc.jpg";

  // Init traffic objects
  int n_intersections = 6;
  for (std::size_t ni = 0; ni < n_intersections; ++ni) {
    intersections.push_back(std::make_shared<Intersection>());
  }

  // Position intersections in pixel coordinates
  intersections.at(0)->SetPosition(1430, 625);
  intersections.at(1)->SetPosition(2575, 1260);
  intersections.at(2)->SetPosition(2200, 1950);
  intersections.at(3)->SetPosition(1000, 1350);
  intersections.at(4)->SetPosition(400, 1000);
  intersections.at(5)->SetPosition(750, 250);

  // Create streets and connect traffic objects
  int n_streets = 7;
  for (std::size_t ns = 0; ns < n_streets; ++ns) {
    streets.push_back(std::make_shared<Street>());
  }

  streets.at(0)->SetInIntersection(intersections.at(0));
  streets.at(0)->SetOutIntersection(intersections.at(1));

  streets.at(1)->SetInIntersection(intersections.at(1));
  streets.at(1)->SetOutIntersection(intersections.at(2));

  streets.at(2)->SetInIntersection(intersections.at(2));
  streets.at(2)->SetOutIntersection(intersections.at(3));

  streets.at(3)->SetInIntersection(intersections.at(3));
  streets.at(3)->SetOutIntersection(intersections.at(4));
  
  streets.at(4)->SetInIntersection(intersections.at(4));
  streets.at(4)->SetOutIntersection(intersections.at(5));
  
  streets.at(5)->SetInIntersection(intersections.at(5));
  streets.at(5)->SetOutIntersection(intersections.at(0));
  
  streets.at(6)->SetInIntersection(intersections.at(0));
  streets.at(6)->SetOutIntersection(intersections.at(3));
  
  // Add vehicles to streets
  for (std::size_t nv = 0; nv < n_vehicles; ++nv) {
    vehicles.push_back(std::make_shared<Vehicle>());
    vehicles.at(nv)->SetCurrentStreet(streets.at(nv));
    vehicles.at(nv)->SetCurrentDestination(intersections.at(nv));
  }
}

// Main function
int main() {
  // PART 1: Set up traffic objects

  // Create and connect intersections and streets
  std::vector<std::shared_ptr<Street>> streets;
  std::vector<std::shared_ptr<Intersection>> intersections;
  std::vector<std::shared_ptr<Vehicle>> vehicles;
  std::string background_img;
  int n_vehicles = 6;
  CreateTrafficObjects_Paris(
      streets, intersections, vehicles, background_img, n_vehicles);
  
  // PART 2: Simulate traffic objects

  // Simulate intersection
  std::for_each(
      intersections.begin(), intersections.end(),
      [](std::shared_ptr<Intersection> &i) { i->Simulate(); });
  
  // Simulate vehicles
  std::for_each(
      vehicles.begin(), vehicles.end(),
      [](std::shared_ptr<Vehicle> &v) { v->Simulate(); });
  
  // PART 3: Launch visualization
  
  // Add all objects into common vector
  std::vector<std::shared_ptr<TrafficObject>> traffic_objects;
  std::for_each(
      intersections.begin(), intersections.end(),
      [&traffic_objects](std::shared_ptr<Intersection> &intersection) {
          auto traffic_object =
              std::dynamic_pointer_cast<TrafficObject>(intersection);
          traffic_objects.push_back(traffic_object);
      });
  
  std::for_each(
      vehicles.begin(), vehicles.end(),
      [&traffic_objects](std::shared_ptr<Vehicle> &vehicle) {
          auto traffic_object =
              std::dynamic_pointer_cast<TrafficObject>(vehicle);
          traffic_objects.push_back(traffic_object);
      });
  
  // Draw all objects in vector
  Graphics *graphics = new Graphics();
  graphics->SetBgFilename(background_img);
  graphics->SetTrafficObjects(traffic_objects);
  graphics->Simulate();
}
