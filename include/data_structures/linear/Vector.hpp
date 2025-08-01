#pragma once

#include "../core/Common.hpp"
#include "../core/Traits.hpp"
#include <memory>
#include <iterator>
#include <algorithm>
#include <initializer_list>

/**
 * @file Vector.hpp
 * @brief High-performance dynamic array optimized for pathfinding
 * 
 * This implementation provides a std::vector-like interface with optimizations
 * specific to pathfinding algorithms that frequently store paths and node lists.
 */

namespace ds {
    
    /**
     * @brief High-performance dynamic array with RAII and move semantics
     * @tparam T Element type
     * 
     * Time Complexity:
     * - Random access: O(1)
     * - Push back: O(1) amortized
     * - Pop back: O(1)
     * - Insert: O(n)
     * - Erase: O(n)
     * 
     * Space Complexity: O(n)
     * 
     * Features:
     * - Exception-safe RAII design
     * - Move semantics support
     * - Optimized growth strategy
     * - Cache-friendly memory layout
     * - Strong exception safety guarantees
     */
    template<typename T>
    class Vector {
    public:
        using value_type = T;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = T&;
        using const_reference = const T&;
        using pointer = T*;
        using const_pointer = const T*;
        
    private:
        pointer data_;
        size_type size_;
        size_type capacity_;
        
        /**
         * @brief Allocate raw memory
         * @param count Number of elements to allocate
         * @return Pointer to allocated memory
         * @complexity O(1)
         */
        pointer allocate(size_type count) {
            if (count == 0) return nullptr;
            
            if (count > max_size()) {
                throw AllocationException("Vector allocation size too large");
            }
            
            return static_cast<pointer>(::operator new(count * sizeof(T)));
        }
        
        /**
         * @brief Deallocate raw memory
         * @param ptr Pointer to memory to deallocate
         * @param count Number of elements (for debugging)
         * @complexity O(1)
         */
        void deallocate(pointer ptr, size_type count = 0) noexcept {
            (void)count; // Suppress unused parameter warning
            if (ptr) {
                ::operator delete(ptr);
            }
        }
        
        /**
         * @brief Construct element in place
         * @param ptr Pointer to construct at
         * @param args Arguments for construction
         * @complexity O(1)
         */
        template<typename... Args>
        void construct(pointer ptr, Args&&... args) {
            new (ptr) T(std::forward<Args>(args)...);
        }
        
        /**
         * @brief Destroy element
         * @param ptr Pointer to element to destroy
         * @complexity O(1)
         */
        void destroy(pointer ptr) noexcept {
            ptr->~T();
        }
        
        /**
         * @brief Destroy range of elements
         * @param first First element to destroy
         * @param last One past last element to destroy
         * @complexity O(n)
         */
        void destroy_range(pointer first, pointer last) noexcept {
            for (pointer current = first; current != last; ++current) {
                destroy(current);
            }
        }
        
        /**
         * @brief Calculate new capacity for growth
         * @param min_capacity Minimum required capacity
         * @return New capacity
         * @complexity O(1)
         */
        size_type calculate_growth(size_type min_capacity) const {
            const size_type max_cap = max_size();
            if (min_capacity > max_cap) {
                throw AllocationException("Vector growth exceeds max_size");
            }
            
            const size_type current_cap = capacity_;
            if (current_cap > max_cap / 2) {
                return max_cap;
            }
            
            const size_type doubled = current_cap * 2;
            return std::max(doubled, min_capacity);
        }
        
        /**
         * @brief Reallocate with new capacity
         * @param new_capacity New capacity
         * @complexity O(n)
         */
        void reallocate(size_type new_capacity) {
            if (new_capacity < size_) {
                throw std::invalid_argument("New capacity less than current size");
            }
            
            if (new_capacity == capacity_) {
                return;
            }
            
            pointer new_data = allocate(new_capacity);
            
            // Move/copy construct elements in new location
            pointer new_current = new_data;
            pointer old_current = data_;
            
            try {
                for (size_type i = 0; i < size_; ++i, ++new_current, ++old_current) {
                    if constexpr (std::is_nothrow_move_constructible_v<T>) {
                        construct(new_current, std::move(*old_current));
                    } else {
                        construct(new_current, *old_current);
                    }
                }
            } catch (...) {
                // Cleanup constructed elements
                destroy_range(new_data, new_current);
                deallocate(new_data, new_capacity);
                throw;
            }
            
            // Destroy old elements and deallocate
            destroy_range(data_, data_ + size_);
            deallocate(data_, capacity_);
            
            data_ = new_data;
            capacity_ = new_capacity;
        }
        
