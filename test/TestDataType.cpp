#include "Common.h"

void testCommonMethods() {
    Registry::Reset();

    DataType* sint = type<i32>("i32");

    REQUIRE(sint->getOwnNamespace()->getName() == "i32");
    REQUIRE(sint->getOwnNamespace()->getFullName() == "i32");
    REQUIRE(sint->getOwnNamespace()->getSymbolName() == sint->getSymbolName());
    REQUIRE(sint->getOwnNamespace()->getSymbolId() == sint->getSymbolId());
}

template <typename T1, typename T2>
void testPrimitiveTypeMethods1() {
    if constexpr (std::is_same_v<T1, T2>) {
        REQUIRE(Registry::GetType<T1>()->isEqualTo(Registry::GetType<T2>()));
    } else {
        REQUIRE(!Registry::GetType<T1>()->isEqualTo(Registry::GetType<T2>()));
    }
    
    if constexpr (std::is_same_v<T1, void>) {
        if constexpr (std::is_same_v<T2, void>) {
            // void is convertible to void
            REQUIRE(Registry::GetType<T1>()->isConvertibleTo(Registry::GetType<T2>()));
        } else {
            // void is convertible to nothing
            REQUIRE(!Registry::GetType<T1>()->isConvertibleTo(Registry::GetType<T2>()));
        }
    } else if constexpr (std::is_same_v<T2, void>) {
        // nothing is convertible to void
        REQUIRE(!Registry::GetType<T1>()->isConvertibleTo(Registry::GetType<T2>()));
    } else {
        // all non-void primitives are convertible to each other
        REQUIRE(Registry::GetType<T1>()->isConvertibleTo(Registry::GetType<T2>()));
    }
}

template <typename T>
void testPrimitiveTypeMethods() {
    testPrimitiveTypeMethods1<T, void>();
    testPrimitiveTypeMethods1<T, bool>();
    testPrimitiveTypeMethods1<T, i8>();
    testPrimitiveTypeMethods1<T, i16>();
    testPrimitiveTypeMethods1<T, i32>();
    testPrimitiveTypeMethods1<T, i64>();
    testPrimitiveTypeMethods1<T, u8>();
    testPrimitiveTypeMethods1<T, u16>();
    testPrimitiveTypeMethods1<T, u32>();
    testPrimitiveTypeMethods1<T, u64>();
    testPrimitiveTypeMethods1<T, f32>();
    testPrimitiveTypeMethods1<T, f64>();

    REQUIRE(Registry::GetType<T>()->isEqualTo(Registry::GetType<T>()->getEffectiveType()));
}

void testMethodsOnPrimitives() {
    Registry::Reset();

    type<void>("void");
    type<bool>("bool");
    type<i8>("i8");
    type<i16>("i16");
    type<i32>("i32");
    type<i64>("i64");
    type<u8>("u8");
    type<u16>("u16");
    type<u32>("u32");
    type<u64>("u64");
    type<f32>("f32");
    type<f64>("f64");

    testPrimitiveTypeMethods<void>();
    testPrimitiveTypeMethods<bool>();
    testPrimitiveTypeMethods<i8>();
    testPrimitiveTypeMethods<i16>();
    testPrimitiveTypeMethods<i32>();
    testPrimitiveTypeMethods<i64>();
    testPrimitiveTypeMethods<u8>();
    testPrimitiveTypeMethods<u16>();
    testPrimitiveTypeMethods<u32>();
    testPrimitiveTypeMethods<u64>();
    testPrimitiveTypeMethods<f32>();
    testPrimitiveTypeMethods<f64>();
}

template <typename T1, typename T2>
void testPointerTypeMethods1() {
    if constexpr (std::is_same_v<T1, T2>) {
        REQUIRE(Registry::GetType<T1*>()->isEqualTo(Registry::GetType<T2*>()));
    } else {
        REQUIRE(!Registry::GetType<T1*>()->isEqualTo(Registry::GetType<T2*>()));
    }
}

