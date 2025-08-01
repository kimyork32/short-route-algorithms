#pragma once

/**
 * @file DataStructures.hpp
 * @brief Custom Data Structures Library for Pathfinding Algorithms
 * @author Your Name
 * @date 2025
 * 
 * This library provides custom implementations of essential data structures
 * optimized for pathfinding algorithms processing thousands of Point nodes.
 * 
 * Features:
 * - C++17 compliant with modern features (auto, constexpr, move semantics)
 * - RAII memory management
 * - Template-based generic design
 * - Optimized hash functions for Point structures
 * - Big-O complexity documentation
 * - Gradual migration support via aliases
 */

// Core utilities and traits
#include "core/Common.hpp"
#include "core/Hash.hpp"
#include "core/Traits.hpp"

// Hash-based structures (critical for pathfinding)
#include "hash/HashMap.hpp"
#include "hash/HashSet.hpp"

// Linear structures
#include "linear/Vector.hpp"
#include "linear/Array.hpp"
#include "linear/Stack.hpp"
#include "linear/Queue.hpp"
#include "linear/PriorityQueue.hpp"

// Tree structures
#include "trees/AVLTree.hpp"
#include "trees/RedBlackTree.hpp"

// Graph structures
#include "graph/Graph.hpp"

// Testing and benchmarking
#include "tests/TestRunner.hpp"
#include "benchmarks/Benchmark.hpp"

// Compatibility layer for gradual migration
#include "core/Aliases.hpp"

/**
 * @namespace ds
 * @brief Custom Data Structures namespace
 */
namespace ds {
    
    /**
     * @brief Version information
     */
    constexpr const char* VERSION = "1.0.0";
    constexpr int VERSION_MAJOR = 1;
    constexpr int VERSION_MINOR = 0;
    constexpr int VERSION_PATCH = 0;
    
    /**
     * @brief Library initialization (if needed)
     * Currently just returns version info
     */
    inline const char* initialize() {
        return VERSION;
    }
    
} // namespace ds
