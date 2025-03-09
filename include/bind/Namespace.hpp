#pragma once
#include <bind/Namespace.h>

namespace bind {
    template <typename T>
    std::enable_if_t<std::is_class_v<T>, ObjectTypeBuilder<T>>
    Namespace::type(const String& name) {
        DataType* tp = Registry::GetType<T>();
        if (tp) throw Exception(String::Format("Namespace::type - Type '%s' has already been registered", type_name<T>()));

        return ObjectTypeBuilder<T>(name, this);
    }

    template <typename T>
    std::enable_if_t<std::is_class_v<T>, ObjectTypeBuilder<T>>
    Namespace::extend() {
        DataType* tp = Registry::GetType<T>();
        if (!tp) throw Exception(String::Format("Namespace::extend - Type '%s' has not been registered", type_name<T>()));

        return ObjectTypeBuilder<T>(tp);
    }

    template <typename T>
    std::enable_if_t<std::is_fundamental_v<T>, PrimitiveTypeBuilder<T>>
    Namespace::type(const String& name) {
        DataType* tp = Registry::GetType<T>();
        if (tp) throw Exception(String::Format("Namespace::type - Type '%s' has already been registered", type_name<T>()));

        return PrimitiveTypeBuilder<T>(name, this);
    }

    template <typename T>
    std::enable_if_t<std::is_fundamental_v<T>, PrimitiveTypeBuilder<T>>
    Namespace::extend() {
        DataType* tp = Registry::GetType<T>();
        if (!tp) throw Exception(String::Format("Namespace::extend - Type '%s' has not been registered", type_name<T>()));

        return PrimitiveTypeBuilder<T>(tp);
    }

    template <typename T>
    std::enable_if_t<std::is_enum_v<T>, EnumTypeBuilder<T>>
    Namespace::type(const String& name) {
        DataType* tp = Registry::GetType<T>();
        if (tp) throw Exception(String::Format("Namespace::type - Type '%s' has already been registered", type_name<T>()));

        return EnumTypeBuilder<T>(name, this);
    }

    template <typename T>
    std::enable_if_t<std::is_enum_v<T>, EnumTypeBuilder<T>>
    Namespace::extend() {
        DataType* tp = Registry::GetType<T>();
        if (!tp) throw Exception(String::Format("Namespace::extend - Type '%s' has not been registered", type_name<T>()));

        return EnumTypeBuilder<T>((EnumType*)tp);
    }
    
    template <typename T>
    ValuePointer* Namespace::value(const String& name, T* val) {
        DataType* tp = Registry::GetType<T>();
        if (!tp) throw Exception(String::Format("Namespace::value - Type '%s' has not been registered", type_name<T>()));

        ValuePointer* v = new ValuePointer(name, tp, val, this);
        Registry::Add(v);
        return v;
    }

    template <typename Ret, typename... Args>
    Function* Namespace::function(const String& name, Ret (*fn)(Args...)) {
        Function* func = new Function(
            name,
            fn,
            Registry::Signature<Ret, Args...>(),
            this
        );

        func->setCallHandler(new HostCallHandler(func));

        Registry::Add(func);
        return func;
    }

    template <typename T>
    AliasType* Namespace::alias(const String& name) {
        DataType* tp = Registry::GetType<T>();
        if (!tp) throw Exception(String::Format("Namespace::alias - Type '%s' has not been registered", type_name<T>()));

        return Namespace::alias(name, tp);
    }
}