        /**
         * @brief Insert gap in the middle of vector
         * @param pos Position to insert gap
         * @param count Number of elements in gap
         * @complexity O(n)
         */
        void insert_gap(pointer pos, size_type count) {
            const size_type pos_index = pos - data_;
            const size_type old_size = size_;
            const size_type new_size = old_size + count;
            
            if (new_size > capacity_) {
                const size_type new_capacity = calculate_growth(new_size);
                pointer new_data = allocate(new_capacity);
                
                // Copy elements before insertion point
                pointer new_current = new_data;
                for (size_type i = 0; i < pos_index; ++i, ++new_current) {
                    construct(new_current, std::move(data_[i]));
                }
                
                // Skip gap (will be filled by caller)
                new_current += count;
                
                // Copy elements after insertion point
                for (size_type i = pos_index; i < old_size; ++i, ++new_current) {
                    construct(new_current, std::move(data_[i]));
                }
                
                // Cleanup old data
                destroy_range(data_, data_ + size_);
                deallocate(data_, capacity_);
                
                data_ = new_data;
                capacity_ = new_capacity;
            } else {
                // Move elements to make space
                const size_type elements_to_move = old_size - pos_index;
                
                // Move construct elements that go beyond current end
                const size_type elements_beyond_end = std::min(count, elements_to_move);
                for (size_type i = 0; i < elements_beyond_end; ++i) {
                    construct(data_ + old_size + count - 1 - i, 
                             std::move(data_[old_size - 1 - i]));
                }
                
                // Move assign elements that stay within current range
                if (elements_to_move > elements_beyond_end) {
                    std::move_backward(data_ + pos_index, 
                                     data_ + old_size - elements_beyond_end,
                                     data_ + new_size - elements_beyond_end);
                }
            }
            
            size_ = new_size;
        }
        
    public:
        // ========================
        // ITERATOR CLASSES
        // ========================
        
        class iterator {
        private:
            pointer ptr_;
            
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = T;
            using difference_type = ptrdiff_t;
            using pointer = T*;
            using reference = T&;
            
            explicit iterator(pointer ptr = nullptr) : ptr_(ptr) {}
            
            reference operator*() const { return *ptr_; }
            pointer operator->() const { return ptr_; }
            
            iterator& operator++() { ++ptr_; return *this; }
            iterator operator++(int) { iterator temp = *this; ++ptr_; return temp; }
            iterator& operator--() { --ptr_; return *this; }
            iterator operator--(int) { iterator temp = *this; --ptr_; return temp; }
            
            iterator& operator+=(difference_type n) { ptr_ += n; return *this; }
            iterator& operator-=(difference_type n) { ptr_ -= n; return *this; }
            
            iterator operator+(difference_type n) const { return iterator(ptr_ + n); }
            iterator operator-(difference_type n) const { return iterator(ptr_ - n); }
            
            difference_type operator-(const iterator& other) const { return ptr_ - other.ptr_; }
            
            reference operator[](difference_type n) const { return ptr_[n]; }
            
            bool operator==(const iterator& other) const { return ptr_ == other.ptr_; }
            bool operator!=(const iterator& other) const { return ptr_ != other.ptr_; }
            bool operator<(const iterator& other) const { return ptr_ < other.ptr_; }
            bool operator<=(const iterator& other) const { return ptr_ <= other.ptr_; }
            bool operator>(const iterator& other) const { return ptr_ > other.ptr_; }
            bool operator>=(const iterator& other) const { return ptr_ >= other.ptr_; }
        };
        
        class const_iterator {
        private:
            const_pointer ptr_;
            
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = T;
            using difference_type = ptrdiff_t;
            using pointer = const T*;
            using reference = const T&;
            
