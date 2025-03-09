#include "Common.h"

struct method_test {
    i32 a, b, c;

    void set(i32 _a, i32 _b, i32 _c) {
        a = _a;
        b = _b;
        c = _c;
    }

    void setFromValue(method_test val) {
        a = val.a;
        b = val.b;
        c = val.c;
    }

    void setFromRef(method_test& val) {
        a = val.a;
        b = val.b;
        c = val.c;
    }

    i32 getA() {
        return a;
    }

    plain_struct toStructValue() {
        return { a * 3, b * 3, c * 3 };
    }

    method_test& toStructRef() {
        return *this;
    }
};

plain_struct ps;
method_test mt;

void resetTestData() {
    ps.a = ps.b = ps.c = 0;
    mt.a = mt.b = mt.c = 0;
}

void testPrimitiveArguments() {
    Registry::Reset();

    type<void>("void");
    type<i32>("i32");
    
    auto mb = type<method_test>("method_test");
    mb.method("realMethod", &method_test::set);
    mb.pseudoMethod("pseudoMethod", +[](method_test* self, i32 a, i32 b, i32 c) {
        self->a = a;
        self->b = b;
        self->c = c;
    });

    DataType* mtp = mb;

    SECTION("Normal functions") {
        resetTestData();

        Function* fn = function("func", +[](i32 a, i32 b, i32 c) {
            ps.a = a;
            ps.b = b;
            ps.c = c;
        });
        
        i32 a = 1, b = 2, c = 3;
        void* args[] = { &a, &b, &c };
        fn->call(nullptr, args);

        REQUIRE(ps.a == 1);
        REQUIRE(ps.b == 2);
        REQUIRE(ps.c == 3);
    }

    SECTION("Object methods") {
        resetTestData();

        Function* fn = nullptr;
        mtp->findMethods(FuncMatch("realMethod"), &fn);
        REQUIRE(fn != nullptr);
        
        i32 a = 1, b = 2, c = 3;
        method_test* self = &mt;
        void* args[] = { &self, &a, &b, &c };
        fn->call(nullptr, args);

        REQUIRE(mt.a == 1);
        REQUIRE(mt.b == 2);
        REQUIRE(mt.c == 3);
    }

    SECTION("Pseudo methods") {
        resetTestData();

        Function* fn = nullptr;
        mtp->findMethods(FuncMatch("pseudoMethod"), &fn);
        REQUIRE(fn != nullptr);
        
        i32 a = 1, b = 2, c = 3;
        method_test* self = &mt;
        void* args[] = { &self, &a, &b, &c };
        fn->call(nullptr, args);

        REQUIRE(mt.a == 1);
        REQUIRE(mt.b == 2);
        REQUIRE(mt.c == 3);
    }
}

void testObjectByValArguments() {
    Registry::Reset();

    type<void>("void");
    type<i32>("i32");
    auto pb = type<plain_struct>("plain_struct");
    pb.prop("a", &plain_struct::a);
    // b purposefully omitted
    pb.prop("c", &plain_struct::c);
    
    auto mb = type<method_test>("method_test");
    mb.method("realMethod", &method_test::setFromValue);
    mb.pseudoMethod("pseudoMethod", +[](method_test* self, method_test val) {
        self->a = val.a;
        self->b = val.b;
        self->c = val.c;
    });

    DataType* mtp = mb;

    SECTION("Normal functions") {
        resetTestData();

        plain_struct s = { 1, 2, 3 };

        Function* fn = function("func", +[](plain_struct val) {
            ps.a = val.a;
            ps.b = val.b;
            ps.c = val.c;
        });
        
        void* args[] = { &s };
        fn->call(nullptr, args);

        REQUIRE(ps.a == 1);
        REQUIRE(ps.b == 2);
        REQUIRE(ps.c == 3);
    }

    SECTION("Object methods") {
        resetTestData();

        Function* fn = nullptr;
        mtp->findMethods(FuncMatch("realMethod"), &fn);
        REQUIRE(fn != nullptr);
        
        method_test arg = { 1, 2, 3 };
        method_test* self = &mt;
        void* args[] = { &self, &arg };
        fn->call(nullptr, args);

        REQUIRE(mt.a == 1);
        REQUIRE(mt.b == 2);
        REQUIRE(mt.c == 3);
    }

    SECTION("Pseudo methods") {
        resetTestData();

        Function* fn = nullptr;
        mtp->findMethods(FuncMatch("pseudoMethod"), &fn);
        REQUIRE(fn != nullptr);
        
        method_test arg = { 1, 2, 3 };
        method_test* self = &mt;
        void* args[] = { &self, &arg };
        fn->call(nullptr, args);

        REQUIRE(mt.a == 1);
        REQUIRE(mt.b == 2);
        REQUIRE(mt.c == 3);
    }
}

