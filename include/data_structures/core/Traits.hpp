#pragma once

#include "Common.hpp"
#include <type_traits>

/**
 * @file Traits.hpp
 * @brief Type traits and SFINAE utilities for the data structures library
 */

namespace ds {
    
    /**
     * @brief Check if type T has equality operator
     */
    template<typename T, typename = void>
    struct has_equality : std::false_type {};
    
    template<typename T>
    struct has_equality<T, std::void_t<decltype(std::declval<T>() == std::declval<T>())>> : std::true_type {};
    
    template<typename T>
    constexpr bool has_equality_v = has_equality<T>::value;
    
    /**
     * @brief Check if type T has less-than operator
     */
    template<typename T, typename = void>
    struct has_less_than : std::false_type {};
    
    template<typename T>
    struct has_less_than<T, std::void_t<decltype(std::declval<T>() < std::declval<T>())>> : std::true_type {};
    
    template<typename T>
    constexpr bool has_less_than_v = has_less_than<T>::value;
    
    /**
     * @brief Check if type T is move constructible and move assignable
     */
    template<typename T>
    struct is_movable : std::conjunction<std::is_move_constructible<T>, std::is_move_assignable<T>> {};
    
    template<typename T>
    constexpr bool is_movable_v = is_movable<T>::value;
    
    /**
     * @brief Check if type T is copyable
     */
    template<typename T>
    struct is_copyable : std::conjunction<std::is_copy_constructible<T>, std::is_copy_assignable<T>> {};
    
    template<typename T>
    constexpr bool is_copyable_v = is_copyable<T>::value;
    
    /**
     * @brief Perfect forwarding utility
     */
    template<typename T, typename U>
    constexpr T&& forward_like(U&& x) noexcept {
        if constexpr (std::is_lvalue_reference_v<T>) {
            return static_cast<std::remove_reference_t<T>&>(x);
        } else {
            return static_cast<std::remove_reference_t<T>&&>(x);
        }
    }
    
    /**
     * @brief SFINAE helper for enabling functions based on type traits
     */
    template<bool Condition, typename T = void>
    using enable_if_t = std::enable_if_t<Condition, T>;
    
    /**
     * @brief Container iterator traits
     */
    template<typename Iterator>
    struct iterator_traits {
        using value_type = typename std::iterator_traits<Iterator>::value_type;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        using pointer = typename std::iterator_traits<Iterator>::pointer;
        using reference = typename std::iterator_traits<Iterator>::reference;
        using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
    };
    
    /**
     * @brief Check if Iterator is a forward iterator
     */
    template<typename Iterator>
    struct is_forward_iterator : std::is_base_of<
        std::forward_iterator_tag,
        typename iterator_traits<Iterator>::iterator_category
    > {};
    
    template<typename Iterator>
    constexpr bool is_forward_iterator_v = is_forward_iterator<Iterator>::value;
    
    /**
     * @brief Check if Iterator is a random access iterator
     */
    template<typename Iterator>
    struct is_random_access_iterator : std::is_base_of<
        std::random_access_iterator_tag,
        typename iterator_traits<Iterator>::iterator_category
    > {};
    
    template<typename Iterator>
    constexpr bool is_random_access_iterator_v = is_random_access_iterator<Iterator>::value;
    
    /**
     * @brief Utility to remove cv qualifiers and references
     */
    template<typename T>
    using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;
    
    /**
     * @brief Check if types are the same after removing cv and ref qualifiers
     */
    template<typename T, typename U>
    struct is_same_unqualified : std::is_same<remove_cvref_t<T>, remove_cvref_t<U>> {};
    
    template<typename T, typename U>
    constexpr bool is_same_unqualified_v = is_same_unqualified<T, U>::value;
    
} // namespace ds
