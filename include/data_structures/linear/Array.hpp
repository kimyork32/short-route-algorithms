#pragma once

#include "../core/Common.hpp"
#include <array>

/**
 * @file Array.hpp
 * @brief Fixed-size array wrapper
 */

namespace ds {
    
    /**
     * @brief Fixed-size array container
     * @tparam T Element type
     * @tparam N Array size
     * 
     * Time Complexity:
     * - Access: O(1)
     * - Fill: O(n)
     * 
     * Space Complexity: O(n)
     */
    template<typename T, size_t N>
    class Array {
    private:
        std::array<T, N> data_;
        
    public:
        using value_type = T;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = T&;
        using const_reference = const T&;
        using pointer = T*;
        using const_pointer = const T*;
        using iterator = typename std::array<T, N>::iterator;
        using const_iterator = typename std::array<T, N>::const_iterator;
        using reverse_iterator = typename std::array<T, N>::reverse_iterator;
        using const_reverse_iterator = typename std::array<T, N>::const_reverse_iterator;
        
        // Constructors
        Array() = default;
        Array(const Array&) = default;
        Array(Array&&) = default;
        Array& operator=(const Array&) = default;
        Array& operator=(Array&&) = default;
        
        explicit Array(const T& value) { fill(value); }
        
        // Element access
        reference at(size_type pos) { return data_.at(pos); }
        const_reference at(size_type pos) const { return data_.at(pos); }
        
        reference operator[](size_type pos) { return data_[pos]; }
        const_reference operator[](size_type pos) const { return data_[pos]; }
        
        reference front() { return data_.front(); }
        const_reference front() const { return data_.front(); }
        
        reference back() { return data_.back(); }
        const_reference back() const { return data_.back(); }
        
        pointer data() noexcept { return data_.data(); }
        const_pointer data() const noexcept { return data_.data(); }
        
        // Iterators
        iterator begin() noexcept { return data_.begin(); }
        const_iterator begin() const noexcept { return data_.begin(); }
        const_iterator cbegin() const noexcept { return data_.cbegin(); }
        
        iterator end() noexcept { return data_.end(); }
        const_iterator end() const noexcept { return data_.end(); }
        const_iterator cend() const noexcept { return data_.cend(); }
        
        reverse_iterator rbegin() noexcept { return data_.rbegin(); }
        const_reverse_iterator rbegin() const noexcept { return data_.rbegin(); }
        const_reverse_iterator crbegin() const noexcept { return data_.crbegin(); }
        
        reverse_iterator rend() noexcept { return data_.rend(); }
        const_reverse_iterator rend() const noexcept { return data_.rend(); }
        const_reverse_iterator crend() const noexcept { return data_.crend(); }
        
        // Capacity
        bool empty() const noexcept { return N == 0; }
        size_type size() const noexcept { return N; }
        size_type max_size() const noexcept { return N; }
        
        // Operations
        void fill(const T& value) { data_.fill(value); }
        void swap(Array& other) noexcept { data_.swap(other.data_); }
    };
    
    // Non-member functions
    template<typename T, size_t N>
    bool operator==(const Array<T, N>& lhs, const Array<T, N>& rhs) {
        return lhs.data_ == rhs.data_;
    }
    
    template<typename T, size_t N>
    bool operator!=(const Array<T, N>& lhs, const Array<T, N>& rhs) {
        return !(lhs == rhs);
    }
    
    template<typename T, size_t N>
    void swap(Array<T, N>& lhs, Array<T, N>& rhs) noexcept {
        lhs.swap(rhs);
    }
    
} // namespace ds
