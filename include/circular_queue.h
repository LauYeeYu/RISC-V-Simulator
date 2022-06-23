// RISC-V Simulator
// Copyright (C) 2022 Lau Yee-Yu
//
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef RISC_V_SIMULATOR_INCLUDE_CIRCULAR_QUEUE_H
#define RISC_V_SIMULATOR_INCLUDE_CIRCULAR_QUEUE_H

template<class T, int kSize>
class CircularQueue {
public:
    CircularQueue() : head_(0), tail_(0) {}

    ~CircularQueue() = default;

    [[nodiscard]] bool Full() const {
        return (tail_ + 1) % kSize == head_;
    }

    [[nodiscard]] bool Empty() const {
        return tail_ == head_;
    }

    /**
     * Push an element into the queue.  Please check whether the queue is
     * full before calling this function.
     * @param element
     * @param data
     */
    void Push(const T& data) {
        queue_[tail_] = data;
        tail_ = (tail_ + 1) % kSize;
    }

    /**
     * Pop an element from the queue.  Please check whether the queue is
     * empty before calling this function.
     * @return the element popped from the queue.
     */
    void Pop() {
        queue_[head_].~T();
        head_ = (head_ + 1) % kSize;
    }

    /**
     * Get the element at the head of the queue by absolute index.
     * @return the element at the head of the queue.
     */
    T& operator[](int index) {
        return queue_[index];
    }

private:
    T queue_[kSize];
    int head_;
    int tail_;
};

#endif //RISC_V_SIMULATOR_INCLUDE_CIRCULAR_QUEUE_H
