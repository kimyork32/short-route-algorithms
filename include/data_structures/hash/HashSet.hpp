#pragma once

#include "HashMap.hpp"

/**
 * @file HashSet.hpp
 * @brief High-performance hash set built on top of HashMap
 * 
 * This implementation leverages the optimized HashMap with Robin Hood hashing
 * for excellent performance in pathfinding algorithms where we need to track
 * visited nodes (closed set) efficiently.
 */

namespace ds {
    
    /**
     * @brief High-performance hash set for pathfinding
     * @tparam Key Key type (optimized for Point)
     * @tparam Hash Hash function type
     * 
     * Time Complexity:
     * - Insert: O(1) average, O(n) worst case
     * - Find: O(1) average, O(n) worst case
     * - Erase: O(1) average, O(n) worst case
     * 
     * Space Complexity: O(n)
     * 
     * Features:
     * - Built on optimized HashMap
     * - Robin Hood hashing for reduced variance
     * - Optimized for Point-based keys
     * - Move semantics support
     * - Exception safety guarantees
     */
    template<
        typename Key,
        typename Hash = ds::Hash<Key>
    >
    class HashSet {
    public:
        using key_type = Key;
        using value_type = Key;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using hasher = Hash;
        using reference = const value_type&;
        using const_reference = const value_type&;
        
    private:
        // Use HashMap with dummy value type
        struct DummyValue {};
        using InternalMap = HashMap<Key, DummyValue, Hash>;
        InternalMap map_;
        
    public:
        // Iterator types (forward the HashMap's key iterator)
        class iterator {
        private:
            typename InternalMap::iterator map_iter_;
            
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = HashSet::value_type;
            using difference_type = HashSet::difference_type;
            using pointer = const value_type*;
            using reference = const value_type&;
            
            explicit iterator(typename InternalMap::iterator iter) : map_iter_(iter) {}
            
            reference operator*() const { return map_iter_->first; }
            pointer operator->() const { return &map_iter_->first; }
            
            iterator& operator++() {
                ++map_iter_;
                return *this;
            }
            
            iterator operator++(int) {
                iterator temp = *this;
                ++(*this);
                return temp;
            }
            
            bool operator==(const iterator& other) const {
                return map_iter_ == other.map_iter_;
            }
            
            bool operator!=(const iterator& other) const {
                return !(*this == other);
            }
        };
        
        class const_iterator {
        private:
            typename InternalMap::const_iterator map_iter_;
            
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = HashSet::value_type;
            using difference_type = HashSet::difference_type;
            using pointer = const value_type*;
            using reference = const value_type&;
            
            explicit const_iterator(typename InternalMap::const_iterator iter) : map_iter_(iter) {}
            
            reference operator*() const { return map_iter_->first; }
            pointer operator->() const { return &map_iter_->first; }
            
            const_iterator& operator++() {
                ++map_iter_;
                return *this;
            }
            
            const_iterator operator++(int) {
                const_iterator temp = *this;
                ++(*this);
                return temp;
            }
            
            bool operator==(const const_iterator& other) const {
                return map_iter_ == other.map_iter_;
            }
            
            bool operator!=(const const_iterator& other) const {
                return !(*this == other);
            }
        };
        
        // ========================
        // CONSTRUCTORS & DESTRUCTOR
        // ========================
        
        /**
         * @brief Default constructor
         * @complexity O(1)
         */
        explicit HashSet(size_t initial_capacity = constants::DEFAULT_INITIAL_CAPACITY)
            : map_(initial_capacity) {}
        
        /**
         * @brief Copy constructor
         * @complexity O(n)
         */
        HashSet(const HashSet& other) : map_(other.map_) {}
        
        /**
         * @brief Move constructor
         * @complexity O(1)
         */
        HashSet(HashSet&& other) noexcept : map_(std::move(other.map_)) {}
        
        /**
         * @brief Initializer list constructor
         * @complexity O(n)
         */
        HashSet(std::initializer_list<value_type> init) : map_(init.size()) {
            for (const auto& item : init) {
                insert(item);
            }
        }
        
        /**
         * @brief Range constructor
         * @complexity O(n)
         */
        template<typename InputIterator>
        HashSet(InputIterator first, InputIterator last) {
            for (auto it = first; it != last; ++it) {
                insert(*it);
            }
        }
        
        // ========================
        // ASSIGNMENT OPERATORS
        // ========================
        
