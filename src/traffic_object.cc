#include <chrono>
#include <iostream>

#include "traffic_object.h"

// init static variable
std::mutex TrafficObject::mtx_cout_;
int TrafficObject::id_cnt_ = 0;

TrafficObject::TrafficObject() : type_(ObjectType::NO_OBJECT), id_(id_cnt_++) {}

TrafficObject::TrafficObject(ObjectType type) : type_(type), id_(id_cnt_++) {}

void TrafficObject::SetPosition(double x, double y) {
  posX_ = x;
  posY_ = y;
}

void TrafficObject::GetPosition(double &x, double &y) {
  x = posX_;
  y = posY_;
}

TrafficObject::~TrafficObject() {
  for (auto &t : threads_)
    t.join();
}
