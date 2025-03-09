#pragma once
#include <bind/DataType.h>

namespace bind {
    class AliasType : public DataType {
        public:
            AliasType(const String& name, DataType* aliasOf, Namespace* ns);
            ~AliasType();

            DataType* getAliasedType() const;
            DataType* getBaseType() const;
        
        protected:
            DataType* m_aliasOf;
            DataType* m_baseType;
    };
};