            explicit const_iterator(const_pointer ptr = nullptr) : ptr_(ptr) {}
            const_iterator(const iterator& it) : ptr_(it.operator->()) {}
            
            reference operator*() const { return *ptr_; }
            pointer operator->() const { return ptr_; }
            
            const_iterator& operator++() { ++ptr_; return *this; }
            const_iterator operator++(int) { const_iterator temp = *this; ++ptr_; return temp; }
            const_iterator& operator--() { --ptr_; return *this; }
            const_iterator operator--(int) { const_iterator temp = *this; --ptr_; return temp; }
            
            const_iterator& operator+=(difference_type n) { ptr_ += n; return *this; }
            const_iterator& operator-=(difference_type n) { ptr_ -= n; return *this; }
            
            const_iterator operator+(difference_type n) const { return const_iterator(ptr_ + n); }
            const_iterator operator-(difference_type n) const { return const_iterator(ptr_ - n); }
            
            difference_type operator-(const const_iterator& other) const { return ptr_ - other.ptr_; }
            
            reference operator[](difference_type n) const { return ptr_[n]; }
            
            bool operator==(const const_iterator& other) const { return ptr_ == other.ptr_; }
            bool operator!=(const const_iterator& other) const { return ptr_ != other.ptr_; }
            bool operator<(const const_iterator& other) const { return ptr_ < other.ptr_; }
            bool operator<=(const const_iterator& other) const { return ptr_ <= other.ptr_; }
            bool operator>(const const_iterator& other) const { return ptr_ > other.ptr_; }
            bool operator>=(const const_iterator& other) const { return ptr_ >= other.ptr_; }
        };
        
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        
        // ========================
        // CONSTRUCTORS & DESTRUCTOR
        // ========================
        
        /**
         * @brief Default constructor
         * @complexity O(1)
         */
        Vector() : data_(nullptr), size_(0), capacity_(0) {}
        
        /**
         * @brief Constructor with initial capacity
         * @complexity O(1)
         */
        explicit Vector(size_type initial_capacity) 
            : data_(allocate(initial_capacity)), size_(0), capacity_(initial_capacity) {}
        
        /**
         * @brief Constructor with size and default value
         * @complexity O(n)
         */
        explicit Vector(size_type count, const T& value = T()) 
            : data_(allocate(count)), size_(count), capacity_(count) {
            size_type i = 0;
            try {
                for (i = 0; i < count; ++i) {
                    construct(data_ + i, value);
                }
            } catch (...) {
                destroy_range(data_, data_ + i);
                deallocate(data_, capacity_);
                throw;
            }
        }
        
        /**
         * @brief Copy constructor
         * @complexity O(n)
         */
        Vector(const Vector& other) 
            : data_(allocate(other.capacity_)), size_(other.size_), capacity_(other.capacity_) {
            size_type i = 0;
            try {
                for (i = 0; i < size_; ++i) {
                    construct(data_ + i, other.data_[i]);
                }
            } catch (...) {
                destroy_range(data_, data_ + i);
                deallocate(data_, capacity_);
                throw;
            }
        }
        
        /**
         * @brief Move constructor
         * @complexity O(1)
         */
        Vector(Vector&& other) noexcept 
            : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        
        /**
         * @brief Initializer list constructor
         * @complexity O(n)
         */
        Vector(std::initializer_list<T> init) 
            : data_(allocate(init.size())), size_(init.size()), capacity_(init.size()) {
            auto current = data_;
            try {
                for (const auto& item : init) {
                    construct(current++, item);
                }
            } catch (...) {
                destroy_range(data_, current);
                deallocate(data_, capacity_);
                throw;
            }
        }
        
