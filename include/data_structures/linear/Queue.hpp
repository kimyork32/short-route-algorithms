#pragma once

#include "../core/Common.hpp"
#include "Vector.hpp"

/**
 * @file Queue.hpp
 * @brief Queue implementation using circular buffer
 */

namespace ds {
    
    /**
     * @brief Queue (FIFO) container
     * @tparam T Element type
     * 
     * Time Complexity:
     * - Push: O(1) amortized
     * - Pop: O(1)
     * - Front/Back: O(1)
     * 
     * Space Complexity: O(n)
     */
    template<typename T>
    class Queue {
    private:
        Vector<T> container_;
        size_t front_index_;
        size_t size_;
        
        void ensure_capacity() {
            if (size_ == container_.capacity()) {
                // Resize and defragment
                Vector<T> new_container(container_.capacity() * 2);
                for (size_t i = 0; i < size_; ++i) {
                    new_container.push_back(std::move(container_[(front_index_ + i) % container_.capacity()]));
                }
                container_ = std::move(new_container);
                front_index_ = 0;
            }
        }
        
    public:
        using value_type = T;
        using size_type = size_t;
        using reference = T&;
        using const_reference = const T&;
        
        // Constructors
        Queue() : container_(16), front_index_(0), size_(0) {}
        explicit Queue(size_t initial_capacity) : container_(initial_capacity), front_index_(0), size_(0) {}
        
        // Element access
        reference front() { return container_[front_index_]; }
        const_reference front() const { return container_[front_index_]; }
        
        reference back() { return container_[(front_index_ + size_ - 1) % container_.capacity()]; }
        const_reference back() const { return container_[(front_index_ + size_ - 1) % container_.capacity()]; }
        
        // Capacity
        bool empty() const noexcept { return size_ == 0; }
        size_type size() const noexcept { return size_; }
        
        // Modifiers
        void push(const T& value) {
            ensure_capacity();
            container_[(front_index_ + size_) % container_.capacity()] = value;
            ++size_;
        }
        
        void push(T&& value) {
            ensure_capacity();
            container_[(front_index_ + size_) % container_.capacity()] = std::move(value);
            ++size_;
        }
        
        template<typename... Args>
        void emplace(Args&&... args) {
            ensure_capacity();
            container_[(front_index_ + size_) % container_.capacity()] = T(std::forward<Args>(args)...);
            ++size_;
        }
        
        void pop() {
            if (!empty()) {
                front_index_ = (front_index_ + 1) % container_.capacity();
                --size_;
            }
        }
        
        void swap(Queue& other) noexcept {
            container_.swap(other.container_);
            std::swap(front_index_, other.front_index_);
            std::swap(size_, other.size_);
        }
    };
    
} // namespace ds
