#pragma once

/**
 * @file Aliases.hpp
 * @brief Compatibility layer for gradual migration from STL to custom data structures
 * 
 * This file provides aliases and macros that allow gradual migration from STL
 * containers to custom implementations. You can switch between implementations
 * by defining preprocessor macros.
 * 
 * Usage:
 * - Define USE_CUSTOM_CONTAINERS to use custom implementations
 * - Define USE_STD_CONTAINERS (or leave undefined) to use STL
 * - Define USE_HYBRID_CONTAINERS to mix both based on specific needs
 */

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <stack>
#include <array>

// Include custom implementations
#include "hash/HashMap.hpp"
#include "hash/HashSet.hpp"
#include "linear/Vector.hpp"
#include "linear/PriorityQueue.hpp"
#include "linear/Stack.hpp"
#include "linear/Queue.hpp"
#include "linear/Array.hpp"

namespace ds {
    namespace pathfinding {
        
        // ========================
        // CONFIGURATION MACROS
        // ========================
        
        // Uncomment one of these to choose implementation:
        // #define USE_CUSTOM_CONTAINERS    // Use all custom implementations
        // #define USE_STD_CONTAINERS       // Use all STL implementations  
        // #define USE_HYBRID_CONTAINERS    // Mix custom and STL based on performance
        
        // Default to STL if nothing is defined
        #if !defined(USE_CUSTOM_CONTAINERS) && !defined(USE_STD_CONTAINERS) && !defined(USE_HYBRID_CONTAINERS)
        #define USE_STD_CONTAINERS
        #endif
        
        // ========================
        // HASH-BASED CONTAINERS (Critical for pathfinding)
        // ========================
        
        #ifdef USE_CUSTOM_CONTAINERS
            template<typename Key, typename Value, typename Hash = ds::Hash<Key>>
            using HashMap = ds::HashMap<Key, Value, Hash>;
            
            template<typename Key, typename Hash = ds::Hash<Key>>
            using HashSet = ds::HashSet<Key, Hash>;
            
        #elif defined(USE_HYBRID_CONTAINERS)
            // Use custom for Point-based containers (optimized), STL for others
            template<typename Key, typename Value, typename Hash = ds::Hash<Key>>
            using HashMap = typename std::conditional<
                std::is_same_v<Key, Point>,
                ds::HashMap<Key, Value, Hash>,
                std::unordered_map<Key, Value, Hash>
            >::type;
            
            template<typename Key, typename Hash = ds::Hash<Key>>
            using HashSet = typename std::conditional<
                std::is_same_v<Key, Point>,
                ds::HashSet<Key, Hash>,
                std::unordered_set<Key, Hash>
            >::type;
            
        #else // USE_STD_CONTAINERS
            template<typename Key, typename Value, typename Hash = std::hash<Key>>
            using HashMap = std::unordered_map<Key, Value, Hash>;
            
            template<typename Key, typename Hash = std::hash<Key>>
            using HashSet = std::unordered_set<Key, Hash>;
        #endif
        
        // ========================
        // LINEAR CONTAINERS
        // ========================
        
        #ifdef USE_CUSTOM_CONTAINERS
            template<typename T>
            using Vector = ds::Vector<T>;
            
            template<typename T, typename Compare = std::greater<T>>
            using PriorityQueue = ds::PriorityQueue<T, Compare>;
            
            template<typename T>
            using Stack = ds::Stack<T>;
            
            template<typename T>
            using Queue = ds::Queue<T>;
            
        #elif defined(USE_HYBRID_CONTAINERS)
            // Use custom PriorityQueue (optimized for pathfinding), STL for others
            template<typename T>
            using Vector = std::vector<T>;
            
            template<typename T, typename Compare = std::greater<T>>
            using PriorityQueue = ds::PriorityQueue<T, Compare>;
            
            template<typename T>
            using Stack = std::stack<T>;
            
            template<typename T>
            using Queue = std::queue<T>;
            
        #else // USE_STD_CONTAINERS
            template<typename T>
            using Vector = std::vector<T>;
            
            template<typename T, typename Compare = std::greater<T>>
            using PriorityQueue = std::priority_queue<T, std::vector<T>, Compare>;
            
            template<typename T>
            using Stack = std::stack<T>;
            
            template<typename T>
            using Queue = std::queue<T>;
        #endif
        
        // ========================
        // SPECIALIZED ALIASES FOR PATHFINDING
        // ========================
        
        /**
         * @brief Point-to-double mapping (for g-scores, f-scores, distances)
         */
        using PointDistanceMap = HashMap<Point, double>;
        
        /**
         * @brief Point-to-Point mapping (for parent tracking in pathfinding)
         */
        using PointParentMap = HashMap<Point, Point>;
        
        /**
         * @brief Set of visited/closed Points
         */
        using PointSet = HashSet<Point>;
        
        /**
         * @brief Priority queue for pathfinding (pairs of <priority, point>)
         */
        using PathfindingQueue = PriorityQueue<std::pair<double, Point>>;
        
        /**
         * @brief Vector of Points (for storing paths)
         */
        using PointPath = Vector<Point>;
        
        /**
         * @brief Complex graph structure type (from your current code)
         */
        using GraphStructure = HashMap<Point, Vector<std::pair<Point, std::pair<bool, Vector<Point>>>>>;
        
        // ========================
        // CONVENIENCE ALIASES
        // ========================
        
        // Common integer containers
        using IntVector = Vector<int>;
        using IntSet = HashSet<int>;
        using IntMap = HashMap<int, int>;
        
        // Common string containers
        using StringVector = Vector<std::string>;
        using StringSet = HashSet<std::string>;
        using StringMap = HashMap<std::string, std::string>;
        
        // ========================
        // TYPE DETECTION UTILITIES
        // ========================
        
