#include <bind/ValuePointer.h>
#include <bind/Namespace.h>
#include <bind/Registry.h>

namespace bind {
    ValuePointer::ValuePointer(const String& name, DataType* type, void* address, Namespace* ns)
        : ISymbol(name, ISymbol::genFullSymbolName(ns, name), ISymbol::genValueSymbolName(ns, name), SymbolType::Value),
          m_type(type), m_address(address)
    {
        if (ns) ns->add(this);
        else Registry::GlobalNamespace()->add(this);
    }

    ValuePointer::~ValuePointer() {
    }

    DataType* ValuePointer::getType() const {
        return m_type;
    }

    void* ValuePointer::getAddress() const {
        return m_address;
    }
};