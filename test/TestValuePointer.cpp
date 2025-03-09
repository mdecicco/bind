#include "Common.h"

TEST_CASE("Test ValuePointer", "[bind]") {
    Registry::Reset();

    type<u32>("u32");

    u32 value = 3;
    ValuePointer* vp = global("value", &value);

    REQUIRE(vp->getAddress() == &value);
    REQUIRE(vp == Registry::GetValue(vp->getSymbolId()));
    REQUIRE(vp->getType()->isEqualTo(Registry::GetType<u32>()));
}