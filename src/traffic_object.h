#ifndef CONCURRENT_TRAFFIC_SIMULATION_TRAFFIC_OBJECT_H_
#define CONCURRENT_TRAFFIC_SIMULATION_TRAFFIC_OBJECT_H_

#include <mutex>
#include <vector>
#include <thread>

enum class ObjectType {
  NO_OBJECT,
  VEHICLE,
  INTERSECTION,
  STREET,
  TRAFFIC_LIGHT
};

class TrafficObject {
 public:
  // constructor & destructor
  TrafficObject();
  TrafficObject(ObjectType);
  ~TrafficObject();

  // getter and setter
  int GetID() { return id_; }
  void SetPosition(double, double);
  void GetPosition(double &, double &);
  ObjectType GetType() { return type_; }

  virtual void Simulate() {};

 protected:
  static std::mutex mtx_cout_;  // mutex for cout

  ObjectType type_;  // identifies the class type
  int id_;  // every traffic object has its own unique id
  double posX_, posY_;  // vehicle position in pixels
  std::vector<std::thread> threads_; // holds all threads that have been
                                     // launched within this object
 private:
  static int id_cnt_;  // global variable for counting object ids
};

#endif  // CONCURRENT_TRAFFIC_SIMULATION_TRAFFIC_OBJECT_H_