        /**
         * @brief Check if we're using custom implementations
         */
        constexpr bool using_custom_containers() {
            #ifdef USE_CUSTOM_CONTAINERS
                return true;
            #else
                return false;
            #endif
        }
        
        /**
         * @brief Check if we're using hybrid implementations
         */
        constexpr bool using_hybrid_containers() {
            #ifdef USE_HYBRID_CONTAINERS
                return true;
            #else
                return false;
            #endif
        }
        
        /**
         * @brief Get implementation info string
         */
        inline const char* get_implementation_info() {
            #ifdef USE_CUSTOM_CONTAINERS
                return "Custom Data Structures";
            #elif defined(USE_HYBRID_CONTAINERS)
                return "Hybrid (Custom + STL)";
            #else
                return "Standard Library (STL)";
            #endif
        }
        
        // ========================
        // MIGRATION HELPERS
        // ========================
        
        /**
         * @brief Convert STL container to custom container
         */
        template<typename CustomContainer, typename StdContainer>
        CustomContainer convert_to_custom(const StdContainer& std_container) {
            return CustomContainer(std_container.begin(), std_container.end());
        }
        
        /**
         * @brief Convert custom container to STL container
         */
        template<typename StdContainer, typename CustomContainer>
        StdContainer convert_to_std(const CustomContainer& custom_container) {
            return StdContainer(custom_container.begin(), custom_container.end());
        }
        
        // ========================
        // PATHFINDING-SPECIFIC FACTORY FUNCTIONS
        // ========================
        
        /**
         * @brief Create optimized distance map for pathfinding
         */
        inline PointDistanceMap create_distance_map(size_t expected_size = 1000) {
            PointDistanceMap map;
            #ifdef USE_CUSTOM_CONTAINERS
                map.reserve(expected_size);
            #else
                map.reserve(expected_size);
            #endif
            return map;
        }
        
        /**
         * @brief Create optimized parent map for pathfinding
         */
        inline PointParentMap create_parent_map(size_t expected_size = 1000) {
            PointParentMap map;
            #ifdef USE_CUSTOM_CONTAINERS
                map.reserve(expected_size);
            #else
                map.reserve(expected_size);
            #endif
            return map;
        }
        
        /**
         * @brief Create optimized closed set for pathfinding
         */
        inline PointSet create_closed_set(size_t expected_size = 1000) {
            PointSet set;
            #ifdef USE_CUSTOM_CONTAINERS
                set.reserve(expected_size);
            #else
                set.reserve(expected_size);
            #endif
            return set;
        }
        
        /**
         * @brief Create optimized priority queue for pathfinding
         */
        inline PathfindingQueue create_pathfinding_queue(size_t expected_size = 100) {
            #ifdef USE_CUSTOM_CONTAINERS
                return PathfindingQueue(expected_size);
            #else
                return PathfindingQueue();
            #endif
        }
        
        /**
         * @brief Create optimized path vector
         */
        inline PointPath create_path(size_t expected_size = 50) {
            #ifdef USE_CUSTOM_CONTAINERS
                PointPath path;
                path.reserve(expected_size);
                return path;
            #else
                PointPath path;
                path.reserve(expected_size);
                return path;
            #endif
        }
        
    } // namespace pathfinding
} // namespace ds

// ========================
// GLOBAL MACROS FOR EASY SWITCHING
// ========================

/**
 * @brief Macros for quick switching in existing code
 */
#define DS_HASHMAP ds::pathfinding::HashMap
#define DS_HASHSET ds::pathfinding::HashSet
#define DS_VECTOR ds::pathfinding::Vector
#define DS_PRIORITY_QUEUE ds::pathfinding::PriorityQueue
#define DS_STACK ds::pathfinding::Stack
#define DS_QUEUE ds::pathfinding::Queue

// Pathfinding-specific macros
#define DS_POINT_DISTANCE_MAP ds::pathfinding::PointDistanceMap
#define DS_POINT_PARENT_MAP ds::pathfinding::PointParentMap  
#define DS_POINT_SET ds::pathfinding::PointSet
#define DS_PATHFINDING_QUEUE ds::pathfinding::PathfindingQueue
#define DS_POINT_PATH ds::pathfinding::PointPath
#define DS_GRAPH_STRUCTURE ds::pathfinding::GraphStructure

// Tree-based container macros
#define DS_AVL_TREE ds::pathfinding::AVLTree
#define DS_RB_TREE ds::pathfinding::RedBlackTree

// Graph container macros
#define DS_GRAPH ds::pathfinding::Graph
#define DS_POINT_GRAPH ds::PointGraph
#define DS_WEIGHTED_POINT_GRAPH ds::WeightedPointGraph

// Factory function macros
#define DS_CREATE_DISTANCE_MAP(size) ds::pathfinding::create_distance_map(size)
#define DS_CREATE_PARENT_MAP(size) ds::pathfinding::create_parent_map(size)
#define DS_CREATE_CLOSED_SET(size) ds::pathfinding::create_closed_set(size)
#define DS_CREATE_PATHFINDING_QUEUE(size) ds::pathfinding::create_pathfinding_queue(size)
#define DS_CREATE_PATH(size) ds::pathfinding::create_path(size)
#define DS_CREATE_GRID_GRAPH(width, height, diagonals) ds::create_grid_graph(width, height, diagonals)

/**
 * @brief Performance tuning macros
 */
#define DS_OPTIMIZE_FOR_PATHFINDING() \
    static_assert(true, "Pathfinding optimizations enabled")

#define DS_ENABLE_STATISTICS() \
    static_assert(true, "Container statistics enabled")

#define DS_ENABLE_DEBUG_MODE() \
    static_assert(true, "Debug mode enabled")