template <typename T>
void testPointerTypeMethods() {
    testPointerTypeMethods1<T, void>();
    testPointerTypeMethods1<T, bool>();
    testPointerTypeMethods1<T, i8>();
    testPointerTypeMethods1<T, i16>();
    testPointerTypeMethods1<T, i32>();
    testPointerTypeMethods1<T, i64>();
    testPointerTypeMethods1<T, u8>();
    testPointerTypeMethods1<T, u16>();
    testPointerTypeMethods1<T, u32>();
    testPointerTypeMethods1<T, u64>();
    testPointerTypeMethods1<T, f32>();
    testPointerTypeMethods1<T, f64>();

    REQUIRE(Registry::GetType<T>()->getPointerType()->isEqualTo(Registry::GetType<T>()->getPointerType()->getEffectiveType()));
}

void testMethodsOnPointers() {
    Registry::Reset();

    type<void>("void");
    type<bool>("bool");
    type<i8>("i8");
    type<i16>("i16");
    type<i32>("i32");
    type<i64>("i64");
    type<u8>("u8");
    type<u16>("u16");
    type<u32>("u32");
    type<u64>("u64");
    type<f32>("f32");
    type<f64>("f64");

    testPointerTypeMethods<void>();
    testPointerTypeMethods<bool>();
    testPointerTypeMethods<i8>();
    testPointerTypeMethods<i16>();
    testPointerTypeMethods<i32>();
    testPointerTypeMethods<i64>();
    testPointerTypeMethods<u8>();
    testPointerTypeMethods<u16>();
    testPointerTypeMethods<u32>();
    testPointerTypeMethods<u64>();
    testPointerTypeMethods<f32>();
    testPointerTypeMethods<f64>();
}

void testMethodsOnObjects() {
    Registry::Reset();

    type<void>("void");
    type<bool>("bool");
    type<i8>("i8");
    type<i16>("i16");
    type<i32>("i32");
    type<i64>("i64");
    type<u8>("u8");
    type<u16>("u16");
    type<u32>("u32");
    type<u64>("u64");
    type<f32>("f32");
    type<f64>("f64");
    
    ObjectTypeBuilder<plain_struct> bobj0 = type<plain_struct>("plain_struct");
    ObjectTypeBuilder<similar_to_plain_struct> bobj1 = type<similar_to_plain_struct>("similar_to_plain_struct");
    ObjectTypeBuilder<non_trivial_copy> bobj2 = type<non_trivial_copy>("non_trivial_copy");
    ObjectTypeBuilder<equivalent_to_plain_struct> bobj3 = type<equivalent_to_plain_struct>("equivalent_to_plain_struct");
    DataType* obj0 = bobj0;
    DataType* obj1 = bobj1;
    DataType* obj2 = bobj2;
    DataType* obj3 = bobj3;

    REQUIRE(obj0->isEqualTo(obj0));
    REQUIRE(obj1->isEqualTo(obj1));
    REQUIRE(!obj0->isEqualTo(obj1));
    REQUIRE(!obj1->isEqualTo(obj0));

    // empty structs are not convertible to each other if they are not fully
    // defined
    REQUIRE(!obj0->isConvertibleTo(obj1));
    REQUIRE(!obj1->isConvertibleTo(obj0));
    REQUIRE(!obj2->isConvertibleTo(obj0));
    REQUIRE(!obj2->isConvertibleTo(obj1));

    // empty structs are not convertible to each other when the destination type
    // is not trivially copyable
    REQUIRE(!obj0->isConvertibleTo(obj2));
    REQUIRE(!obj1->isConvertibleTo(obj2));

    // add the props
    bobj0.prop("a", &plain_struct::a);
    bobj0.prop("b", &plain_struct::b);
    bobj0.prop("c", &plain_struct::c);

    bobj1.prop("a", &similar_to_plain_struct::a);
    bobj1.prop("b", &similar_to_plain_struct::b);
    bobj1.prop("c", &similar_to_plain_struct::c);
    bobj1.prop("d", &similar_to_plain_struct::d);
    
    bobj2.prop("a", &non_trivial_copy::a);
    bobj2.prop("b", &non_trivial_copy::b);
    bobj2.prop("c", &non_trivial_copy::c);
    
    bobj3.prop("a", &equivalent_to_plain_struct::a);
    bobj3.prop("b", &equivalent_to_plain_struct::b);
    bobj3.prop("c", &equivalent_to_plain_struct::c);

    // structs are now fully defined, the trivially copyable ones with the same
    // shape should now be convertible to each other
    REQUIRE(obj0->isConvertibleTo(obj3));
    REQUIRE(obj3->isConvertibleTo(obj0));

    // All properties of obj0, obj3 can be sourced from obj1
    REQUIRE(obj1->isConvertibleTo(obj0));
    REQUIRE(obj1->isConvertibleTo(obj3));

    // All properties of obj2 can be sourced from obj1, but
    // obj2 is not trivially copyable
    REQUIRE(!obj1->isConvertibleTo(obj2));

    // but that doesn't go both ways
    REQUIRE(!obj0->isConvertibleTo(obj1));
    REQUIRE(!obj3->isConvertibleTo(obj1));

    // copy ctor not added yet
    REQUIRE(!obj2->isConstructableWith({ obj2->getPointerType() }));

    // add copy contstructor
    bobj2.ctor<const non_trivial_copy&>();

    // now it should work
    REQUIRE(obj2->isConstructableWith({ obj2->getPointerType() }));
}

