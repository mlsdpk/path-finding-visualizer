#pragma once

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>

template <class T>
class MessageQueue {
 public:
  T receive() {
    // perform queue modification under the lock
    std::unique_lock<std::mutex> uLock(mutex_);
    cond_.wait(uLock, [this] {
      return !messages_.empty();
    });  // pass unique lock to condition variable

    // remove last vector element from queue
    T msg = std::move(messages_.back());
    messages_.pop();

    return msg;  // will not be copied due to return value optimization (RVO) in
                 // C++
  };

  void send(T &&msg) {
    // perform vector modification under the lock
    std::lock_guard<std::mutex> uLock(mutex_);

    // add vector to queue
    messages_.push(std::move(msg));
    cond_.notify_one();  // notify client after pushing new Vehicle into vector
  };

 private:
  std::mutex mutex_;
  std::condition_variable cond_;
  std::queue<T> messages_;
};