        HashSet& operator=(const HashSet& other) {
            if (this != &other) {
                map_ = other.map_;
            }
            return *this;
        }
        
        HashSet& operator=(HashSet&& other) noexcept {
            if (this != &other) {
                map_ = std::move(other.map_);
            }
            return *this;
        }
        
        HashSet& operator=(std::initializer_list<value_type> init) {
            clear();
            for (const auto& item : init) {
                insert(item);
            }
            return *this;
        }
        
        // ========================
        // MODIFIERS
        // ========================
        
        /**
         * @brief Insert element
         * @param key Key to insert
         * @return Pair of iterator and bool indicating success
         * @complexity O(1) average
         */
        std::pair<iterator, bool> insert(const Key& key) {
            auto [map_iter, inserted] = map_.insert(key, DummyValue{});
            return {iterator(map_iter), inserted};
        }
        
        std::pair<iterator, bool> insert(Key&& key) {
            auto [map_iter, inserted] = map_.insert(std::move(key), DummyValue{});
            return {iterator(map_iter), inserted};
        }
        
        /**
         * @brief Emplace element
         * @param args Arguments to construct element
         * @return Pair of iterator and bool indicating success
         * @complexity O(1) average
         */
        template<typename... Args>
        std::pair<iterator, bool> emplace(Args&&... args) {
            Key key(std::forward<Args>(args)...);
            return insert(std::move(key));
        }
        
        /**
         * @brief Insert range of elements
         * @param first Begin iterator
         * @param last End iterator
         * @complexity O(n) where n is distance(first, last)
         */
        template<typename InputIterator>
        void insert(InputIterator first, InputIterator last) {
            for (auto it = first; it != last; ++it) {
                insert(*it);
            }
        }
        
        /**
         * @brief Erase element by key
         * @param key Key to erase
         * @return Number of elements erased (0 or 1)
         * @complexity O(1) average
         */
        size_t erase(const Key& key) {
            return map_.erase(key);
        }
        
        /**
         * @brief Erase element by iterator
         * @param pos Iterator to element to erase
         * @return Iterator to next element
         * @complexity O(1) average
         */
        iterator erase(const_iterator pos) {
            // Note: This is a simplified implementation
            // A full implementation would need to properly handle iterator invalidation
            Key key = *pos;
            erase(key);
            return end(); // Simplified - should return iterator to next element
        }
        
        /**
         * @brief Clear all elements
         * @complexity O(n)
         */
        void clear() {
            map_.clear();
        }
        
        /**
         * @brief Swap contents with another HashSet
         * @complexity O(1)
         */
        void swap(HashSet& other) noexcept {
            map_.swap(other.map_);
        }
        
        // ========================
        // LOOKUP
        // ========================
        
        /**
         * @brief Find element
         * @param key Key to find
         * @return Iterator to element or end()
         * @complexity O(1) average
         */
        iterator find(const Key& key) {
            return iterator(map_.find(key));
        }
        
        const_iterator find(const Key& key) const {
            return const_iterator(map_.find(key));
        }
        
        /**
         * @brief Check if key exists
         * @param key Key to check
         * @return true if key exists
         * @complexity O(1) average
         */
        bool contains(const Key& key) const {
            return map_.contains(key);
        }
        
        /**
         * @brief Count elements with key (0 or 1 for HashSet)
         * @param key Key to count
         * @return Number of elements with key
         * @complexity O(1) average
         */
        size_t count(const Key& key) const {
            return map_.count(key);
        }
        
        // ========================
        // CAPACITY
        // ========================
        
        bool empty() const noexcept { return map_.empty(); }
        size_t size() const noexcept { return map_.size(); }
        size_t capacity() const noexcept { return map_.capacity(); }
        size_t max_size() const noexcept { return map_.max_size(); }
        
        double load_factor() const noexcept { return map_.load_factor(); }
        
        /**
         * @brief Reserve capacity
         * @param new_capacity Minimum capacity to reserve
         * @complexity O(n) if rehashing needed
         */
        void reserve(size_t new_capacity) {
            map_.reserve(new_capacity);
        }
        
        // ========================
        // ITERATORS
        // ========================
        
        iterator begin() { return iterator(map_.begin()); }
        const_iterator begin() const { return const_iterator(map_.begin()); }
        const_iterator cbegin() const { return begin(); }
        
        iterator end() { return iterator(map_.end()); }
        const_iterator end() const { return const_iterator(map_.end()); }
        const_iterator cend() const { return end(); }
        
