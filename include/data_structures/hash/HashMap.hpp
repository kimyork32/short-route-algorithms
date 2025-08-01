#pragma once

#include "../core/Common.hpp"
#include "../core/Hash.hpp"
#include "../core/Traits.hpp"
#include <vector>
#include <iterator>

/**
 * @file HashMap.hpp
 * @brief High-performance hash map optimized for pathfinding algorithms
 * 
 * This implementation uses open addressing with Robin Hood hashing for:
 * - Excellent cache locality (critical for pathfinding performance)
 * - Minimal memory overhead
 * - Predictable performance characteristics
 * - Optimized for Point-based keys used in pathfinding
 */

namespace ds {
    
    /**
     * @brief High-performance hash map with Robin Hood hashing
     * @tparam Key Key type (optimized for Point)
     * @tparam Value Value type
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
     * - Robin Hood hashing for reduced variance
     * - Power-of-2 sizing for fast modulo
     * - Tombstone-based deletion
     * - Move semantics support
     * - Exception safety guarantees
     */
    template<
        typename Key,
        typename Value,
        typename Hash = ds::Hash<Key>
    >
    class HashMap {
    public:
        using key_type = Key;
        using mapped_type = Value;
        using value_type = std::pair<Key, Value>;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using hasher = Hash;
        using reference = value_type&;
        using const_reference = const value_type&;
        
    private:
        /**
         * @brief Internal storage entry
         */
        struct Entry {
            value_type data;
            size_t distance_from_desired;  // Robin Hood distance
            bool is_occupied;
            bool is_tombstone;
            
            Entry() : distance_from_desired(0), is_occupied(false), is_tombstone(false) {}
            
            template<typename K, typename V>
            Entry(K&& key, V&& value, size_t distance) 
                : data(std::forward<K>(key), std::forward<V>(value))
                , distance_from_desired(distance)
                , is_occupied(true)
                , is_tombstone(false) {}
        };
        
        std::vector<Entry> buckets_;
        size_t size_;
        size_t capacity_;
        size_t mask_;           // For fast modulo (capacity - 1)
        size_t tombstone_count_;
        hasher hash_function_;
        
        static constexpr double MAX_LOAD_FACTOR = 0.75;
        static constexpr size_t MIN_CAPACITY = 8;
        
        /**
         * @brief Calculate desired bucket for key
         * @complexity O(1)
         */
        size_t desired_bucket(const Key& key) const {
            return hash_function_(key) & mask_;
        }
        
        /**
         * @brief Rehash the container to new capacity
         * @complexity O(n)
         */
        void rehash_to_capacity(size_t new_capacity) {
            new_capacity = std::max(new_capacity, MIN_CAPACITY);
            new_capacity = utils::next_power_of_2(new_capacity);
            
            if (new_capacity == capacity_) return;
            
            std::vector<Entry> old_buckets = std::move(buckets_);
            size_t old_capacity = capacity_;
            
            buckets_.clear();
            buckets_.resize(new_capacity);
            capacity_ = new_capacity;
            mask_ = capacity_ - 1;
            size_ = 0;
            tombstone_count_ = 0;
            
            // Reinsert all elements
            for (size_t i = 0; i < old_capacity; ++i) {
                Entry& entry = old_buckets[i];
                if (entry.is_occupied && !entry.is_tombstone) {
                    insert_internal(std::move(entry.data.first), std::move(entry.data.second));
                }
            }
        }
        
        /**
         * @brief Check if rehashing is needed
         * @complexity O(1)
         */
        bool needs_rehash() const {
            return (size_ + tombstone_count_) >= static_cast<size_t>(capacity_ * MAX_LOAD_FACTOR);
        }
        
        /**
         * @brief Internal insertion with Robin Hood hashing
         * @complexity O(1) average
         */
        template<typename K, typename V>
        std::pair<size_t, bool> insert_internal(K&& key, V&& value) {
            if (needs_rehash()) {
                rehash_to_capacity(capacity_ * 2);
            }
            
            size_t bucket = desired_bucket(key);
            size_t distance = 0;
            
            Entry new_entry(std::forward<K>(key), std::forward<V>(value), distance);
            
            while (true) {
                Entry& current = buckets_[bucket];
                
                // Found empty slot
                if (!current.is_occupied) {
                    if (current.is_tombstone) {
                        tombstone_count_--;
                    }
                    new_entry.distance_from_desired = distance;
                    current = std::move(new_entry);
                    size_++;
                    return {bucket, true};
                }
                
                // Found existing key
                if (!current.is_tombstone && current.data.first == new_entry.data.first) {
                    current.data.second = std::forward<V>(value);
                    return {bucket, false};
                }
                
                // Robin Hood: evict if current has shorter distance
                if (!current.is_tombstone && distance > current.distance_from_desired) {
                    new_entry.distance_from_desired = distance;
                    std::swap(current, new_entry);
                    distance = new_entry.distance_from_desired;
                }
                
                bucket = (bucket + 1) & mask_;
                distance++;
            }
        }
        