        /**
         * @brief Range constructor
         * @complexity O(n)
         */
        template<typename InputIterator>
        Vector(InputIterator first, InputIterator last) {
            if constexpr (is_random_access_iterator_v<InputIterator>) {
                const size_type count = std::distance(first, last);
                data_ = allocate(count);
                size_ = count;
                capacity_ = count;
                
                auto current = data_;
                try {
                    for (auto it = first; it != last; ++it, ++current) {
                        construct(current, *it);
                    }
                } catch (...) {
                    destroy_range(data_, current);
                    deallocate(data_, capacity_);
                    throw;
                }
            } else {
                // For non-random access iterators, grow incrementally
                data_ = nullptr;
                size_ = 0;
                capacity_ = 0;
                
                for (auto it = first; it != last; ++it) {
                    push_back(*it);
                }
            }
        }
        
        /**
         * @brief Destructor
         * @complexity O(n)
         */
        ~Vector() {
            destroy_range(data_, data_ + size_);
            deallocate(data_, capacity_);
        }
        
        // ========================
        // ASSIGNMENT OPERATORS
        // ========================
        
        Vector& operator=(const Vector& other) {
            if (this != &other) {
                Vector temp(other);
                swap(temp);
            }
            return *this;
        }
        
        Vector& operator=(Vector&& other) noexcept {
            if (this != &other) {
                destroy_range(data_, data_ + size_);
                deallocate(data_, capacity_);
                
                data_ = other.data_;
                size_ = other.size_;
                capacity_ = other.capacity_;
                
                other.data_ = nullptr;
                other.size_ = 0;
                other.capacity_ = 0;
            }
            return *this;
        }
        
        Vector& operator=(std::initializer_list<T> init) {
            Vector temp(init);
            swap(temp);
            return *this;
        }
        
        // ========================
        // ELEMENT ACCESS
        // ========================
        
        reference at(size_type pos) {
            if (pos >= size_) {
                throw OutOfBoundsException("Vector index out of bounds");
            }
            return data_[pos];
        }
        
        const_reference at(size_type pos) const {
            if (pos >= size_) {
                throw OutOfBoundsException("Vector index out of bounds");
            }
            return data_[pos];
        }
        
        reference operator[](size_type pos) { return data_[pos]; }
        const_reference operator[](size_type pos) const { return data_[pos]; }
        
        reference front() { return data_[0]; }
        const_reference front() const { return data_[0]; }
        
        reference back() { return data_[size_ - 1]; }
        const_reference back() const { return data_[size_ - 1]; }
        
        pointer data() noexcept { return data_; }
        const_pointer data() const noexcept { return data_; }
        
        // ========================
        // MODIFIERS
        // ========================
        
        void push_back(const T& value) {
            if (size_ == capacity_) {
                reallocate(calculate_growth(size_ + 1));
            }
            construct(data_ + size_, value);
            ++size_;
        }
        
        void push_back(T&& value) {
            if (size_ == capacity_) {
                reallocate(calculate_growth(size_ + 1));
            }
            construct(data_ + size_, std::move(value));
            ++size_;
        }
        
        template<typename... Args>
        reference emplace_back(Args&&... args) {
            if (size_ == capacity_) {
                reallocate(calculate_growth(size_ + 1));
            }
            construct(data_ + size_, std::forward<Args>(args)...);
            ++size_;
            return back();
        }
        
        void pop_back() {
            if (size_ > 0) {
                --size_;
                destroy(data_ + size_);
            }
        }
        
        iterator insert(const_iterator pos, const T& value) {
            const size_type pos_index = pos - cbegin();
            insert_gap(data_ + pos_index, 1);
            construct(data_ + pos_index, value);
            return iterator(data_ + pos_index);
        }
        
        iterator insert(const_iterator pos, T&& value) {
            const size_type pos_index = pos - cbegin();
            insert_gap(data_ + pos_index, 1);
            construct(data_ + pos_index, std::move(value));
            return iterator(data_ + pos_index);
        }
        
        iterator insert(const_iterator pos, size_type count, const T& value) {
            const size_type pos_index = pos - cbegin();
            insert_gap(data_ + pos_index, count);
            
            for (size_type i = 0; i < count; ++i) {
                construct(data_ + pos_index + i, value);
            }
            
            return iterator(data_ + pos_index);
        }
        
        template<typename InputIterator>
        iterator insert(const_iterator pos, InputIterator first, InputIterator last) {
            const size_type pos_index = pos - cbegin();
            const size_type count = std::distance(first, last);
            insert_gap(data_ + pos_index, count);
            
            auto current = data_ + pos_index;
            for (auto it = first; it != last; ++it, ++current) {
                construct(current, *it);
            }
            
            return iterator(data_ + pos_index);
        }
        
