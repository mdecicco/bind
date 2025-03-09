#pragma once
#include <bind/Function.h>
#include <utils/Pointer.hpp>

namespace bind {
    template <typename Cls, typename... Args>
    void __cdecl _constructor_wrapper(Cls* self, Args... args) {
        new (self) Cls (args...);
    }

    template <typename Cls>
    void __cdecl _destructor_wrapper(Cls* self) {
        self->~Cls();
    }

    template <typename Cls, typename Ret, typename... Args>
    Ret __cdecl _method_wrapper(Function* fn, Cls* self, Args... args) {
        typedef Ret (Cls::*MethodTp)(Args...);
        MethodTp method;
        fn->getAddress().get(&method);

        return (self->*method)(args...);
    }
};