void testObjectByRefArguments() {
    Registry::Reset();

    type<void>("void");
    type<i32>("i32");
    auto pb = type<plain_struct>("plain_struct");
    pb.prop("a", &plain_struct::a);
    // b purposefully omitted
    pb.prop("c", &plain_struct::c);
    
    auto mb = type<method_test>("method_test");
    mb.method("realMethod", &method_test::setFromRef);
    mb.pseudoMethod("pseudoMethod", +[](method_test* self, method_test& val) {
        self->a = val.a;
        self->b = val.b;
        self->c = val.c;
    });

    DataType* mtp = mb;

    SECTION("Normal functions") {
        resetTestData();

        plain_struct s = { 1, 2, 3 };

        Function* fn = function("func", +[](plain_struct& val) {
            ps.a = val.a;
            ps.b = val.b;
            ps.c = val.c;
        });
        
        plain_struct* sp = &s;
        void* args[] = { &sp };
        fn->call(nullptr, args);

        REQUIRE(ps.a == 1);
        REQUIRE(ps.b == 2);
        REQUIRE(ps.c == 3);
    }

    SECTION("Object methods") {
        resetTestData();

        Function* fn = nullptr;
        mtp->findMethods(FuncMatch("realMethod"), &fn);
        REQUIRE(fn != nullptr);
        
        method_test arg = { 1, 2, 3 };
        method_test* argp = &arg;
        method_test* self = &mt;
        void* args[] = { &self, &argp };
        fn->call(nullptr, args);

        REQUIRE(mt.a == 1);
        REQUIRE(mt.b == 2);
        REQUIRE(mt.c == 3);
    }

    SECTION("Pseudo methods") {
        resetTestData();

        Function* fn = nullptr;
        mtp->findMethods(FuncMatch("pseudoMethod"), &fn);
        REQUIRE(fn != nullptr);
        
        method_test arg = { 1, 2, 3 };
        method_test* argp = &arg;
        method_test* self = &mt;
        void* args[] = { &self, &argp };
        fn->call(nullptr, args);

        REQUIRE(mt.a == 1);
        REQUIRE(mt.b == 2);
        REQUIRE(mt.c == 3);
    }
}

void testPrimitiveReturnValue() {
    Registry::Reset();

    type<void>("void");
    type<i32>("i32");
    
    auto mb = type<method_test>("method_test");
    mb.method("realMethod", &method_test::getA);
    mb.pseudoMethod("pseudoMethod", +[](method_test* self) {
        return self->a;
    });

    DataType* mtp = mb;

    SECTION("Normal functions") {
        resetTestData();

        Function* fn = function("func", +[](i32 a, i32 b, i32 c) {
            return a + b + c;
        });
        
        i32 a = 1, b = 2, c = 3;
        i32 result = 0;
        void* args[] = { &a, &b, &c };
        fn->call(&result, args);
        REQUIRE(result == 6);
    }

    SECTION("Object methods") {
        resetTestData();

        Function* fn = nullptr;
        mtp->findMethods(FuncMatch("realMethod"), &fn);
        REQUIRE(fn != nullptr);
        
        method_test* self = &mt;
        void* args[] = { &self };
        i32 result = 0;
        mt.a = 3;
        fn->call(&result, args);

        REQUIRE(result == 3);
    }

    SECTION("Pseudo methods") {
        resetTestData();

        Function* fn = nullptr;
        mtp->findMethods(FuncMatch("pseudoMethod"), &fn);
        REQUIRE(fn != nullptr);
        
        method_test* self = &mt;
        mt.a = 9;
        void* args[] = { &self };
        i32 result = 0;
        fn->call(&result, args);

        REQUIRE(result == 9);
    }
}