        /**
         * @brief Find bucket for key
         * @complexity O(1) average
         */
        size_t find_bucket(const Key& key) const {
            size_t bucket = desired_bucket(key);
            size_t distance = 0;
            
            while (true) {
                const Entry& current = buckets_[bucket];
                
                if (!current.is_occupied) {
                    return capacity_; // Not found
                }
                
                if (!current.is_tombstone && 
                    current.data.first == key) {
                    return bucket;
                }
                
                // If current distance is less than search distance, key not present
                if (!current.is_tombstone && distance > current.distance_from_desired) {
                    return capacity_; // Not found
                }
                
                bucket = (bucket + 1) & mask_;
                distance++;
            }
        }
        
    public:
        /**
         * @brief Iterator class
         */
        class iterator {
        private:
            Entry* buckets_;
            size_t index_;
            size_t capacity_;
            
            void advance_to_next() {
                while (index_ < capacity_ && 
                       (!buckets_[index_].is_occupied || buckets_[index_].is_tombstone)) {
                    index_++;
                }
            }
            
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = HashMap::value_type;
            using difference_type = HashMap::difference_type;
            using pointer = value_type*;
            using reference = value_type&;
            
            iterator(Entry* buckets, size_t index, size_t capacity) 
                : buckets_(buckets), index_(index), capacity_(capacity) {
                advance_to_next();
            }
            
            reference operator*() { return buckets_[index_].data; }
            pointer operator->() { return &buckets_[index_].data; }
            
            iterator& operator++() {
                index_++;
                advance_to_next();
                return *this;
            }
            
            iterator operator++(int) {
                iterator temp = *this;
                ++(*this);
                return temp;
            }
            
            bool operator==(const iterator& other) const {
                return index_ == other.index_;
            }
            
            bool operator!=(const iterator& other) const {
                return !(*this == other);
            }
        };
        
        /**
         * @brief Const iterator class
         */
        class const_iterator {
        private:
            const Entry* buckets_;
            size_t index_;
            size_t capacity_;
            
            void advance_to_next() {
                while (index_ < capacity_ && 
                       (!buckets_[index_].is_occupied || buckets_[index_].is_tombstone)) {
                    index_++;
                }
            }
            
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = HashMap::value_type;
            using difference_type = HashMap::difference_type;
            using pointer = const value_type*;
            using reference = const value_type&;
            
            const_iterator(const Entry* buckets, size_t index, size_t capacity) 
                : buckets_(buckets), index_(index), capacity_(capacity) {
                advance_to_next();
            }
            
            reference operator*() const { return buckets_[index_].data; }
            pointer operator->() const { return &buckets_[index_].data; }
            
            const_iterator& operator++() {
                index_++;
                advance_to_next();
                return *this;
            }
            
            const_iterator operator++(int) {
                const_iterator temp = *this;
                ++(*this);
                return temp;
            }
            
