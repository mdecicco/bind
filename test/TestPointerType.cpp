#include "Common.h"

TEST_CASE("Test PointerType", "[bind]") {
    Registry::Reset();

    DataType* sint = type<i32>("i32");
    PointerType* ptr0 = sint->getPointerType();
    PointerType* ptr1 = ptr0->getPointerType();

    REQUIRE(ptr0->getName() == "i32*");
    REQUIRE(ptr1->getName() == "i32**");

    // pointer type should be separate from the pointed-to type
    REQUIRE(ptr0 != sint);
    REQUIRE(ptr1 != ptr0);

    // and not be equal to it
    REQUIRE(!ptr0->isEqualTo(sint));
    REQUIRE(!ptr0->isEqualTo(ptr1));
    REQUIRE(!ptr1->isEqualTo(sint));
    REQUIRE(!ptr1->isEqualTo(ptr0));

    // and should refer to the original
    REQUIRE(ptr0->getDestinationType() == sint);
    REQUIRE(ptr1->getDestinationType() == ptr0);

    // Also, the type info for all pointers should be the same
    type_meta i0 = meta<void*>();
    const type_meta& i1 = ptr0->getInfo();
    const type_meta& i2 = ptr1->getInfo();
    
    REQUIRE(i0.is_pointer == 1);
    REQUIRE(i1.is_pointer == 1);
    REQUIRE(i2.is_pointer == 1);
    
    REQUIRE(i0.size == i1.size);
    REQUIRE(i0.is_trivial == i1.is_trivial);
    REQUIRE(i0.is_standard_layout == i1.is_standard_layout);
    REQUIRE(i0.is_trivially_constructible == i1.is_trivially_constructible);
    REQUIRE(i0.is_trivially_copyable == i1.is_trivially_copyable);
    REQUIRE(i0.is_trivially_destructible == i1.is_trivially_destructible);
    REQUIRE(i0.is_primitive == i1.is_primitive);
    REQUIRE(i0.is_floating_point == i1.is_floating_point);
    REQUIRE(i0.is_integral == i1.is_integral);
    REQUIRE(i0.is_unsigned == i1.is_unsigned);
    REQUIRE(i0.is_function == i1.is_function);
    REQUIRE(i0.is_pointer == i1.is_pointer);
    REQUIRE(i0.is_enum == i1.is_enum);

    REQUIRE(i0.size == i2.size);
    REQUIRE(i0.is_trivial == i2.is_trivial);
    REQUIRE(i0.is_standard_layout == i2.is_standard_layout);
    REQUIRE(i0.is_trivially_constructible == i2.is_trivially_constructible);
    REQUIRE(i0.is_trivially_copyable == i2.is_trivially_copyable);
    REQUIRE(i0.is_trivially_destructible == i2.is_trivially_destructible);
    REQUIRE(i0.is_primitive == i2.is_primitive);
    REQUIRE(i0.is_floating_point == i2.is_floating_point);
    REQUIRE(i0.is_integral == i2.is_integral);
    REQUIRE(i0.is_unsigned == i2.is_unsigned);
    REQUIRE(i0.is_function == i2.is_function);
    REQUIRE(i0.is_pointer == i2.is_pointer);
    REQUIRE(i0.is_enum == i2.is_enum);
}