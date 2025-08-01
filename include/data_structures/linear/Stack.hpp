#pragma once

#include "../core/Common.hpp"
#include "Vector.hpp"

/**
 * @file Stack.hpp
 * @brief Stack implementation using Vector
 */

namespace ds {
    
    /**
     * @brief Stack (LIFO) container
     * @tparam T Element type
     * 
     * Time Complexity:
     * - Push: O(1) amortized
     * - Pop: O(1)
     * - Top: O(1)
     * 
     * Space Complexity: O(n)
     */
    template<typename T>
    class Stack {
    private:
        Vector<T> container_;
        
    public:
        using value_type = T;
        using size_type = size_t;
        using reference = T&;
        using const_reference = const T&;
        
        // Constructors
        Stack() = default;
        explicit Stack(size_t initial_capacity) : container_(initial_capacity) {}
        
        // Element access
        reference top() { return container_.back(); }
        const_reference top() const { return container_.back(); }
        
        // Capacity
        bool empty() const noexcept { return container_.empty(); }
        size_type size() const noexcept { return container_.size(); }
        
        // Modifiers
        void push(const T& value) { container_.push_back(value); }
        void push(T&& value) { container_.push_back(std::move(value)); }
        
        template<typename... Args>
        void emplace(Args&&... args) { container_.emplace_back(std::forward<Args>(args)...); }
        
        void pop() { container_.pop_back(); }
        
        void swap(Stack& other) noexcept { container_.swap(other.container_); }
    };
    
} // namespace ds
