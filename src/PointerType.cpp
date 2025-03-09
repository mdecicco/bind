#include <bind/PointerType.h>
#include <bind/util/meta.hpp>

namespace bind {
    PointerType::PointerType(DataType* pointsTo)
        : DataType(pointsTo->getName() + "*", pointsTo->getFullName() + "*", meta<void*>(), nullptr), m_destTp(pointsTo), m_baseType(pointsTo)
    {
    }

    PointerType::~PointerType() {
    }

    DataType* PointerType::getDestinationType() const {
        return m_destTp;
    }
};