void testObjectReturnValue() {
    Registry::Reset();

    type<void>("void");
    type<i32>("i32");
    auto pb = type<plain_struct>("plain_struct");
    pb.prop("a", &plain_struct::a);
    // b purposefully omitted
    pb.prop("c", &plain_struct::c);
    
    auto mb = type<method_test>("method_test");
    mb.method("realMethod", &method_test::toStructValue);
    mb.pseudoMethod("pseudoMethod", +[](method_test* self, plain_struct& v) {
        return plain_struct({ self->a + v.a, self->b + v.b, self->c + v.c });
    });

    DataType* mtp = mb;

    SECTION("Normal functions") {
        resetTestData();

        plain_struct s = { 1, 2, 3 };

        Function* fn = function("func", +[](plain_struct val) {
            return plain_struct({ val.a * 2, val.b * 2, val.c * 2 });
        });
        
        void* args[] = { &s };
        plain_struct result = { 0, 0, 0 };
        fn->call(&result, args);

        REQUIRE(result.a == 2);
        REQUIRE(result.b == 4);
        REQUIRE(result.c == 6);
    }

    SECTION("Object methods") {
        resetTestData();

        Function* fn = nullptr;
        mtp->findMethods(FuncMatch("realMethod"), &fn);
        REQUIRE(fn != nullptr);
        
        mt.a = 1;
        mt.b = 2;
        mt.c = 3;

        method_test arg = { 1, 2, 3 };
        method_test* self = &mt;
        void* args[] = { &self, &arg };
        plain_struct result = { 0, 0, 0 };
        fn->call(&result, args);

        REQUIRE(result.a == 3);
        REQUIRE(result.b == 6);
        REQUIRE(result.c == 9);
    }

    SECTION("Pseudo methods") {
        resetTestData();

        Function* fn = nullptr;
        mtp->findMethods(FuncMatch("pseudoMethod"), &fn);
        REQUIRE(fn != nullptr);

        mt.a = 1;
        mt.b = 2;
        mt.c = 3;
        
        method_test arg = { 5, 4, 3 };
        method_test* self = &mt;
        method_test* argp = &arg;
        void* args[] = { &self, &argp };
        plain_struct result = { 0, 0, 0 };
        fn->call(&result, args);

        REQUIRE(result.a == 6);
        REQUIRE(result.b == 6);
        REQUIRE(result.c == 6);
    }
}

void testPointerReturnValue() {
    Registry::Reset();

    type<void>("void");
    type<i32>("i32");
    auto pb = type<plain_struct>("plain_struct");
    pb.prop("a", &plain_struct::a);
    // b purposefully omitted
    pb.prop("c", &plain_struct::c);
    
    auto mb = type<method_test>("method_test");
    mb.method("realMethod", &method_test::toStructRef);
    mb.pseudoMethod("pseudoMethod", +[](method_test* self) {
        return self;
    });

    DataType* mtp = mb;

    SECTION("Normal functions") {
        resetTestData();

        plain_struct s = { 1, 2, 3 };

        Function* fn = function("func", +[](plain_struct& val) -> plain_struct& {
            return val;
        });
        
        plain_struct* sp = &s;
        plain_struct* rv = nullptr;
        void* args[] = { &sp };
        fn->call(&rv, args);

        REQUIRE(rv == sp);
        REQUIRE(s.a == 1);
        REQUIRE(s.b == 2);
        REQUIRE(s.c == 3);
    }

    SECTION("Object methods") {
        resetTestData();

        Function* fn = nullptr;
        mtp->findMethods(FuncMatch("realMethod"), &fn);
        REQUIRE(fn != nullptr);
        
        method_test* self = &mt;
        method_test* rv = nullptr;
        void* args[] = { &self };
        fn->call(&rv, args);

        REQUIRE(rv == self);
        REQUIRE(mt.a == 0);
        REQUIRE(mt.b == 0);
        REQUIRE(mt.c == 0);
    }

    SECTION("Pseudo methods") {
        resetTestData();

        Function* fn = nullptr;
        mtp->findMethods(FuncMatch("pseudoMethod"), &fn);
        REQUIRE(fn != nullptr);
        
        method_test* self = &mt;
        method_test* rv = nullptr;
        void* args[] = { &self };
        fn->call(&rv, args);

        REQUIRE(rv == self);
        REQUIRE(mt.a == 0);
        REQUIRE(mt.b == 0);
        REQUIRE(mt.c == 0);
    }
}

TEST_CASE("Test that bound functions are called properly", "[bind]") {
    SECTION("Primitive arguments are passed successfully") { testPrimitiveArguments(); }
    SECTION("Object by-value arguments are passed successfully") { testObjectByValArguments(); }
    SECTION("Object by-ref/ptr arguments are passed successfully") { testObjectByRefArguments(); }
    SECTION("Primitive return values are passed successfully") { testPrimitiveReturnValue(); }
    SECTION("Object return values are passed successfully") { testObjectReturnValue(); }
    SECTION("Pointer return values are passed successfully") { testPointerReturnValue(); }
}