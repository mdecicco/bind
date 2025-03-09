#include "Common.h"

template <typename T>
void testPrimitiveMetadata(const char* name) {
    Registry::Reset();

    PrimitiveTypeBuilder<T> b = type<T>(name);
    const type_meta& info = b.getType()->getInfo();

    if constexpr (std::is_same_v<T, void>) {
        REQUIRE(info.size == 0);
        REQUIRE(info.is_trivial == 0);
        REQUIRE(info.is_standard_layout == 0);
        REQUIRE(info.is_trivially_constructible == 0);
        REQUIRE(info.is_trivially_copyable == 0);
        REQUIRE(info.is_trivially_destructible == 0);
        REQUIRE(info.is_floating_point == 0);
        REQUIRE(info.is_integral == 0);
        REQUIRE(info.is_unsigned == 0);
    } else {
        REQUIRE(size_t(info.size) == sizeof(T));
        REQUIRE(info.is_trivial == 1);
        REQUIRE(info.is_standard_layout == 1);
        REQUIRE(info.is_trivially_constructible == 1);
        REQUIRE(info.is_trivially_copyable == 1);
        REQUIRE(info.is_trivially_destructible == 1);
        REQUIRE(info.is_floating_point == (std::is_floating_point_v<T> ? 1 : 0));
        REQUIRE(info.is_integral == (std::is_integral_v<T> ? 1 : 0));
        REQUIRE(info.is_unsigned == (std::is_unsigned_v<T> ? 1 : 0));
    }

    REQUIRE(info.is_primitive == 1);
    REQUIRE(info.is_function == 0);
    REQUIRE(info.is_pointer == 0);
    REQUIRE(info.is_alias == 0);
    REQUIRE(info.is_enum == 0);
}

template <typename T>
void testEnumMetadata(const char* name) {
    Registry::Reset();

    EnumTypeBuilder<T> b = type<T>(name);
    const type_meta& info = b.getType()->getInfo();

    REQUIRE(size_t(info.size) == sizeof(T));
    REQUIRE(info.is_trivial == 1);
    REQUIRE(info.is_standard_layout == 1);
    REQUIRE(info.is_trivially_constructible == 1);
    REQUIRE(info.is_trivially_copyable == 1);
    REQUIRE(info.is_trivially_destructible == 1);
    REQUIRE(info.is_floating_point == (std::is_floating_point_v<T> ? 1 : 0));
    REQUIRE(info.is_integral == 1);
    REQUIRE(info.is_unsigned == (std::is_unsigned_v<T> ? 1 : 0));
    REQUIRE(info.is_primitive == 1);
    REQUIRE(info.is_function == 0);
    REQUIRE(info.is_pointer == 0);
    REQUIRE(info.is_alias == 0);
    REQUIRE(info.is_enum == 1);
}

template <typename T>
void testPointerMetadata(const char* name) {
    Registry::Reset();
    
    auto b = type<T>(name);
    const type_meta& info = b.getType()->getPointerType()->getInfo();

    REQUIRE(size_t(info.size) == sizeof(void*));
    REQUIRE(info.is_trivial == 1);
    REQUIRE(info.is_standard_layout == 1);
    REQUIRE(info.is_trivially_constructible == 1);
    REQUIRE(info.is_trivially_copyable == 1);
    REQUIRE(info.is_trivially_destructible == 1);
    REQUIRE(info.is_floating_point == 0);
    REQUIRE(info.is_integral == 0);
    REQUIRE(info.is_unsigned == 0);
    REQUIRE(info.is_primitive == 0);
    REQUIRE(info.is_function == 0);
    REQUIRE(info.is_pointer == 1);
    REQUIRE(info.is_alias == 0);
    REQUIRE(info.is_enum == 0);
}

template <typename T>
void testObjectMetadata(const char* name) {
    Registry::Reset();

    ObjectTypeBuilder<T> b = type<T>(name);
    const type_meta& info = b.getType()->getInfo();
    REQUIRE(size_t(info.size) == sizeof(T));
    REQUIRE(info.is_trivial == (std::is_trivial_v<T> ? 1 : 0));
    REQUIRE(info.is_standard_layout == (std::is_standard_layout_v<T> ? 1 : 0));
    REQUIRE(info.is_trivially_constructible == (__has_trivial_constructor(T) ? 1 : 0));
    REQUIRE(info.is_trivially_copyable == (std::is_trivially_copyable_v<T> ? 1 : 0));
    REQUIRE(info.is_trivially_destructible == (__has_trivial_destructor(T) ? 1 : 0));
    REQUIRE(info.is_floating_point == 0);
    REQUIRE(info.is_integral == 0);
    REQUIRE(info.is_unsigned == 0);
    REQUIRE(info.is_primitive == 0);
    REQUIRE(info.is_function == 0);
    REQUIRE(info.is_pointer == 0);
    REQUIRE(info.is_alias == 0);
    REQUIRE(info.is_enum == 0);
}

template <typename First, typename... Rest>
void bindTypes(const char* prefix, u32 index) {
    if (!Registry::GetType<First>()) {
        type<First>(String::Format("%s_%d", prefix, index));
    }

    if constexpr (std::tuple_size_v<std::tuple<Rest...>> > 0) {
        bindTypes<Rest...>(prefix, index++);
    }
}

