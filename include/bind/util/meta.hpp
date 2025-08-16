#pragma once
#include <bind/types.h>

#include <type_traits>
#include <typeindex>
#include <typeinfo>

namespace bind {
    template <typename T, typename = void>
    constexpr bool is_type_complete_v = std::is_same_v<T, void>;

    template <typename T>
    constexpr bool is_type_complete_v<T, std::void_t<decltype(sizeof(T))>> = true;

    template <typename T>
    constexpr bool is_type_valid_opaque_v = std::is_pointer_v<T> && !is_type_complete_v<std::remove_pointer_t<T>>;

    template <typename T>
    inline std::size_t type_hash() {
        if constexpr (!is_type_complete_v<T>) {
            return 0;
        } else if constexpr (std::is_reference_v<T>) {
            // `some_type&` has the same hash as `some_type`
            // just use `some_type*`, since it's effectively
            // the same as `some_type&` as far as I know, except
            // that it has a different hash
            return std::type_index(typeid(std::remove_reference_t<T>*)).hash_code();
        } else {
            return std::type_index(typeid(T)).hash_code();
        }
    }

    template <typename T>
    inline const char* type_name() {
        if constexpr (!is_type_complete_v<T>) {
            return "Undefined Type";
        } else if constexpr (std::is_same_v<void, T>) {
            return "void";
        } else {
            return std::type_index(typeid(T)).name();
        }
    }

    template <typename T>
    inline type_meta meta() {
        if constexpr (!is_type_complete_v<T>) {
            return {
                0, // size
                0, // is_trivial
                0, // is_standard_layout
                0, // is_trivially_constructible
                0, // is_trivially_copyable
                0, // is_trivially_destructible
                0, // is_primitive
                0, // is_floating_point
                0, // is_integral
                0, // is_unsigned
                0, // is_function
                0, // is_pointer
                0, // is_alias
                0, // is_enum
                1, // is_opaque
                0  // padding
            };
        } else {
            u16 sz = 0;
            if constexpr (!std::is_same_v<void, T>) {
                sz = (u16)sizeof(T);
            }
            return {
                sz,                                             // size
                std::is_trivial_v<T>,                           // is_trivial
                std::is_standard_layout_v<T>,                   // is_standard_layout
                __has_trivial_constructor(T),                   // is_trivially_constructible
                std::is_trivially_copyable_v<T>,                // is_trivially_copyable
                __has_trivial_destructor(T),                    // is_trivially_destructible
                std::is_fundamental_v<T>,                       // is_primitive
                std::is_floating_point_v<T>,                    // is_floating_point
                std::is_integral_v<T>,                          // is_integral
                std::is_unsigned_v<T>,                          // is_unsigned
                std::is_function_v<T>,                          // is_function
                std::is_pointer_v<T> || std::is_reference_v<T>, // is_pointer
                0,                                              // is_alias
                0,                                              // is_enum
                0,                                              // is_opaque
                0                                               // padding
            };
        }
    }
};