void test_findProperties() {
    Registry::Reset();
    type<i32>("i32");
    type<void>("void");

    ObjectTypeBuilder<plain_struct> obj = type<plain_struct>("plain_struct");
    DataType* tp = obj;

    REQUIRE(tp->getProps().size() == 0);

    obj.prop("a", &plain_struct::a);
    obj.prop("b", &plain_struct::b).accessFlags = 0b00000001000000000000000000000000; // arbitrary bit
    obj.prop("c", &plain_struct::c);
    obj.pseudoMethod("c", +[](plain_struct* self) {});

    REQUIRE(tp->getProps().size() == 4);

    utils::Array<const DataType::Property *> results;
    
    results = tp->findProperties("a");
    REQUIRE(results.size() == 1);
    REQUIRE(results[0]->name == "a");
    REQUIRE(results[0]->offset == 0);
    
    results = tp->findProperties("b");
    REQUIRE(results.size() == 1);
    REQUIRE(results[0]->name == "b");
    REQUIRE(results[0]->offset == 4);
    
    results = tp->findProperties("b", 0b00000001000000000000000000000000);
    REQUIRE(results.size() == 1);
    REQUIRE(results[0]->name == "b");
    REQUIRE(results[0]->offset == 4);
    
    results = tp->findProperties("b", 0b11111110111111111111111111111111);
    REQUIRE(results.size() == 0);

    results = tp->findProperties("c");
    REQUIRE(results.size() == 2);
    REQUIRE(results[0]->name == "c");
    REQUIRE(results[0]->offset == 8);
    REQUIRE(results[1]->name == "c");
    REQUIRE(results[1]->offset == -1);
    REQUIRE(results[1]->flags.is_pseudo_method == 1);
}

