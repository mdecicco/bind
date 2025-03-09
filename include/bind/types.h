#pragma once
#include <utils/types.h>

namespace bind {
    using namespace utils;
    
    typedef u32 AccessFlags;
    typedef u64 symbol_id;

    constexpr u32 PublicAccess = 0;
    constexpr u32 FullAccessRights = 0xFFFFFFFF;

    constexpr const char* ConstructorName = "$ctor";
    constexpr u32 ConstructorNameLen = 5;
    constexpr const char* DestructorName = "$dtor";
    constexpr u32 DestructorNameLen = 5;
    constexpr const char* CastOperatorName = "$cast";
    constexpr u32 CastOperatorNameLen = 5;

    struct type_meta {
        unsigned size : 16;
        unsigned is_trivial : 1;
        unsigned is_standard_layout : 1;
        unsigned is_trivially_constructible : 1;
        unsigned is_trivially_copyable : 1;
        unsigned is_trivially_destructible : 1;
        unsigned is_primitive : 1;
        unsigned is_floating_point : 1;
        unsigned is_integral : 1;
        unsigned is_unsigned : 1;
        unsigned is_function : 1;
        unsigned is_pointer : 1;
        unsigned is_alias : 1;
        unsigned is_enum : 1;
        unsigned __pad0 : 3;
    };
};