        iterator erase(const_iterator pos) {
            const size_type pos_index = pos - cbegin();
            destroy(data_ + pos_index);
            
            // Move elements down
            for (size_type i = pos_index; i < size_ - 1; ++i) {
                construct(data_ + i, std::move(data_[i + 1]));
                destroy(data_ + i + 1);
            }
            
            --size_;
            return iterator(data_ + pos_index);
        }
        
        iterator erase(const_iterator first, const_iterator last) {
            const size_type first_index = first - cbegin();
            const size_type last_index = last - cbegin();
            const size_type count = last_index - first_index;
            
            // Destroy elements in range
            destroy_range(data_ + first_index, data_ + last_index);
            
            // Move remaining elements down
            for (size_type i = first_index; i < size_ - count; ++i) {
                construct(data_ + i, std::move(data_[i + count]));
                destroy(data_ + i + count);
            }
            
            size_ -= count;
            return iterator(data_ + first_index);
        }
        
        void clear() {
            destroy_range(data_, data_ + size_);
            size_ = 0;
        }
        
        void swap(Vector& other) noexcept {
            std::swap(data_, other.data_);
            std::swap(size_, other.size_);
            std::swap(capacity_, other.capacity_);
        }
        
        // ========================
        // CAPACITY
        // ========================
        
        bool empty() const noexcept { return size_ == 0; }
        size_type size() const noexcept { return size_; }
        size_type capacity() const noexcept { return capacity_; }
        size_type max_size() const noexcept { return SIZE_MAX / sizeof(T); }
        
        void reserve(size_type new_capacity) {
            if (new_capacity > capacity_) {
                reallocate(new_capacity);
            }
        }
        
        void shrink_to_fit() {
            if (capacity_ > size_) {
                reallocate(size_);
            }
        }
        
        void resize(size_type new_size, const T& value = T()) {
            if (new_size < size_) {
                destroy_range(data_ + new_size, data_ + size_);
            } else if (new_size > size_) {
                if (new_size > capacity_) {
                    reallocate(calculate_growth(new_size));
                }
                for (size_type i = size_; i < new_size; ++i) {
                    construct(data_ + i, value);
                }
            }
            size_ = new_size;
        }
        
        // ========================
        // ITERATORS
        // ========================
        
        iterator begin() noexcept { return iterator(data_); }
        const_iterator begin() const noexcept { return const_iterator(data_); }
        const_iterator cbegin() const noexcept { return const_iterator(data_); }
        
        iterator end() noexcept { return iterator(data_ + size_); }
        const_iterator end() const noexcept { return const_iterator(data_ + size_); }
        const_iterator cend() const noexcept { return const_iterator(data_ + size_); }
        
        reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
        
        reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
        const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }
    };
    
    // ========================
    // NON-MEMBER FUNCTIONS
    // ========================
    
    template<typename T>
    bool operator==(const Vector<T>& lhs, const Vector<T>& rhs) {
        return lhs.size() == rhs.size() && 
               std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }
    
    template<typename T>
    bool operator!=(const Vector<T>& lhs, const Vector<T>& rhs) {
        return !(lhs == rhs);
    }
    
    template<typename T>
    bool operator<(const Vector<T>& lhs, const Vector<T>& rhs) {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), 
                                          rhs.begin(), rhs.end());
    }
    
    template<typename T>
    bool operator<=(const Vector<T>& lhs, const Vector<T>& rhs) {
        return !(rhs < lhs);
    }
    
    template<typename T>
    bool operator>(const Vector<T>& lhs, const Vector<T>& rhs) {
        return rhs < lhs;
    }
    
    template<typename T>
    bool operator>=(const Vector<T>& lhs, const Vector<T>& rhs) {
        return !(lhs < rhs);
    }
    
    template<typename T>
    void swap(Vector<T>& lhs, Vector<T>& rhs) noexcept {
        lhs.swap(rhs);
    }
    
} // namespace ds
