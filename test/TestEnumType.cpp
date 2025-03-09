#include "Common.h"

TEST_CASE("Test EnumType", "[bind]") {
    Registry::Reset();
    type<u8>("u8");
    type<u16>("u16");
    type<u32>("u32");
    type<u64>("u64");
    type<i8>("i8");
    type<i16>("i16");
    type<i32>("i32");
    type<i64>("i64");

    EnumType* e_u8 = (EnumType*)type<enum_type_u8>("enum_type_u8").getType();
    EnumType* e_u16 = (EnumType*)type<enum_type_u16>("enum_type_u16").getType();
    EnumType* e_u32 = (EnumType*)type<enum_type_u32>("enum_type_u32").getType();
    EnumType* e_u64 = (EnumType*)type<enum_type_u64>("enum_type_u64").getType();
    EnumType* e_i8 = (EnumType*)type<enum_type_i8>("enum_type_i8").getType();
    EnumType* e_i16 = (EnumType*)type<enum_type_i16>("enum_type_i16").getType();
    EnumType* e_i32 = (EnumType*)type<enum_type_i32>("enum_type_i32").getType();
    EnumType* e_i64 = (EnumType*)type<enum_type_i64>("enum_type_i64").getType();
    EnumType* ec_u8 = (EnumType*)type<enum_class_type_u8>("enum_class_type_u8").getType();
    EnumType* ec_u16 = (EnumType*)type<enum_class_type_u16>("enum_class_type_u16").getType();
    EnumType* ec_u32 = (EnumType*)type<enum_class_type_u32>("enum_class_type_u32").getType();
    EnumType* ec_u64 = (EnumType*)type<enum_class_type_u64>("enum_class_type_u64").getType();
    EnumType* ec_i8 = (EnumType*)type<enum_class_type_i8>("enum_class_type_i8").getType();
    EnumType* ec_i16 = (EnumType*)type<enum_class_type_i16>("enum_class_type_i16").getType();
    EnumType* ec_i32 = (EnumType*)type<enum_class_type_i32>("enum_class_type_i32").getType();
    EnumType* ec_i64 = (EnumType*)type<enum_class_type_i64>("enum_class_type_i64").getType();

    REQUIRE(e_u8->isConvertibleTo(ec_u8));
    REQUIRE(e_u16->isConvertibleTo(ec_u16));
    REQUIRE(e_u32->isConvertibleTo(ec_u32));
    REQUIRE(e_u64->isConvertibleTo(ec_u64));
    REQUIRE(e_i8->isConvertibleTo(ec_i8));
    REQUIRE(e_i16->isConvertibleTo(ec_i16));
    REQUIRE(e_i32->isConvertibleTo(ec_i32));
    REQUIRE(e_i64->isConvertibleTo(ec_i64));

    REQUIRE(e_u8->isConvertibleTo(Registry::GetType<u8>()));
    REQUIRE(e_u8->getInfo().is_enum == 1);
    REQUIRE(e_u16->isConvertibleTo(Registry::GetType<u16>()));
    REQUIRE(e_u16->getInfo().is_enum == 1);
    REQUIRE(e_u32->isConvertibleTo(Registry::GetType<u32>()));
    REQUIRE(e_u32->getInfo().is_enum == 1);
    REQUIRE(e_u64->isConvertibleTo(Registry::GetType<u64>()));
    REQUIRE(e_u64->getInfo().is_enum == 1);
    REQUIRE(e_i8->isConvertibleTo(Registry::GetType<i8>()));
    REQUIRE(e_i8->getInfo().is_enum == 1);
    REQUIRE(e_i16->isConvertibleTo(Registry::GetType<i16>()));
    REQUIRE(e_i16->getInfo().is_enum == 1);
    REQUIRE(e_i32->isConvertibleTo(Registry::GetType<i32>()));
    REQUIRE(e_i32->getInfo().is_enum == 1);
    REQUIRE(e_i64->isConvertibleTo(Registry::GetType<i64>()));
    REQUIRE(e_i64->getInfo().is_enum == 1);

    EnumTypeBuilder b = extend<enum_type_u8>();
    b.addEnumValue("a", (enum_type_u8)0);
    b.addEnumValue("b", (enum_type_u8)1);
    b.addEnumValue("c", (enum_type_u8)2);

    REQUIRE(e_u8->getProps().size() == 0);
    REQUIRE(e_u8->getFields().size() == 3);
    REQUIRE(e_u8->getField("a") != nullptr);
    REQUIRE(e_u8->getField("a")->value.i == 0);
    REQUIRE(e_u8->getField("b") != nullptr);
    REQUIRE(e_u8->getField("b")->value.i == 1);
    REQUIRE(e_u8->getField("c") != nullptr);
    REQUIRE(e_u8->getField("c")->value.i == 2);
}