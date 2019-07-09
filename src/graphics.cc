#include <iostream>
#include <cmath>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "graphics.h"
#include "intersection.h"

void Graphics::Simulate() {
  this->LoadBackgroundImg();
  while (true) {
    // Sleep at every iteration to reduce CPU usage
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // Update graphics
    this->DrawTrafficObjects();
  }
}

void Graphics::LoadBackgroundImg() {
  // Create window
  window_name_ = "Concurrency Traffic Simulation";
  cv::namedWindow(window_name_, cv::WINDOW_NORMAL);

  // Load image and create copy to be used for semi-transparent overlay
  cv::Mat background = cv::imread(bg_filename_);

  images_.push_back(background);  // original background
  images_.push_back(background.clone());  // transparent overlay
  images_.push_back(background.clone());  // result image for display
}

void Graphics::DrawTrafficObjects() {
  // Reset images
  images_.at(1) = images_.at(0).clone();
  images_.at(2) = images_.at(0).clone();

  // Create overlay from all traffic objects
  for (auto it : traffic_objects_) {
    double posx, posy;
    it->GetPosition(posx, posy);

    if (it->GetType() == ObjectType::INTERSECTION) {
      // Case object type from TrafficObject to Intersection
      std::shared_ptr<Intersection> intersection =
          std::dynamic_pointer_cast<Intersection>(it);
      
      // Set color according to traffic light and draw the intersection
      // as a circle.
      cv::Scalar color = (intersection->TrafficLightIsGreen()) ?
                         cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
      cv::circle(images_.at(1), cv::Point2d(posx, posy), 25, color, -1);
    } else if (it->GetType() == ObjectType::VEHICLE) {
      // Ensure that length of color vector is always 255
      cv::RNG rng(it->GetID());
      int b = rng.uniform(0, 255);
      int g = rng.uniform(0, 255);
      int r = (int) std::sqrt(255*255 - g*g - b*b);
      cv::Scalar vehicle_color = cv::Scalar(b, g, r);
      cv::circle(images_.at(1), cv::Point2d(posx, posy), 50, vehicle_color, -1);
    }
  }

  float opacity = 0.85;
  cv::addWeighted(
      images_.at(1), opacity, images_.at(0), 1.0 - opacity, 0, images_.at(2));
  
  // Display background and overlay image
  cv::imshow(window_name_, images_.at(2));
  cv::waitKey(33);
}