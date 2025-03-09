#pragma once
#include <bind/DataType.h>
#include <unordered_map>

namespace bind {
    class EnumType : public DataType {
        public:
            struct Field {
                String name;
                union {
                    u64 u;
                    i64 i;
                } value;
            };

            EnumType(const String& name, const type_meta& meta, Namespace* ns);
            ~EnumType();

            const Array<Field>& getFields() const;
            const Field* getField(const String& name) const;
        
        protected:
            friend class ITypeBuilder;
            
            Array<Field> m_fields;
            std::unordered_map<String, u32> m_fieldIndexMap;
    };
};