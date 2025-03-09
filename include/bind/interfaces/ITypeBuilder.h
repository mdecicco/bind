#pragma once
#include <bind/types.h>
#include <bind/DataType.h>
#include <bind/EnumType.h>
#include <utils/String.h>

namespace bind {
    class Namespace;
    
    class ITypeBuilder {
        public:
            ITypeBuilder(const String& name, const type_meta& meta, Namespace* ns, size_t nativeHash);
            ITypeBuilder(DataType* extend);
            virtual ~ITypeBuilder();

            DataType::Property& addProperty(
                i32 offset,
                DataType::Property::Flags flags,
                DataType* type,
                String name
            );

            DataType::Property& addProperty(
                const Pointer& address,
                DataType::Property::Flags flags,
                DataType* type,
                String name
            );

            operator DataType*() const;
            DataType* getType() const;
            type_meta& getMeta() const;

        protected:
            EnumType::Field& _addEnumValue(const String& name, u64 value);
            EnumType::Field& _addEnumValue(const String& name, i64 value);
            void updateFFI();

            DataType* m_type;
    };
};