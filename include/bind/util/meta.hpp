#pragma once
#include <bind/types.h>

#include <type_traits>
#include <typeinfo>
#include <typeindex>

namespace bind {
    template <typename T>
    inline std::size_t type_hash() {
        if constexpr (std::is_reference_v<T>) {
            // `some_type&` has the same hash as `some_type`
            // just use `some_type*`, since it's effectively
            // the same as `some_type&` as far as I know, except
            // that it has a different hash
            return std::type_index(typeid(std::remove_reference_t<T>*)).hash_code();
        }

        return std::type_index(typeid(T)).hash_code();
    }

    template <typename T>
    inline const char* type_name() {
        if constexpr (std::is_same_v<void, T>) return "void";
        else return std::type_index(typeid(T)).name();
    }

    template <typename T>
    inline type_meta meta() {
        u16 sz = 0;
        if constexpr (!std::is_same_v<void, T>) sz = (u16)sizeof(T);
        return {
            sz,                                                   // size
            std::is_trivial_v<T>,                                 // is_trivial
            std::is_standard_layout_v<T>,                         // is_standard_layout
            __has_trivial_constructor(T),                         // is_trivially_constructible
            std::is_trivially_copyable_v<T>,                      // is_trivially_copyable
            __has_trivial_destructor(T),                          // is_trivially_destructible
            std::is_fundamental_v<T>,                             // is_primitive
            std::is_floating_point_v<T>,                          // is_floating_point
            std::is_integral_v<T>,                                // is_integral
            std::is_unsigned_v<T>,                                // is_unsigned
            std::is_function_v<T>,                                // is_function
            std::is_pointer_v<T> || std::is_reference_v<T>,       // is_pointer
            0,                                                    // is_alias
            0,                                                    // is_enum
            0                                                     // padding
        };
    }
};