void test_findMethods() {
    Registry::Reset();
    type<i64>("i64");
    type<i32>("i32");
    type<i16>("i16");
    type<void>("void");
    auto b = type<struct_with_methods>("test");
    b.ctor();
    b.ctor<i32>();
    b.ctor<i64>();
    b.method("meth", &struct_with_methods::method0).accessFlags = 0b10001; // arbitrary bits
    b.method("meth", &struct_with_methods::method1);
    b.method("meth", &struct_with_methods::method2);
    b.method("meth", &struct_with_methods::method3);
    DataType* tp = b;

    Function* singleStrict;
    Array<Function*> results;

    // just by name
    singleStrict = nullptr;
    results = tp->findMethods(FuncMatch("meth"), &singleStrict);
    REQUIRE(singleStrict == nullptr);
    REQUIRE(results.size() == 4);

    singleStrict = nullptr;
    results = tp->findMethods(FuncMatch("meth").access(0b10101), &singleStrict);
    REQUIRE(singleStrict == nullptr);
    REQUIRE(results.size() == 4);

    singleStrict = nullptr;
    results = tp->findMethods(FuncMatch("meth").access(0b00101), &singleStrict);
    REQUIRE(singleStrict == nullptr);
    REQUIRE(results.size() == 3);
    
    singleStrict = nullptr;
    results = tp->findMethods(FuncMatch("no_match"), &singleStrict);
    REQUIRE(singleStrict == nullptr);
    REQUIRE(results.size() == 0);

    // by name and return type (not strict)
    singleStrict = nullptr;
    results = tp->findMethods(FuncMatch("meth").retTp(Registry::GetType<void>(), false), &singleStrict);
    REQUIRE(singleStrict == nullptr);
    REQUIRE(results.size() == 2);
    REQUIRE(results[0]->getSignature()->getReturnType()->isEqualTo(Registry::GetType<void>()));
    REQUIRE(results[1]->getSignature()->getReturnType()->isEqualTo(Registry::GetType<void>()));

    // by name and return type (strict)
    singleStrict = nullptr;
    results = tp->findMethods(FuncMatch("meth").retTp(Registry::GetType<void>(), true), &singleStrict);
    REQUIRE(singleStrict == nullptr);
    REQUIRE(results.size() == 2);
    REQUIRE(results[0]->getSignature()->getReturnType()->isEqualTo(Registry::GetType<void>()));
    REQUIRE(results[1]->getSignature()->getReturnType()->isEqualTo(Registry::GetType<void>()));

    // by name, return type, args (return not strict, args not strict)
    singleStrict = nullptr;
    results = tp->findMethods(
        FuncMatch("meth")
            .retTp(Registry::GetType<void>(), false)
            .argTps({ Registry::GetType<i32>() }, false),
        &singleStrict
    );
    REQUIRE(singleStrict != nullptr);
    REQUIRE(results.size() == 1);
    REQUIRE(singleStrict == results[0]);
    REQUIRE(results[0]->getSignature()->getReturnType()->isEqualTo(Registry::GetType<void>()));
    REQUIRE(results[0]->getExplicitArgs()[0].type->isEqualTo(Registry::GetType<i32>()));

    // by name, return type, args (return not strict, args strict)
    singleStrict = nullptr;
    results = tp->findMethods(
        FuncMatch("meth")
            .retTp(Registry::GetType<void>(), false)
            .argTps({ Registry::GetType<i32>() }, true),
        &singleStrict
    );
    REQUIRE(singleStrict != nullptr);
    REQUIRE(results.size() == 1);
    REQUIRE(singleStrict == results[0]);
    REQUIRE(results[0]->getSignature()->getReturnType()->isEqualTo(Registry::GetType<void>()));
    REQUIRE(results[0]->getExplicitArgs()[0].type->isEqualTo(Registry::GetType<i32>()));

    // by name, return type, args (return strict, args strict)
    singleStrict = nullptr;
    results = tp->findMethods(
        FuncMatch("meth")
            .retTp(Registry::GetType<void>(), true)
            .argTps({ Registry::GetType<i32>() }, true),
        &singleStrict
    );
    REQUIRE(singleStrict != nullptr);
    REQUIRE(results.size() == 1);
    REQUIRE(singleStrict == results[0]);
    REQUIRE(results[0]->getSignature()->getReturnType()->isEqualTo(Registry::GetType<void>()));
    REQUIRE(results[0]->getExplicitArgs()[0].type->isEqualTo(Registry::GetType<i32>()));
    
    // one with convertible return type, one with equivalent return type
    singleStrict = nullptr;
    results = tp->findMethods(
        FuncMatch("meth").retTp(Registry::GetType<i64>(), false),
        &singleStrict
    );
    REQUIRE(singleStrict != nullptr);
    REQUIRE(results.size() == 2);
    REQUIRE(singleStrict == results[1]);
    REQUIRE(results[0]->getSignature()->getReturnType()->isEqualTo(Registry::GetType<i32>()));
    REQUIRE(results[1]->getSignature()->getReturnType()->isEqualTo(Registry::GetType<i64>()));
    
    // two with convertible arg types, one with equivalent arg type
    singleStrict = nullptr;
    results = tp->findMethods(
        FuncMatch("meth").argTps({ Registry::GetType<i64>() }, false),
        &singleStrict
    );
    REQUIRE(singleStrict != nullptr);
    REQUIRE(results.size() == 3);
    REQUIRE(singleStrict == results[2]);
    REQUIRE(results[0]->getExplicitArgs()[0].type->isEqualTo(Registry::GetType<i32>()));
    REQUIRE(results[1]->getExplicitArgs()[0].type->isEqualTo(Registry::GetType<i32>()));
    REQUIRE(results[0] != results[1]);
    REQUIRE(results[2]->getExplicitArgs()[0].type->isEqualTo(Registry::GetType<i64>()));
    
    // convertible return type, not strict
    singleStrict = nullptr;
    results = tp->findMethods(
        FuncMatch("meth")
            .retTp(Registry::GetType<i16>(), false),
        &singleStrict
    );
    REQUIRE(singleStrict == nullptr);
    REQUIRE(results.size() == 2);
    REQUIRE(results[0]->getSignature()->getReturnType()->isEqualTo(Registry::GetType<i32>()));
    REQUIRE(results[1]->getSignature()->getReturnType()->isEqualTo(Registry::GetType<i64>()));

    // convertible return type, strict
    singleStrict = nullptr;
    results = tp->findMethods(
        FuncMatch("meth")
            .retTp(Registry::GetType<i16>(), true),
        &singleStrict
    );
    REQUIRE(singleStrict == nullptr);
    REQUIRE(results.size() == 0);
    
    // convertible arg type, not strict
    singleStrict = nullptr;
    results = tp->findMethods(
        FuncMatch("meth")
            .argTps({ Registry::GetType<i16>() }, false),
        &singleStrict
    );
    REQUIRE(singleStrict == nullptr);
    REQUIRE(results.size() == 3);
    REQUIRE(results[0]->getExplicitArgs()[0].type->isEqualTo(Registry::GetType<i32>()));
    REQUIRE(results[1]->getExplicitArgs()[0].type->isEqualTo(Registry::GetType<i32>()));
    REQUIRE(results[0] != results[1]);
    REQUIRE(results[2]->getExplicitArgs()[0].type->isEqualTo(Registry::GetType<i64>()));

    // convertible arg type, strict
    singleStrict = nullptr;
    results = tp->findMethods(
        FuncMatch("meth")
            .argTps({ Registry::GetType<i16>() }, true),
        &singleStrict
    );
    REQUIRE(singleStrict == nullptr);
    REQUIRE(results.size() == 0);

    // no args
    singleStrict = nullptr;
    results = tp->findMethods(
        FuncMatch("meth")
            .noArgs(),
        &singleStrict
    );
    REQUIRE(singleStrict != nullptr);
    REQUIRE(results.size() == 1);
    REQUIRE(singleStrict == results[0]);
    
    // no args, matching return type
    singleStrict = nullptr;
    results = tp->findMethods(
        FuncMatch("meth")
            .retTp(Registry::GetType<void>())
            .noArgs(),
        &singleStrict
    );
    REQUIRE(singleStrict != nullptr);
    REQUIRE(results.size() == 1);
    REQUIRE(singleStrict == results[0]);
    
    // no args, no matching return type
    singleStrict = nullptr;
    results = tp->findMethods(
        FuncMatch("meth")
            .retTp(Registry::GetType<i32>())
            .noArgs(),
        &singleStrict
    );
    REQUIRE(singleStrict == nullptr);
    REQUIRE(results.size() == 0);
}

