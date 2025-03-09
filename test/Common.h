#include <catch2/catch_test_macros.hpp>
#include <bind/bind.h>
using namespace bind;
using namespace utils;

struct plain_struct { i32 a, b, c; };
struct equivalent_to_plain_struct { i32 a, b, c; };
struct similar_to_plain_struct { i32 a, b, c, d; };

struct non_trivial_ctor {
    non_trivial_ctor() {}
    i32 a, b, c;
};

struct non_trivial_copy {
    non_trivial_copy(const non_trivial_copy&) {}
    i32 a, b, c;
};

struct non_trivial_dtor {
    ~non_trivial_dtor() {}
    i32 a, b, c;
};

struct struct_with_methods {
    i32 a, b, c;
    struct_with_methods() { a = b = c = 0; }
    struct_with_methods(i32 d) { a = b = c = d; }
    struct_with_methods(i64 d) { a = b = c = i32(d); }

    void method0() {}
    void method1(i32 d) {}
    i32 method2(i32 d) { return a + d; }
    i64 method3(i64 d) { return a + d; }

    operator i32() { return a; }
    operator i64() { return a; }
};

enum enum_type {
    et_a,
    et_b,
    et_c
};

enum class enum_class_type { a, b, c };

enum enum_type_u8 : u8 {};
enum enum_type_u16 : u16 {};
enum enum_type_u32 : u32 {};
enum enum_type_u64 : u64 {};
enum enum_type_i8 : i8 {};
enum enum_type_i16 : i16 {};
enum enum_type_i32 : i32 {};
enum enum_type_i64 : i64 {};

enum class enum_class_type_u8 : u8 {};
enum class enum_class_type_u16 : u16 {};
enum class enum_class_type_u32 : u32 {};
enum class enum_class_type_u64 : u64 {};
enum class enum_class_type_i8 : i8 {};
enum class enum_class_type_i16 : i16 {};
enum class enum_class_type_i32 : i32 {};
enum class enum_class_type_i64 : i64 {};