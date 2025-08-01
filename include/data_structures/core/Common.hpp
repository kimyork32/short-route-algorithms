#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>
#include <memory>
#include <stdexcept>
#include <initializer_list>

/**
 * @file Common.hpp
 * @brief Common utilities and definitions for the data structures library
 */

namespace ds {
    
    /**
     * @brief Exception class for data structure operations
     */
    class DataStructureException : public std::runtime_error {
    public:
        explicit DataStructureException(const std::string& message) 
            : std::runtime_error("DataStructure Error: " + message) {}
    };
    
    /**
     * @brief Out of bounds exception
     */
    class OutOfBoundsException : public DataStructureException {
    public:
        explicit OutOfBoundsException(const std::string& message = "Index out of bounds") 
            : DataStructureException(message) {}
    };
    
    /**
     * @brief Empty container exception
     */
    class EmptyContainerException : public DataStructureException {
    public:
        explicit EmptyContainerException(const std::string& message = "Container is empty") 
            : DataStructureException(message) {}
    };
    
    /**
     * @brief Memory allocation exception
     */
    class AllocationException : public DataStructureException {
    public:
        explicit AllocationException(const std::string& message = "Memory allocation failed") 
            : DataStructureException(message) {}
    };
    
    /**
     * @brief Common constants
     */
    namespace constants {
        constexpr size_t DEFAULT_INITIAL_CAPACITY = 16;
        constexpr double DEFAULT_LOAD_FACTOR = 0.75;
        constexpr double GROWTH_FACTOR = 2.0;
        constexpr size_t MIN_CAPACITY = 8;
    }
    
    /**
     * @brief Utility functions
     */
    namespace utils {
        
        /**
         * @brief Calculate next power of 2
         * @param n Input number
         * @return Next power of 2 >= n
         * @complexity O(1)
         */
        constexpr size_t next_power_of_2(size_t n) {
            if (n <= 1) return 1;
            
            n--;
            n |= n >> 1;
            n |= n >> 2;
            n |= n >> 4;
            n |= n >> 8;
            n |= n >> 16;
            if constexpr (sizeof(size_t) == 8) {
                n |= n >> 32;
            }
            n++;
            
            return n;
        }
        
        /**
         * @brief Check if number is power of 2
         * @param n Input number
         * @return true if n is power of 2
         * @complexity O(1)
         */
        constexpr bool is_power_of_2(size_t n) {
            return n > 0 && (n & (n - 1)) == 0;
        }
        
        /**
         * @brief Safe multiplication with overflow check
         * @param a First operand
         * @param b Second operand
         * @return Product if no overflow, throws on overflow
         * @complexity O(1)
         */
        constexpr size_t safe_multiply(size_t a, size_t b) {
            if (a != 0 && b > SIZE_MAX / a) {
                throw AllocationException("Multiplication overflow");
            }
            return a * b;
        }
        
        /**
         * @brief Calculate optimal capacity for growth
         * @param current_capacity Current capacity
         * @param required_size Required minimum size
         * @return New optimal capacity
         * @complexity O(1)
         */
        constexpr size_t calculate_growth(size_t current_capacity, size_t required_size) {
            size_t new_capacity = current_capacity * constants::GROWTH_FACTOR;
            return (new_capacity >= required_size) ? new_capacity : required_size;
        }
        
    } // namespace utils
    
} // namespace ds