void test_findConstructors() {
    Registry::Reset();
    type<i64>("i64");
    type<i32>("i32");
    type<i16>("i16");
    type<void>("void");
    auto b = type<struct_with_methods>("test");
    b.ctor().accessFlags = 0b0001000; // arbitrary bits
    b.ctor<i32>();
    b.ctor<i64>();
    DataType* tp = b;

    Function* singleStrict;
    Array<Function*> results;
    
    singleStrict = nullptr;
    results = tp->findConstructors({}, false, FullAccessRights, &singleStrict);
    REQUIRE(singleStrict != nullptr);
    REQUIRE(results.size() == 1);
    REQUIRE(singleStrict == results[0]);
    
    singleStrict = nullptr;
    results = tp->findConstructors({}, false, 0b0001000, &singleStrict);
    REQUIRE(singleStrict != nullptr);
    REQUIRE(results.size() == 1);
    REQUIRE(singleStrict == results[0]);
    
    singleStrict = nullptr;
    results = tp->findConstructors({}, false, 0b1110111, &singleStrict);
    REQUIRE(singleStrict == nullptr);
    REQUIRE(results.size() == 0);
    
    singleStrict = nullptr;
    results = tp->findConstructors({ Registry::GetType<i32>() }, false, FullAccessRights, &singleStrict);
    REQUIRE(singleStrict != nullptr);
    REQUIRE(results.size() == 2);
    REQUIRE(results[0]->getExplicitArgs()[0].type->isEqualTo(Registry::GetType<i32>()));
    REQUIRE(results[1]->getExplicitArgs()[0].type->isEqualTo(Registry::GetType<i64>()));
    REQUIRE(singleStrict == results[0]);
}