            bool operator==(const const_iterator& other) const {
                return index_ == other.index_;
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
        explicit HashMap(size_t initial_capacity = constants::DEFAULT_INITIAL_CAPACITY)
            : size_(0)
            , capacity_(utils::next_power_of_2(std::max(initial_capacity, MIN_CAPACITY)))
            , mask_(capacity_ - 1)
            , tombstone_count_(0) {
            buckets_.resize(capacity_);
        }
        
        /**
         * @brief Copy constructor
         * @complexity O(n)
         */
        HashMap(const HashMap& other)
            : buckets_(other.buckets_)
            , size_(other.size_)
            , capacity_(other.capacity_)
            , mask_(other.mask_)
            , tombstone_count_(other.tombstone_count_)
            , hash_function_(other.hash_function_) {}
        
        /**
         * @brief Move constructor
         * @complexity O(1)
         */
        HashMap(HashMap&& other) noexcept
            : buckets_(std::move(other.buckets_))
            , size_(other.size_)
            , capacity_(other.capacity_)
            , mask_(other.mask_)
            , tombstone_count_(other.tombstone_count_)
            , hash_function_(std::move(other.hash_function_)) {
            other.size_ = 0;
            other.capacity_ = 0;
            other.mask_ = 0;
            other.tombstone_count_ = 0;
        }
        
        /**
         * @brief Initializer list constructor
         * @complexity O(n)
         */
        HashMap(std::initializer_list<value_type> init)
            : HashMap(init.size()) {
            for (const auto& item : init) {
                insert(item.first, item.second);
            }
        }
        
        // ========================
        // ASSIGNMENT OPERATORS
        // ========================
        
        HashMap& operator=(const HashMap& other) {
            if (this != &other) {
                HashMap temp(other);
                swap(temp);
            }
            return *this;
        }
        
        HashMap& operator=(HashMap&& other) noexcept {
            if (this != &other) {
                swap(other);
            }
            return *this;
        }
        
        HashMap& operator=(std::initializer_list<value_type> init) {
            clear();
            for (const auto& item : init) {
                insert(item.first, item.second);
            }
            return *this;
        }
        
        // ========================
        // ELEMENT ACCESS
        // ========================
        
        /**
         * @brief Access element with bounds checking
         * @param key Key to find
         * @return Reference to mapped value
         * @throws OutOfBoundsException if key not found
         * @complexity O(1) average
         */
        mapped_type& at(const Key& key) {
            size_t bucket = find_bucket(key);
            if (bucket == capacity_) {
                throw OutOfBoundsException("Key not found in HashMap");
            }
            return buckets_[bucket].data.second;
        }
        
        const mapped_type& at(const Key& key) const {
            size_t bucket = find_bucket(key);
            if (bucket == capacity_) {
                throw OutOfBoundsException("Key not found in HashMap");
            }
            return buckets_[bucket].data.second;
        }
        
        /**
         * @brief Access or insert element
         * @param key Key to access
         * @return Reference to mapped value
         * @complexity O(1) average
         */
        mapped_type& operator[](const Key& key) {
            auto [bucket, inserted] = insert_internal(key, mapped_type{});
            return buckets_[bucket].data.second;
        }
        
        // ========================
        // MODIFIERS
        // ========================
        
        /**
         * @brief Insert element
         * @param key Key to insert
         * @param value Value to insert
         * @return Pair of iterator and bool indicating success
         * @complexity O(1) average
         */
        std::pair<iterator, bool> insert(const Key& key, const Value& value) {
            auto [bucket, inserted] = insert_internal(key, value);
            return {iterator(buckets_.data(), bucket, capacity_), inserted};
        }
        
        std::pair<iterator, bool> insert(Key&& key, Value&& value) {
            auto [bucket, inserted] = insert_internal(std::move(key), std::move(value));
            return {iterator(buckets_.data(), bucket, capacity_), inserted};
        }
        
        std::pair<iterator, bool> insert(const value_type& value) {
            return insert(value.first, value.second);
        }
        
        std::pair<iterator, bool> insert(value_type&& value) {
            return insert(std::move(value.first), std::move(value.second));
        }
        
        /**
         * @brief Emplace element
         * @param args Arguments to construct element
         * @return Pair of iterator and bool indicating success
         * @complexity O(1) average
         */
        template<typename... Args>
        std::pair<iterator, bool> emplace(Args&&... args) {
            value_type temp(std::forward<Args>(args)...);
            return insert(std::move(temp));
        }
        
        /**
         * @brief Erase element by key
         * @param key Key to erase
         * @return Number of elements erased (0 or 1)
         * @complexity O(1) average
         */
        size_t erase(const Key& key) {
            size_t bucket = find_bucket(key);
            if (bucket == capacity_) {
                return 0;
            }
            
            buckets_[bucket].is_tombstone = true;
            tombstone_count_++;
            size_--;
            return 1;
        }
        
        /**
         * @brief Clear all elements
         * @complexity O(n)
         */
        void clear() {
            buckets_.clear();
            buckets_.resize(capacity_);
            size_ = 0;
            tombstone_count_ = 0;
        }
        
        /**
         * @brief Swap contents with another HashMap
         * @complexity O(1)
         */
        void swap(HashMap& other) noexcept {
            std::swap(buckets_, other.buckets_);
            std::swap(size_, other.size_);
            std::swap(capacity_, other.capacity_);
            std::swap(mask_, other.mask_);
            std::swap(tombstone_count_, other.tombstone_count_);
            std::swap(hash_function_, other.hash_function_);
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
            size_t bucket = find_bucket(key);
            if (bucket == capacity_) {
                return end();
            }
            return iterator(buckets_.data(), bucket, capacity_);
        }
        
        const_iterator find(const Key& key) const {
            size_t bucket = find_bucket(key);
            if (bucket == capacity_) {
                return end();
            }
            return const_iterator(buckets_.data(), bucket, capacity_);
        }
        
        /**
         * @brief Check if key exists
         * @param key Key to check
         * @return true if key exists
         * @complexity O(1) average
         */
        bool contains(const Key& key) const {
            return find_bucket(key) != capacity_;
        }
        
        /**
         * @brief Count elements with key (0 or 1 for HashMap)
         * @param key Key to count
         * @return Number of elements with key
         * @complexity O(1) average
         */
        size_t count(const Key& key) const {
            return contains(key) ? 1 : 0;
        }
        
        // ========================
        // CAPACITY
        // ========================
        
        bool empty() const noexcept { return size_ == 0; }
        size_t size() const noexcept { return size_; }
        size_t capacity() const noexcept { return capacity_; }
        size_t max_size() const noexcept { return SIZE_MAX / sizeof(Entry); }
        
        double load_factor() const noexcept {
            return capacity_ > 0 ? static_cast<double>(size_) / capacity_ : 0.0;
        }
        
        /**
         * @brief Reserve capacity
         * @param new_capacity Minimum capacity to reserve
         * @complexity O(n) if rehashing needed
         */
        void reserve(size_t new_capacity) {
            size_t required_capacity = static_cast<size_t>(new_capacity / MAX_LOAD_FACTOR) + 1;
            if (required_capacity > capacity_) {
                rehash_to_capacity(required_capacity);
            }
        }
        
        // ========================
        // ITERATORS
        // ========================
        
        iterator begin() { return iterator(buckets_.data(), 0, capacity_); }
        const_iterator begin() const { return const_iterator(buckets_.data(), 0, capacity_); }
        const_iterator cbegin() const { return begin(); }
        
        iterator end() { return iterator(buckets_.data(), capacity_, capacity_); }
        const_iterator end() const { return const_iterator(buckets_.data(), capacity_, capacity_); }
        const_iterator cend() const { return end(); }
        
        // ========================
        // DEBUGGING & STATISTICS
        // ========================
        
        /**
         * @brief Get statistics about hash distribution
         */
        struct Statistics {
            size_t total_entries;
            size_t occupied_entries;
            size_t tombstone_entries;
            double load_factor;
            double average_distance;
            size_t max_distance;
            size_t collisions;
        };
        
        Statistics get_statistics() const {
            Statistics stats{};
            stats.total_entries = capacity_;
            stats.occupied_entries = size_;
            stats.tombstone_entries = tombstone_count_;
            stats.load_factor = load_factor();
            
            size_t total_distance = 0;
            size_t max_distance = 0;
            size_t collisions = 0;
            
            for (size_t i = 0; i < capacity_; ++i) {
                const Entry& entry = buckets_[i];
                if (entry.is_occupied && !entry.is_tombstone) {
                    total_distance += entry.distance_from_desired;
                    max_distance = std::max(max_distance, entry.distance_from_desired);
                    if (entry.distance_from_desired > 0) {
                        collisions++;
                    }
                }
            }
            
            stats.average_distance = size_ > 0 ? static_cast<double>(total_distance) / size_ : 0.0;
            stats.max_distance = max_distance;
            stats.collisions = collisions;
            
            return stats;
        }
    };
    
    // ========================
    // NON-MEMBER FUNCTIONS
    // ========================
    
    template<typename Key, typename Value, typename Hash>
    bool operator==(const HashMap<Key, Value, Hash>& lhs, const HashMap<Key, Value, Hash>& rhs) {
        if (lhs.size() != rhs.size()) return false;
        
        for (const auto& [key, value] : lhs) {
            auto it = rhs.find(key);
            if (it == rhs.end() || it->second != value) {
                return false;
            }
        }
        return true;
    }
    
    template<typename Key, typename Value, typename Hash>
    bool operator!=(const HashMap<Key, Value, Hash>& lhs, const HashMap<Key, Value, Hash>& rhs) {
        return !(lhs == rhs);
    }
    
    template<typename Key, typename Value, typename Hash>
    void swap(HashMap<Key, Value, Hash>& lhs, HashMap<Key, Value, Hash>& rhs) noexcept {
        lhs.swap(rhs);
    }
    
} // namespace ds