        // ========================
        // SET OPERATIONS
        // ========================
        
        /**
         * @brief Check if this set contains all elements of other set
         * @param other Other set to check
         * @return true if this is superset of other
         * @complexity O(other.size())
         */
        bool contains_all(const HashSet& other) const {
            for (const auto& key : other) {
                if (!contains(key)) {
                    return false;
                }
            }
            return true;
        }
        
        /**
         * @brief Union with another set (modifies this set)
         * @param other Other set to union with
         * @complexity O(other.size())
         */
        void union_with(const HashSet& other) {
            for (const auto& key : other) {
                insert(key);
            }
        }
        
        /**
         * @brief Intersection with another set (modifies this set)
         * @param other Other set to intersect with
         * @complexity O(this.size())
         */
        void intersect_with(const HashSet& other) {
            HashSet result;
            for (const auto& key : *this) {
                if (other.contains(key)) {
                    result.insert(key);
                }
            }
            *this = std::move(result);
        }
        
        /**
         * @brief Difference with another set (modifies this set)
         * @param other Other set to subtract
         * @complexity O(other.size())
         */
        void difference_with(const HashSet& other) {
            for (const auto& key : other) {
                erase(key);
            }
        }
        
        // ========================
        // DEBUGGING & STATISTICS
        // ========================
        
        /**
         * @brief Get statistics about hash distribution
         */
        auto get_statistics() const {
            return map_.get_statistics();
        }
        
        /**
         * @brief Print debug information
         */
        void debug_print() const {
            auto stats = get_statistics();
            std::cout << "HashSet Statistics:\n"
                      << "  Size: " << size() << "\n"
                      << "  Capacity: " << capacity() << "\n"
                      << "  Load Factor: " << load_factor() << "\n"
                      << "  Average Distance: " << stats.average_distance << "\n"
                      << "  Max Distance: " << stats.max_distance << "\n"
                      << "  Collisions: " << stats.collisions << std::endl;
        }
    };
    
    // ========================
    // NON-MEMBER FUNCTIONS
    // ========================
    
    template<typename Key, typename Hash>
    bool operator==(const HashSet<Key, Hash>& lhs, const HashSet<Key, Hash>& rhs) {
        if (lhs.size() != rhs.size()) return false;
        
        for (const auto& key : lhs) {
            if (!rhs.contains(key)) {
                return false;
            }
        }
        return true;
    }
    
    template<typename Key, typename Hash>
    bool operator!=(const HashSet<Key, Hash>& lhs, const HashSet<Key, Hash>& rhs) {
        return !(lhs == rhs);
    }
    
    template<typename Key, typename Hash>
    void swap(HashSet<Key, Hash>& lhs, HashSet<Key, Hash>& rhs) noexcept {
        lhs.swap(rhs);
    }
    
    /**
     * @brief Set union (creates new set)
     * @complexity O(lhs.size() + rhs.size())
     */
    template<typename Key, typename Hash>
    HashSet<Key, Hash> set_union(const HashSet<Key, Hash>& lhs, const HashSet<Key, Hash>& rhs) {
        HashSet<Key, Hash> result = lhs;
        result.union_with(rhs);
        return result;
    }
    
    /**
     * @brief Set intersection (creates new set)
     * @complexity O(min(lhs.size(), rhs.size()))
     */
    template<typename Key, typename Hash>
    HashSet<Key, Hash> set_intersection(const HashSet<Key, Hash>& lhs, const HashSet<Key, Hash>& rhs) {
        const HashSet<Key, Hash>& smaller = lhs.size() < rhs.size() ? lhs : rhs;
        const HashSet<Key, Hash>& larger = lhs.size() < rhs.size() ? rhs : lhs;
        
        HashSet<Key, Hash> result;
        for (const auto& key : smaller) {
            if (larger.contains(key)) {
                result.insert(key);
            }
        }
        return result;
    }
    
    /**
     * @brief Set difference (creates new set)
     * @complexity O(lhs.size())
     */
    template<typename Key, typename Hash>
    HashSet<Key, Hash> set_difference(const HashSet<Key, Hash>& lhs, const HashSet<Key, Hash>& rhs) {
        HashSet<Key, Hash> result;
        for (const auto& key : lhs) {
            if (!rhs.contains(key)) {
                result.insert(key);
            }
        }
        return result;
    }
    
} // namespace ds
