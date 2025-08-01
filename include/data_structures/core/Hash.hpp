#pragma once

#include "Common.hpp"
#include "../../Point.hpp"
#include <functional>

/**
 * @file Hash.hpp
 * @brief Hash function implementations optimized for pathfinding
 */

namespace ds {
    
    /**
     * @brief Generic hash function wrapper
     * @tparam T Type to hash
     */
    template<typename T>
    struct Hash {
        constexpr size_t operator()(const T& value) const noexcept {
            return std::hash<T>{}(value);
        }
    };
    
    /**
     * @brief Specialized hash for Point (critical for pathfinding)
     * Uses optimized hash combination for 2D coordinates
     * @complexity O(1)
     */
    template<>
    struct Hash<Point> {
        constexpr size_t operator()(const Point& point) const noexcept {
            // Use prime numbers for better distribution
            // This is optimized for typical pathfinding coordinate ranges
            constexpr size_t prime1 = 73856093;
            constexpr size_t prime2 = 19349663;
            
            size_t hx = static_cast<size_t>(point.x) * prime1;
            size_t hy = static_cast<size_t>(point.y) * prime2;
            
            // XOR with bit shifting for better avalanche effect
            return hx ^ (hy + 0x9e3779b9 + (hx << 6) + (hx >> 2));
        }
    };
    
    /**
     * @brief Hash for std::pair<Point, Point> (useful for edges)
     * @complexity O(1)
     */
    template<>
    struct Hash<std::pair<Point, Point>> {
        constexpr size_t operator()(const std::pair<Point, Point>& edge) const noexcept {
            Hash<Point> point_hasher;
            size_t h1 = point_hasher(edge.first);
            size_t h2 = point_hasher(edge.second);
            
            // Combine hashes with golden ratio magic number
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };
    
    /**
     * @brief Hash for integer types with better distribution
     * @complexity O(1)
     */
    template<>
    struct Hash<int> {
        constexpr size_t operator()(int value) const noexcept {
            // Wang's integer hash function - excellent distribution
            size_t x = static_cast<size_t>(value);
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = ((x >> 16) ^ x) * 0x45d9f3b;
            x = (x >> 16) ^ x;
            return x;
        }
    };
    
    /**
     * @brief Hash for string types
     * @complexity O(n) where n is string length
     */
    template<>
    struct Hash<std::string> {
        size_t operator()(const std::string& str) const noexcept {
            // FNV-1a hash algorithm - fast and good distribution
            constexpr size_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
            constexpr size_t FNV_PRIME = 1099511628211ULL;
            
            size_t hash = FNV_OFFSET_BASIS;
            
            for (char c : str) {
                hash ^= static_cast<size_t>(c);
                hash *= FNV_PRIME;
            }
            
            return hash;
        }
    };
    
    /**
     * @brief Hash combination utility
     * Combines multiple hash values into one
     * @param seed Starting seed value
     * @param value Value to combine
     * @complexity O(1)
     */
    template<typename T>
    constexpr void hash_combine(size_t& seed, const T& value) noexcept {
        Hash<T> hasher;
        seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    
    /**
     * @brief Hash multiple values together
     * @param args Values to hash together
     * @return Combined hash value
     * @complexity O(k) where k is number of arguments
     */
    template<typename... Args>
    constexpr size_t hash_values(const Args&... args) noexcept {
        size_t seed = 0;
        (hash_combine(seed, args), ...); // C++17 fold expression
        return seed;
    }
    
    /**
     * @brief Check if hash function is available for type T
     */
    template<typename T, typename = void>
    struct is_hashable : std::false_type {};
    
    template<typename T>
    struct is_hashable<T, std::void_t<decltype(Hash<T>{}(std::declval<T>()))>> : std::true_type {};
    
    template<typename T>
    constexpr bool is_hashable_v = is_hashable<T>::value;
    
} // namespace ds

/**
 * @brief Specialization for std namespace (for STL compatibility)
 */
namespace std {
    template<>
    struct hash<Point> {
        size_t operator()(const Point& point) const noexcept {
            return ds::Hash<Point>{}(point);
        }
    };
}
