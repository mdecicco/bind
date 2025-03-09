#include "Common.h"

TEST_CASE("Test AliasType", "[bind]") {
    Registry::Reset();

    DataType* sint = type<i32>("i32");
    AliasType* alias0 = alias<i32>("alias0");
    AliasType* alias1 = alias("alias1", alias0);

    REQUIRE(alias0->getName() == "alias0");
    REQUIRE(alias1->getName() == "alias1");

    // alias type should be separate from the aliased type
    REQUIRE(alias0 != sint);
    REQUIRE(alias1 != alias0);

    // but be equal to it
    REQUIRE(alias0->isEqualTo(sint));
    REQUIRE(alias0->isEqualTo(alias1));
    REQUIRE(alias1->isEqualTo(sint));
    REQUIRE(alias1->isEqualTo(alias0));

    // and should refer to the original
    REQUIRE(alias0->getAliasedType() == sint);
    REQUIRE(alias1->getAliasedType() == alias0);

    // base type should be the start of the chain of aliases
    REQUIRE(alias0->getBaseType() == sint);
    REQUIRE(alias1->getBaseType() == sint);

    // Also, the type info should be identical no matter how long the chain of aliases is

    const type_meta& i0 = sint->getInfo();
    const type_meta& i1 = alias0->getInfo();
    const type_meta& i2 = alias1->getInfo();
    
    REQUIRE(i0.is_alias == 0);
    REQUIRE(i1.is_alias == 1);
    REQUIRE(i2.is_alias == 1);
    
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