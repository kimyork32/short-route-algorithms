#pragma once

#include "../core/Common.hpp"
#include "../core/Traits.hpp"
#include "Vector.hpp"
#include <functional>
#include <algorithm>

/**
 * @file PriorityQueue.hpp
 * @brief High-performance priority queue optimized for pathfinding algorithms
 * 
 * This implementation uses a binary heap built on our optimized Vector class.
 * It's specifically optimized for the priority queue operations needed in
 * Dijkstra and A* algorithms where we frequently need to extract minimum elements.
 */

namespace ds {
    
    /**
     * @brief High-performance priority queue with binary heap
     * @tparam T Element type
     * @tparam Compare Comparison function (default: std::greater for min-heap)
     * 
     * Time Complexity:
     * - Push: O(log n)
     * - Pop: O(log n)
     * - Top: O(1)
     * - Size/Empty: O(1)
     * 
     * Space Complexity: O(n)
     * 
     * Features:
     * - Min-heap by default (suitable for pathfinding)
     * - Built on optimized Vector for cache locality
     * - Move semantics support
     * - Custom comparators
     * - Exception safety guarantees
     */
    template<
        typename T,
        typename Compare = std::greater<T>  // Min-heap by default
    >
    class PriorityQueue {
    public:
        using value_type = T;
        using size_type = size_t;
        using reference = T&;
        using const_reference = const T&;
        using compare_type = Compare;
        
    private:
        Vector<T> heap_;
        Compare comp_;
        
        /**
         * @brief Get parent index
         * @complexity O(1)
         */
        static constexpr size_t parent(size_t index) {
            return (index - 1) / 2;
        }
        
        /**
         * @brief Get left child index
         * @complexity O(1)
         */
        static constexpr size_t left_child(size_t index) {
            return 2 * index + 1;
        }
        
        /**
         * @brief Get right child index
         * @complexity O(1)
         */
        static constexpr size_t right_child(size_t index) {
            return 2 * index + 2;
        }
        
        /**
         * @brief Bubble up element to maintain heap property
         * @param index Index of element to bubble up
         * @complexity O(log n)
         */
        void bubble_up(size_t index) {
            while (index > 0) {
                size_t parent_idx = parent(index);
                
                // If heap property is satisfied, stop
                if (!comp_(heap_[index], heap_[parent_idx])) {
                    break;
                }
                
                // Swap with parent and continue
                std::swap(heap_[index], heap_[parent_idx]);
                index = parent_idx;
            }
        }
        
        /**
         * @brief Bubble down element to maintain heap property
         * @param index Index of element to bubble down
         * @complexity O(log n)
         */
        void bubble_down(size_t index) {
            size_t size = heap_.size();
            
            while (true) {
                size_t left_idx = left_child(index);
                size_t right_idx = right_child(index);
                size_t target_idx = index;
                
                // Find the element that should be at the top among parent and children
                if (left_idx < size && comp_(heap_[left_idx], heap_[target_idx])) {
                    target_idx = left_idx;
                }
                
                if (right_idx < size && comp_(heap_[right_idx], heap_[target_idx])) {
                    target_idx = right_idx;
                }
                
                // If heap property is satisfied, stop
                if (target_idx == index) {
                    break;
                }
                
                // Swap with target child and continue
                std::swap(heap_[index], heap_[target_idx]);
                index = target_idx;
            }
        }
        
        /**
         * @brief Build heap from existing elements
         * @complexity O(n)
         */
        void heapify() {
            if (heap_.size() <= 1) return;
            
            // Start from last internal node and bubble down
            size_t start = parent(heap_.size() - 1);
            for (size_t i = start + 1; i > 0; --i) {
                bubble_down(i - 1);
            }
        }
        
    public:
        // ========================
        // CONSTRUCTORS & DESTRUCTOR
        // ========================
        
        /**
         * @brief Default constructor
         * @complexity O(1)
         */
        explicit PriorityQueue(const Compare& comp = Compare())
            : heap_(), comp_(comp) {}
        
        /**
         * @brief Constructor with initial capacity
         * @complexity O(1)
         */
        explicit PriorityQueue(size_t initial_capacity, const Compare& comp = Compare())
            : heap_(initial_capacity), comp_(comp) {}
        
        /**
         * @brief Copy constructor
         * @complexity O(n)
         */
        PriorityQueue(const PriorityQueue& other)
            : heap_(other.heap_), comp_(other.comp_) {}
        
        /**
         * @brief Move constructor
         * @complexity O(1)
         */
        PriorityQueue(PriorityQueue&& other) noexcept
            : heap_(std::move(other.heap_)), comp_(std::move(other.comp_)) {}
        
