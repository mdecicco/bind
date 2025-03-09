#include <bind/AliasType.h>
#include <utils/Array.hpp>
#include <string.h>

namespace bind {
    type_meta alias(const type_meta& m) {
        type_meta result = m;
        result.is_alias = 1;
        return result;
    }

    AliasType::AliasType(const String& name, DataType* aliasOf, Namespace* ns)
        : DataType(name, alias(aliasOf->getInfo()), ns), m_aliasOf(aliasOf), m_baseType(aliasOf)
    {
        if (m_aliasOf->getInfo().is_alias) {
            m_baseType = ((AliasType*)m_aliasOf)->m_baseType;
        }
    }

    AliasType::~AliasType() {
    }

    DataType* AliasType::getAliasedType() const {
        return m_aliasOf;
    }

    DataType* AliasType::getBaseType() const {
        return m_baseType;
    }
};