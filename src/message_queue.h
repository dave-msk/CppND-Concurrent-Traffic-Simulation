#ifndef CONCURRENT_TRAFFIC_SIMULATION_MESSAGE_QUEUE_H_
#define CONCURRENT_TRAFFIC_SIMULATION_MESSAGE_QUEUE_H_

#include <condition_variable>
#include <deque>
#include <mutex>
#include <utility>

template<typename T>
class MessageQueue {
 public:
  void Send(T&& message) {
    std::lock_guard<std::mutex> lock(mtx_);
    queue_.push_back(std::move(message));
    cond_.notify_one();
  }

  T Receive() {
    std::unique_lock<std::mutex> lock(mtx_);
    cond_.wait(lock, [this]{ return !queue_.empty(); });
    T message = std::move(queue_.front());
    queue_.pop_front();
    return message;
  }

 private:
  std::deque<T> queue_;

  std::condition_variable cond_;
  std::mutex mtx_;
};

#endif  // CONCURRENT_TRAFFIC_SIMULATION_MESSAGE_QUEUE_H_