        /**
         * @brief Initializer list constructor
         * @complexity O(n log n)
         */
        PriorityQueue(std::initializer_list<T> init, const Compare& comp = Compare())
            : heap_(init), comp_(comp) {
            heapify();
        }
        
        /**
         * @brief Range constructor
         * @complexity O(n log n)
         */
        template<typename InputIterator>
        PriorityQueue(InputIterator first, InputIterator last, const Compare& comp = Compare())
            : heap_(first, last), comp_(comp) {
            heapify();
        }
        
        // ========================
        // ASSIGNMENT OPERATORS
        // ========================
        
        PriorityQueue& operator=(const PriorityQueue& other) {
            if (this != &other) {
                heap_ = other.heap_;
                comp_ = other.comp_;
            }
            return *this;
        }
        
        PriorityQueue& operator=(PriorityQueue&& other) noexcept {
            if (this != &other) {
                heap_ = std::move(other.heap_);
                comp_ = std::move(other.comp_);
            }
            return *this;
        }
        
        PriorityQueue& operator=(std::initializer_list<T> init) {
            heap_ = init;
            heapify();
            return *this;
        }
        
        // ========================
        // ELEMENT ACCESS
        // ========================
        
        /**
         * @brief Access top element
         * @return Reference to top element
         * @throws EmptyContainerException if empty
         * @complexity O(1)
         */
        const_reference top() const {
            if (empty()) {
                throw EmptyContainerException("PriorityQueue is empty");
            }
            return heap_[0];
        }
        
        // ========================
        // MODIFIERS
        // ========================
        
        /**
         * @brief Push element
         * @param value Element to push
         * @complexity O(log n)
         */
        void push(const T& value) {
            heap_.push_back(value);
            bubble_up(heap_.size() - 1);
        }
        
        void push(T&& value) {
            heap_.push_back(std::move(value));
            bubble_up(heap_.size() - 1);
        }
        
        /**
         * @brief Emplace element
         * @param args Arguments to construct element
         * @complexity O(log n)
         */
        template<typename... Args>
        void emplace(Args&&... args) {
            heap_.emplace_back(std::forward<Args>(args)...);
            bubble_up(heap_.size() - 1);
        }
        
        /**
         * @brief Pop top element
         * @throws EmptyContainerException if empty
         * @complexity O(log n)
         */
        void pop() {
            if (empty()) {
                throw EmptyContainerException("Cannot pop from empty PriorityQueue");
            }
            
            if (heap_.size() == 1) {
                heap_.pop_back();
                return;
            }
            
            // Move last element to top and bubble down
            heap_[0] = std::move(heap_.back());
            heap_.pop_back();
            bubble_down(0);
        }
        
        /**
         * @brief Pop and return top element
         * @return Top element
         * @throws EmptyContainerException if empty
         * @complexity O(log n)
         */
        T extract_top() {
            T result = top();
            pop();
            return result;
        }
        
        /**
         * @brief Clear all elements
         * @complexity O(n)
         */
        void clear() {
            heap_.clear();
        }
        
        /**
         * @brief Swap contents with another PriorityQueue
         * @complexity O(1)
         */
        void swap(PriorityQueue& other) noexcept {
            heap_.swap(other.heap_);
            std::swap(comp_, other.comp_);
        }
        
        // ========================
        // CAPACITY
        // ========================
        
        bool empty() const noexcept { return heap_.empty(); }
        size_t size() const noexcept { return heap_.size(); }
        size_t capacity() const noexcept { return heap_.capacity(); }
        size_t max_size() const noexcept { return heap_.max_size(); }
        
        /**
         * @brief Reserve capacity
         * @param new_capacity Minimum capacity to reserve
         * @complexity O(1) amortized
         */
        void reserve(size_t new_capacity) {
            heap_.reserve(new_capacity);
        }
        
        /**
         * @brief Shrink capacity to fit size
         * @complexity O(n)
         */
        void shrink_to_fit() {
            heap_.shrink_to_fit();
        }
        
        // ========================
        // HEAP OPERATIONS
        // ========================
        
        /**
         * @brief Check if heap property is satisfied (for debugging)
         * @return true if heap is valid
         * @complexity O(n)
         */
        bool is_heap() const {
            for (size_t i = 0; i < heap_.size(); ++i) {
                size_t left_idx = left_child(i);
                size_t right_idx = right_child(i);
                
                if (left_idx < heap_.size() && comp_(heap_[left_idx], heap_[i])) {
                    return false;
                }
                
                if (right_idx < heap_.size() && comp_(heap_[right_idx], heap_[i])) {
                    return false;
                }
            }
            return true;
        }
        
        /**
         * @brief Get underlying container (for debugging)
         * @return Reference to underlying vector
         */
        const Vector<T>& get_container() const {
            return heap_;
        }
        
        // ========================
        // PATHFINDING OPTIMIZATIONS
        // ========================
        
