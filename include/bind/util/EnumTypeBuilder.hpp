#pragma once
#include <bind/util/PrimitiveTypeBuilder.hpp>
#include <bind/EnumType.h>

namespace bind {
    template <typename Prim>
    class EnumTypeBuilder : public PrimitiveTypeBuilder<Prim> {
        public:
            EnumTypeBuilder(const String& name, Namespace* ns)
                : PrimitiveTypeBuilder<Prim>(new EnumType(name, meta<std::underlying_type_t<Prim>>(), ns))
            {
                // I don't know why `this->` is required here... For some reason it can't
                // find the identifier otherwise.
                Registry::Add(this->m_type, type_hash<Prim>());
            }

            EnumTypeBuilder(EnumType* extend) : PrimitiveTypeBuilder<Prim>(extend) { }

            EnumType::Field& addEnumValue(const String& name, Prim value) {
                if constexpr (std::is_unsigned_v<std::underlying_type_t<Prim>>) {
                    // I don't know why `this->` is required here... For some reason it can't
                    // find the identifier otherwise.
                    return this->_addEnumValue(name, u64(value));
                } else {
                    // I don't know why `this->` is required here... For some reason it can't
                    // find the identifier otherwise.
                    return this->_addEnumValue(name, i64(value));
                }
            }
    };
};