#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <string>
#include <vector>

#include <opencv2/core.hpp>

#include "traffic_object.h"

class Graphics {
 public:
  
  // Getters & setters
  void SetBgFilename(std::string filename) { bg_filename_ = filename; }
  void SetTrafficObjects(
      std::vector<std::shared_ptr<TrafficObject>> &traffic_objects) {
        traffic_objects_ = traffic_objects;
  }
  
  void Simulate();

 private:
  // Typical behaviour methods
  void LoadBackgroundImg();
  void DrawTrafficObjects();

  // Member variables
  std::vector<std::shared_ptr<TrafficObject>> traffic_objects_;
  std::string bg_filename_;
  std::string window_name_;
  std::vector<cv::Mat> images_;
};

#endif
