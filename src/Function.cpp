#include <bind/Function.h>
#include <bind/interfaces/ICallHandler.h>
#include <bind/Namespace.h>
#include <bind/Registry.h>
#include <utils/Exception.h>
#include <utils/Array.hpp>

namespace bind {
    Function::Function(const String& name, FunctionType* sig, Namespace* ns)
        : ISymbol(name, ISymbol::genFullSymbolName(ns, name), ISymbol::genFuncSymbolName(ns, name, sig), SymbolType::Function),
          m_signature(sig), m_callHandler(nullptr), m_implicitArgCount(0)
    {
        if (ns) ns->add(this);
        else Registry::GlobalNamespace()->add(this);
    }
    Function::Function(const String& name, const Pointer& address, FunctionType* sig, Namespace* ns) 
        : ISymbol(name, ISymbol::genFullSymbolName(ns, name), ISymbol::genFuncSymbolName(ns, name, sig), SymbolType::Function),
          m_signature(sig), m_callHandler(nullptr), m_implicitArgCount(0), m_address(address)
    {
        if (ns) ns->add(this);
        else Registry::GlobalNamespace()->add(this);
    }

    Function::~Function() {
        if (m_callHandler) delete m_callHandler;
        m_callHandler = nullptr;
    }

    void Function::setCallHandler(ICallHandler* callHandler) {
        if (m_callHandler) delete m_callHandler;
        m_callHandler = callHandler;
    }
    
    void Function::call(void* retDest, void** args) {
        if (!m_callHandler) {
            throw Exception("Function::call - function '%s' has no call handler", getFullName().c_str());
        }

        m_callHandler->call(retDest, args);
    }

    FunctionType* Function::getSignature() const {
        return m_signature;
    }

    ConstArrayView<FunctionType::Argument> Function::getExplicitArgs() const {
        return m_signature->getArgs().view(m_implicitArgCount);
    }

    ICallHandler* Function::getCallHandler() const {
        return m_callHandler;
    }

    const Pointer& Function::getAddress() const {
        return m_address;
    }
}