template <typename Ret, typename... Args>
void testFunctionPointerMetadata() {
    Registry::Reset();

    type<Ret>("return_type");
    if constexpr (std::tuple_size_v<std::tuple<Args...>> > 0) bindTypes<Args...>("arg_type", 0);

    FunctionType* sig = Registry::Signature<Ret, Args...>();
    const type_meta& info = sig->getInfo();

    REQUIRE(size_t(info.size) == sizeof(void*));
    REQUIRE(info.is_trivial == 1);
    REQUIRE(info.is_standard_layout == 1);
    REQUIRE(info.is_trivially_constructible == 1);
    REQUIRE(info.is_trivially_copyable == 1);
    REQUIRE(info.is_trivially_destructible == 1);
    REQUIRE(info.is_floating_point == 0);
    REQUIRE(info.is_integral == 0);
    REQUIRE(info.is_unsigned == 0);
    REQUIRE(info.is_primitive == 0);
    REQUIRE(info.is_function == 1);
    REQUIRE(info.is_pointer == 1);
    REQUIRE(info.is_alias == 0);
    REQUIRE(info.is_enum == 0);
}

template <typename Ret, typename Cls, typename... Args>
void testMethodPointerMetadata() {
    Registry::Reset();

    type<Ret>("return_type");
    type<Cls>("class_type");
    if constexpr (std::tuple_size_v<std::tuple<Args...>> > 0) bindTypes<Args...>("arg_type", 0);

    FunctionType* sig = Registry::MethodSignature<Ret, Cls, Args...>();
    const type_meta& info = sig->getInfo();

    REQUIRE(size_t(info.size) == sizeof(void*));
    REQUIRE(info.is_trivial == 1);
    REQUIRE(info.is_standard_layout == 1);
    REQUIRE(info.is_trivially_constructible == 1);
    REQUIRE(info.is_trivially_copyable == 1);
    REQUIRE(info.is_trivially_destructible == 1);
    REQUIRE(info.is_floating_point == 0);
    REQUIRE(info.is_integral == 0);
    REQUIRE(info.is_unsigned == 0);
    REQUIRE(info.is_primitive == 0);
    REQUIRE(info.is_function == 1);
    REQUIRE(info.is_pointer == 1);
    REQUIRE(info.is_alias == 0);
    REQUIRE(info.is_enum == 0);
}



TEST_CASE("Primitive types are created with the correct metadata", "[bind]") {
    SECTION("void") { testPrimitiveMetadata<void>("void"); }
    SECTION("u64") { testPrimitiveMetadata<u64>("u64"); }
    SECTION("i64") { testPrimitiveMetadata<i64>("i64"); }
    SECTION("u32") { testPrimitiveMetadata<u32>("u32"); }
    SECTION("i32") { testPrimitiveMetadata<i32>("i32"); }
    SECTION("u16") { testPrimitiveMetadata<u16>("u16"); }
    SECTION("i16") { testPrimitiveMetadata<i16>("i16"); }
    SECTION("u8") { testPrimitiveMetadata<u8>("u8"); }
    SECTION("i8") { testPrimitiveMetadata<i8>("i8"); }
    SECTION("f32") { testPrimitiveMetadata<f32>("f32"); }
    SECTION("f64") { testPrimitiveMetadata<f64>("f64"); }
}

TEST_CASE("Enum types are created with the correct metadata", "[bind]") {
    SECTION("enum_type") { testEnumMetadata<enum_type>("enum_type"); }
    SECTION("enum_class_type") { testEnumMetadata<enum_class_type>("enum_class_type"); }
}

TEST_CASE("Pointer types are created with the correct metadata", "[bind]") {
    SECTION("void") { testPointerMetadata<void>("void"); }
    SECTION("u64") { testPointerMetadata<u64>("u64"); }
    SECTION("i64") { testPointerMetadata<i64>("i64"); }
    SECTION("u32") { testPointerMetadata<u32>("u32"); }
    SECTION("i32") { testPointerMetadata<i32>("i32"); }
    SECTION("u16") { testPointerMetadata<u16>("u16"); }
    SECTION("i16") { testPointerMetadata<i16>("i16"); }
    SECTION("u8") { testPointerMetadata<u8>("u8"); }
    SECTION("i8") { testPointerMetadata<i8>("i8"); }
    SECTION("f32") { testPointerMetadata<f32>("f32"); }
    SECTION("f64") { testPointerMetadata<f64>("f64"); }
    SECTION("test_struct") { testPointerMetadata<plain_struct>("test_struct"); }
}

TEST_CASE("Object types are created with the correct metadata", "[bind]") {
    SECTION("plain_struct") { testObjectMetadata<plain_struct>("plain_struct"); }
    SECTION("non_trivial_ctor") { testObjectMetadata<non_trivial_ctor>("non_trivial_ctor"); }
    SECTION("non_trivial_copy") { testObjectMetadata<non_trivial_copy>("non_trivial_copy"); }
    SECTION("non_trivial_dtor") { testObjectMetadata<non_trivial_dtor>("non_trivial_dtor"); }
}

TEST_CASE("Function types are created with the correct metadata", "[bind]") {
    SECTION("Normal functions") {
        testFunctionPointerMetadata<void>();
        testFunctionPointerMetadata<void, i32>();
    }

    SECTION("Object methods") {
        testMethodPointerMetadata<void, struct_with_methods>();
        testMethodPointerMetadata<void, struct_with_methods, i32>();
    }
}