        /**
         * @brief Bulk insert elements (optimized for pathfinding initialization)
         * @param elements Container of elements to insert
         * @complexity O(n + m log n) where m is number of new elements
         */
        template<typename Container>
        void bulk_push(const Container& elements) {
            heap_.reserve(heap_.size() + elements.size());
            
            for (const auto& element : elements) {
                heap_.push_back(element);
            }
            
            heapify();
        }
        
        /**
         * @brief Update priority of element (requires custom equality check)
         * This is useful for pathfinding when we need to update priorities
         * @param old_value Old value to find
         * @param new_value New value to replace with
         * @return true if element was found and updated
         * @complexity O(n) to find, O(log n) to reheapify
         */
        template<typename EqualityPredicate>
        bool update_priority(const T& new_value, EqualityPredicate pred) {
            auto it = std::find_if(heap_.begin(), heap_.end(), pred);
            if (it == heap_.end()) {
                return false;
            }
            
            size_t index = std::distance(heap_.begin(), it);
            T old_value = *it;
            heap_[index] = new_value;
            
            // Decide whether to bubble up or down
            if (comp_(new_value, old_value)) {
                bubble_up(index);
            } else {
                bubble_down(index);
            }
            
            return true;
        }
        
        /**
         * @brief Remove specific element (useful for pathfinding)
         * @param pred Predicate to find element
         * @return true if element was found and removed
         * @complexity O(n) to find, O(log n) to reheapify
         */
        template<typename Predicate>
        bool remove_if(Predicate pred) {
            auto it = std::find_if(heap_.begin(), heap_.end(), pred);
            if (it == heap_.end()) {
                return false;
            }
            
            size_t index = std::distance(heap_.begin(), it);
            
            // Move last element to this position
            if (index == heap_.size() - 1) {
                heap_.pop_back();
                return true;
            }
            
            heap_[index] = std::move(heap_.back());
            heap_.pop_back();
            
            // Reheapify from this position
            if (index > 0 && comp_(heap_[index], heap_[parent(index)])) {
                bubble_up(index);
            } else {
                bubble_down(index);
            }
            
            return true;
        }
        
        // ========================
        // DEBUGGING & STATISTICS
        // ========================
        
        /**
         * @brief Print heap structure (for debugging)
         */
        void debug_print() const {
            std::cout << "PriorityQueue (size=" << size() << ", capacity=" << capacity() << "):\n";
            
            if (empty()) {
                std::cout << "  [empty]\n";
                return;
            }
            
            // Print level by level
            size_t level = 0;
            size_t level_start = 0;
            size_t level_size = 1;
            
            while (level_start < heap_.size()) {
                std::cout << "  Level " << level << ": ";
                
                size_t level_end = std::min(level_start + level_size, heap_.size());
                for (size_t i = level_start; i < level_end; ++i) {
                    std::cout << heap_[i];
                    if (i < level_end - 1) std::cout << " ";
                }
                std::cout << "\n";
                
                level_start += level_size;
                level_size *= 2;
                level++;
            }
            
            std::cout << "  Is valid heap: " << (is_heap() ? "Yes" : "No") << std::endl;
        }
    };
    
    // ========================
    // NON-MEMBER FUNCTIONS
    // ========================
    
    template<typename T, typename Compare>
    void swap(PriorityQueue<T, Compare>& lhs, PriorityQueue<T, Compare>& rhs) noexcept {
        lhs.swap(rhs);
    }
    
    /**
     * @brief Make priority queue from container
     * @param container Container to make heap from
     * @param comp Comparison function
     * @return PriorityQueue containing elements
     * @complexity O(n)
     */
    template<typename Container, typename Compare = std::greater<typename Container::value_type>>
    PriorityQueue<typename Container::value_type, Compare> 
    make_priority_queue(const Container& container, const Compare& comp = Compare()) {
        return PriorityQueue<typename Container::value_type, Compare>(
            container.begin(), container.end(), comp
        );
    }
    
    // ========================
    // SPECIALIZED PRIORITY QUEUES FOR PATHFINDING
    // ========================
    
    /**
     * @brief Min-heap priority queue (default for pathfinding)
     */
    template<typename T>
    using MinPriorityQueue = PriorityQueue<T, std::greater<T>>;
    
    /**
     * @brief Max-heap priority queue
     */
    template<typename T>
    using MaxPriorityQueue = PriorityQueue<T, std::less<T>>;
    
    /**
     * @brief Priority queue for pairs (useful for Dijkstra/A*)
     * Compares by first element (priority), assumes pair<Priority, Data>
     */
    template<typename Priority, typename Data>
    using PathfindingPriorityQueue = PriorityQueue<
        std::pair<Priority, Data>,
        std::greater<std::pair<Priority, Data>>
    >;
    
} // namespace ds
