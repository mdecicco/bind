#include <bind/EnumType.h>
#include <bind/util/meta.hpp>
#include <utils/Array.hpp>
#include <utils/Exception.h>

namespace bind {
    type_meta enum_meta(const type_meta& in) {
        if (!in.is_primitive || !in.is_integral) {
            throw InputException("EnumType - Should be a primitive / integral type");
        }

        type_meta m = in;
        m.is_enum   = 1;
        return m;
    }

    EnumType::EnumType(const String& name, const type_meta& meta, Namespace* ns)
        : DataType(name, enum_meta(meta), ns) {}

    EnumType::~EnumType() {}

    const Array<EnumType::Field>& EnumType::getFields() const {
        return m_fields;
    }

    const EnumType::Field* EnumType::getField(const String& name) const {
        auto it = m_fieldIndexMap.find(name);
        if (it == m_fieldIndexMap.end()) {
            return nullptr;
        }

        return &m_fields[it->second];
    }
};