void test_findConversionOperator() {
    Registry::Reset();
    type<i64>("i64");
    type<i32>("i32");
    type<i16>("i16");
    type<void>("void");
    auto b = type<struct_with_methods>("test");
    b.opCast<i32>().accessFlags = 0b0001000; // arbitrary bits
    b.opCast<i64>();
    DataType* tp = b;

    Function* fn;
    
    fn = tp->findConversionOperator(Registry::GetType<i32>());
    REQUIRE(fn != nullptr);
    REQUIRE(fn->getSignature()->getReturnType()->isEqualTo(Registry::GetType<i32>()));
    
    fn = tp->findConversionOperator(Registry::GetType<i32>(), 0b0001000);
    REQUIRE(fn != nullptr);
    REQUIRE(fn->getSignature()->getReturnType()->isEqualTo(Registry::GetType<i32>()));
    
    fn = tp->findConversionOperator(Registry::GetType<i32>(), 0b1110111);
    REQUIRE(fn == nullptr);
    
    fn = tp->findConversionOperator(Registry::GetType<i64>());
    REQUIRE(fn != nullptr);
    REQUIRE(fn->getSignature()->getReturnType()->isEqualTo(Registry::GetType<i64>()));
    
    fn = tp->findConversionOperator(Registry::GetType<i64>(), 0b0001000);
    REQUIRE(fn != nullptr);
    REQUIRE(fn->getSignature()->getReturnType()->isEqualTo(Registry::GetType<i64>()));
    
    fn = tp->findConversionOperator(Registry::GetType<i64>(), 0b1110111);
    REQUIRE(fn != nullptr);
    REQUIRE(fn->getSignature()->getReturnType()->isEqualTo(Registry::GetType<i64>()));
}

TEST_CASE("Test DataType methods", "[bind]") {
    SECTION("Common") { testCommonMethods(); }
    SECTION("Primitives") { testMethodsOnPrimitives(); }
    SECTION("Pointers") { testMethodsOnPointers(); }
    SECTION("Objects") { testMethodsOnObjects(); }

    SECTION("findProperties") { test_findProperties(); }
    SECTION("findMethods") { test_findMethods(); }
    SECTION("findConstructors") { test_findConstructors(); }
    SECTION("findConversionOperator") { test_findConversionOperator(); }
}