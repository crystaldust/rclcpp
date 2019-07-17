// Copyright 2019 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef RCLCPP__BUFFERS__RING_BUFFER_IMPLEMENTATION_HPP_
#define RCLCPP__BUFFERS__RING_BUFFER_IMPLEMENTATION_HPP_

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <utility>
#include <vector>

#include "rclcpp/macros.hpp"
#include "rclcpp/visibility_control.hpp"

#include "iostream"

namespace rclcpp
{
namespace intra_process_buffer
{

template<typename BufferT>
class RingBufferImplementation : public BufferImplementationBase<BufferT>
{
public:
  explicit RingBufferImplementation(size_t size)
  : ring_buffer_(size)
  {
    buffer_size_ = size;
    write_ = buffer_size_ - 1;
    read_ = 0;
    length_ = 0;

    if (size == 0) {
      throw std::invalid_argument("size must be a positive, non-zero value");
    }
  }

  virtual ~RingBufferImplementation() {}

  void enqueue(BufferT request)
  {
    std::lock_guard<std::mutex> lock(mutex_);

    write_ = next(write_);
    ring_buffer_[write_] = std::move(request);

    if (is_full()) {
      read_ = next(read_);
    } else {
      length_++;
    }
  }

  BufferT dequeue()
  {
    assert(has_data());

    std::lock_guard<std::mutex> lock(mutex_);

    auto request = std::move(ring_buffer_[read_]);
    read_ = next(read_);

    length_--;

    return request;
  }

  inline uint32_t next(uint32_t val)
  {
    return (val + 1) % buffer_size_;
  }

  inline bool has_data() const
  {
    return length_ != 0;
  }

  inline bool is_full()
  {
    return length_ == buffer_size_;
  }

  void clear() {}

private:
  std::vector<BufferT> ring_buffer_;

  int write_;
  int read_;
  size_t length_;
  size_t buffer_size_;

  std::mutex mutex_;
};

}  // namespace intra_process_buffer
}  // namespace rclcpp

#endif  // RCLCPP__BUFFERS__RING_BUFFER_IMPLEMENTATION_HPP_