#include <bind/AliasType.h>
#include <bind/Namespace.h>
#include <bind/Registry.h>
#include <utils/Array.hpp>

namespace bind {
    Namespace::Namespace(const String& name)
        : ISymbol(
              name,
              ISymbol::genFullSymbolName(Registry::GlobalNamespace(), name),
              ISymbol::genNamespaceSymbolName(Registry::GlobalNamespace(), name),
              SymbolType::Namespace
          ),
          m_parent(Registry::GlobalNamespace()),
          m_forType(nullptr) {
        if (m_parent) {
            m_parent->add(this);
        }
    }

    Namespace::Namespace(Namespace* parent, const String& name)
        : ISymbol(
              name,
              ISymbol::genFullSymbolName(parent, name),
              ISymbol::genNamespaceSymbolName(parent, name),
              SymbolType::Namespace
          ),
          m_parent(parent ? parent : Registry::GlobalNamespace()),
          m_forType(nullptr) {
        if (m_parent) {
            m_parent->add(this);
        }
    }

    Namespace::Namespace(DataType* type)
        : ISymbol(
              type->getName(),
              ISymbol::genFullSymbolName(Registry::GlobalNamespace(), type->getName()),
              ISymbol::genTypeSymbolName(Registry::GlobalNamespace(), type->getName()),
              SymbolType::Namespace
          ),
          m_parent(Registry::GlobalNamespace()),
          m_forType(type) {
        if (m_parent) {
            m_parent->add(this);
        }
    }

    Namespace::Namespace(Namespace* parent, DataType* type)
        : ISymbol(
              type->getName(),
              ISymbol::genFullSymbolName(parent, type->getName()),
              ISymbol::genTypeSymbolName(parent, type->getName()),
              SymbolType::Namespace
          ),
          m_parent(parent ? parent : Registry::GlobalNamespace()),
          m_forType(type) {
        if (m_parent) {
            m_parent->add(this);
        }
    }

    AliasType* Namespace::alias(const String& name, DataType* aliasOf) {
        AliasType* tp = new AliasType(name, aliasOf, this);
        Registry::Add(tp);
        return tp;
    }

    Namespace* Namespace::getParent() const {
        return m_parent;
    }

    DataType* Namespace::getCorrespondingType() const {
        return m_forType;
    }

    ISymbol* Namespace::findSymbol(const String& symbolName) const {
        auto it = m_symbolMap.find(ISymbol::genSymbolID(symbolName));
        if (it != m_symbolMap.end()) {
            return it->second;
        }

        return nullptr;
    }

    void Namespace::findSymbols(const String& name, Array<ISymbol*>& outResults) const {
        for (ISymbol* sym : m_symbols) {
            if (sym->getName() == name) {
                outResults.push(sym);
            }
        }
    }

    const Array<ISymbol*>& Namespace::getSymbols() const {
        return m_symbols;
    }

    void Namespace::add(ISymbol* sym) {
        if (sym->getSymbolType() == SymbolType::Namespace) {
            Namespace* ns = (Namespace*)sym;
            if (ns->getCorrespondingType()) {
                // type should be added separately
                return;
            }
        }

        if (sym->m_namespace) {
            if (sym->m_namespace == this) {
                throw InputException(
                    String::Format("Namespace::add - Symbol '%s' has already been added", sym->getName().c_str())
                );
            }

            throw InputException(String::Format(
                "Namespace::add - Symbol '%s' has already been added to another namespace", sym->getName().c_str()
            ));
        }

        auto it = m_symbolMap.find(sym->getSymbolId());
        if (it != m_symbolMap.end()) {
            throw InputException(
                String::Format("Namespace::add - Symbol '%s' has already been added", sym->getName().c_str())
            );
        };

        m_symbolMap.insert(std::pair(sym->getSymbolId(), sym));
        m_symbols.push(sym);

        sym->m_namespace = this;
    }

    void Namespace::remove(ISymbol* sym) {
        if (sym->m_namespace != this) {
            return;
        }

        auto it = m_symbolMap.find(sym->getSymbolId());
        if (it == m_symbolMap.end()) {
            return;
        }

        m_symbolMap.erase(it);
        for (u32 i = 0; i < m_symbols.size(); i++) {
            if (m_symbols[i] == sym) {
                m_symbols.remove(i);
                break;
            }
        }

        sym->m_namespace = nullptr;
    }
};