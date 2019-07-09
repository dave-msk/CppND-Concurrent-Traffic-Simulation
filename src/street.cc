#include <memory>

#include "intersection.h"
#include "street.h"
#include "traffic_object.h"

Street::Street() : TrafficObject(ObjectType::STREET),
                   length_(1000.0) {}  // in m

void Street::SetInIntersection(std::shared_ptr<Intersection> in) {
  inter_in_ = in;
  // Add this street to list of streets connected to the intersection
  in->AddStreet(GetSharedThis());
}

void Street::SetOutIntersection(std::shared_ptr<Intersection> out) {
  inter_out_ = out;
  // Add this street to list of streets connected to the intersection
  out->AddStreet(GetSharedThis());
}
