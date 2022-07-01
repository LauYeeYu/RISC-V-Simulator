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

#include "type.h"

template<class T, int kSize>
class CircularQueue {
    friend class Iterator;
    friend class ConstIterator;
public:
    class Iterator {
        friend class CircularQueue;
    public:
        Iterator(const Iterator&) = default;
        Iterator(Iterator&&) = default;
        Iterator& operator=(const Iterator&) noexcept = default;
        Iterator& operator=(Iterator&&) noexcept = default;
        ~Iterator() = default;

        [[nodiscard]] T& operator*() { return queue_[index_]; }
        [[nodiscard]] T* operator->() { return &(queue_[index_]); }

        Iterator& operator++() {
            index_ = (index_ + 1) % kSize;
            return *this;
        }
        Iterator operator++(int) {
            Iterator tmp(*this);
            ++*this;
            return tmp;
        }

        Iterator& operator--() {
            index_ = (index_ - 1 + kSize) % kSize;
            return *this;
        }
        Iterator operator--(int) {
            Iterator tmp(*this);
            --*this;
            return tmp;
        }

        [[nodiscard]] bool operator==(const Iterator& rhs) const {
            return &(queue_) == &(rhs.queue_) && index_ == rhs.index_;
        }
        [[nodiscard]] bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }

    private:
        Iterator(CircularQueue<T, kSize>& queue, int index)
            : queue_(queue), index_(index) {}

        CircularQueue<T, kSize>& queue_;
        SizeType index_ = 0;
    };

    class ConstIterator {
        friend class CircularQueue;
    public:
        ConstIterator(const ConstIterator&) = default;
        ConstIterator(ConstIterator&&) = default;
        ConstIterator& operator=(const ConstIterator&) = default;
        ConstIterator& operator=(ConstIterator&&) noexcept = default;
        ~ConstIterator() = default;

        [[nodiscard]] const T& operator*() const { return queue_[index_]; }
        [[nodiscard]] const T* operator->() const { return &(queue_[index_]); }

        Iterator& operator++() {
            index_ = (index_ + 1) % kSize;
            return *this;
        }
        Iterator operator++(int) {
            Iterator tmp(*this);
            ++*this;
            return tmp;
        }

        Iterator& operator--() {
            index_ = (index_ - 1 + kSize) % kSize;
            return *this;
        }
        Iterator operator--(int) {
            Iterator tmp(*this);
            --*this;
            return tmp;
        }

        [[nodiscard]] bool operator==(const Iterator& rhs) const {
            return &(queue_) == &(rhs.queue_) && index_ == rhs.index_;
        }
        [[nodiscard]] bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }

    private:
        ConstIterator(const CircularQueue<T, kSize>& queue, SizeType index)
            : queue_(queue), index_(index) {}

        const CircularQueue<T, kSize>& queue_;
        SizeType index_ = 0;
    };

    CircularQueue() : head_(0), tail_(0) {}

    CircularQueue(const CircularQueue&) = default;
    CircularQueue(CircularQueue&&) = default;

    CircularQueue& operator=(const CircularQueue&) = default;
    CircularQueue& operator=(CircularQueue&&) = default;

    ~CircularQueue() = default;

    [[nodiscard]] bool Full() const { return (tail_ + 2) % kSize == head_; }

    [[nodiscard]] bool Empty() const { return tail_ == head_; }

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

    void Clear() {
        head_ = 0;
        tail_ = 0;
    }

    T& Front() { return queue_[head_]; }

    /**
     * Get the element at the head of the queue by absolute index.
     * @return the element at the head of the queue.
     */
    T& operator[](SizeType index) { return queue_[index]; }

    const T& operator[](SizeType index) const { return queue_[index]; }

    [[nodiscard]] SizeType HeadIndex() const { return head_; }
    [[nodiscard]] SizeType TailIndex() const { return (tail_ - 1 + kSize) % kSize; }
    [[nodiscard]] constexpr SizeType MaxSize() const { return kSize; }

    Iterator Begin() { return Iterator(*this, head_); }
    Iterator begin() { return Iterator(*this, head_); }
    Iterator End() { return Iterator(*this, tail_); }
    Iterator end() { return Iterator(*this, tail_); }
    ConstIterator Begin() const { return ConstIterator(*this, head_); }
    ConstIterator End() const { return ConstIterator(*this, tail_); }

    void SetAsEnd(SizeType index) { tail_ = index; }

private:
    T        queue_[kSize];
    SizeType head_;
    SizeType tail_;
};

#endif //RISC_V_SIMULATOR_INCLUDE_CIRCULAR_QUEUE_H
