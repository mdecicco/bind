#pragma once
#include <bind/AliasType.h>
#include <bind/DataType.h>
#include <bind/Function.h>
#include <bind/FunctionType.h>
#include <bind/Namespace.hpp>
#include <bind/PointerType.h>
#include <bind/Registry.hpp>
#include <bind/ValuePointer.h>
#include <bind/util/EnumTypeBuilder.hpp>
#include <bind/util/ObjectTypeBuilder.hpp>
#include <bind/util/PrimitiveTypeBuilder.hpp>
#include <bind/util/meta.hpp>
#include <utils/Array.hpp>
#include <utils/Exception.h>
#include <utils/interfaces/IWithUserData.hpp>

namespace bind {
    template <typename T>
    std::enable_if_t<is_type_valid_opaque_v<T>, DataType*> opaqueType(const String& name);

    template <typename T>
    std::enable_if_t<std::is_class_v<T>, ObjectTypeBuilder<T>> type(const String& name);

    template <typename T>
    std::enable_if_t<std::is_class_v<T>, ObjectTypeBuilder<T>> extend();

    template <typename T>
    std::enable_if_t<std::is_fundamental_v<T>, PrimitiveTypeBuilder<T>> type(const String& name);

    template <typename T>
    std::enable_if_t<std::is_fundamental_v<T>, PrimitiveTypeBuilder<T>> extend();

    template <typename T>
    std::enable_if_t<std::is_enum_v<T>, EnumTypeBuilder<T>> type(const String& name);

    template <typename T>
    std::enable_if_t<std::is_enum_v<T>, EnumTypeBuilder<T>> extend();

    template <typename T>
    ValuePointer* global(const String& name, T* val);

    template <typename Ret, typename... Args>
    Function* function(const String& name, Ret (*fn)(Args...));

    template <typename T>
    AliasType* alias(const String& name);

    AliasType* alias(const String& name, DataType* aliasOf);
};

#include <bind/bind.hpp>