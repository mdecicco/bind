#pragma once
#include <bind/bind.h>
#include <bind/interfaces/ICallHandler.h>

namespace bind {
    template <typename T>
    std::enable_if_t<std::is_class_v<T>, ObjectTypeBuilder<T>>
    type(const String& name) {
        DataType* tp = Registry::GetType<T>();
        if (tp) throw Exception(String::Format("type - Type '%s' has already been registered", type_name<T>()));

        return ObjectTypeBuilder<T>(name, Registry::GlobalNamespace());
    }

    template <typename T>
    std::enable_if_t<std::is_class_v<T>, ObjectTypeBuilder<T>>
    extend() {
        DataType* tp = Registry::GetType<T>();
        if (!tp) throw Exception(String::Format("extend - Type '%s' has not been registered", type_name<T>()));

        return ObjectTypeBuilder<T>(tp);
    }

    template <typename T>
    std::enable_if_t<std::is_fundamental_v<T>, PrimitiveTypeBuilder<T>>
    type(const String& name) {
        DataType* tp = Registry::GetType<T>();
        if (tp) throw Exception(String::Format("type - Type '%s' has already been registered", type_name<T>()));

        return PrimitiveTypeBuilder<T>(name, Registry::GlobalNamespace());
    }

    template <typename T>
    std::enable_if_t<std::is_fundamental_v<T>, PrimitiveTypeBuilder<T>>
    extend() {
        DataType* tp = Registry::GetType<T>();
        if (!tp) throw Exception(String::Format("extend - Type '%s' has not been registered", type_name<T>()));

        return PrimitiveTypeBuilder<T>(tp);
    }

    template <typename T>
    std::enable_if_t<std::is_enum_v<T>, EnumTypeBuilder<T>>
    type(const String& name) {
        DataType* tp = Registry::GetType<T>();
        if (tp) throw Exception(String::Format("type - Type '%s' has already been registered", type_name<T>()));

        return EnumTypeBuilder<T>(name, Registry::GlobalNamespace());
    }

    template <typename T>
    std::enable_if_t<std::is_enum_v<T>, EnumTypeBuilder<T>>
    extend() {
        DataType* tp = Registry::GetType<T>();
        if (!tp) throw Exception(String::Format("extend - Type '%s' has not been registered", type_name<T>()));

        return EnumTypeBuilder<T>((EnumType*)tp);
    }

    template <typename T>
    ValuePointer* global(const String& name, T* val) {
        DataType* tp = Registry::GetType<T>();
        if (!tp) throw Exception(String::Format("global - Type '%s' has not been registered", type_name<T>()));

        ValuePointer* v = new ValuePointer(name, tp, val, Registry::GlobalNamespace());
        Registry::Add(v);
        return v;
    }

    template <typename Ret, typename... Args>
    Function* function(const String& name, Ret (*fn)(Args...)) {
        Function* func = new Function(
            name,
            fn,
            Registry::Signature<Ret, Args...>(),
            Registry::GlobalNamespace()
        );

        func->setCallHandler(new HostCallHandler(func));

        Registry::Add(func);
        return func;
    }

    template <typename T>
    AliasType* alias(const String& name) {
        DataType* tp = Registry::GetType<T>();
        if (!tp) throw Exception(String::Format("alias - Type '%s' has not been registered", type_name<T>()));

        return alias(name, tp);
    }
};