#include <bind/DataType.h>
#include <bind/Function.h>
#include <bind/Namespace.h>
#include <bind/Registry.h>
#include <bind/ValuePointer.h>
#include <mutex>
#include <utils/Array.hpp>
#include <utils/Exception.h>

namespace bind {
    Registry* Registry::instance = nullptr;

    Registry::Registry() {
        m_global = nullptr;
    }

    Registry::~Registry() {
        std::unique_lock l(instance->m_mutex);

        for (auto& it : instance->m_valueMap) {
            delete it.second;
        }
        for (auto& it : instance->m_funcMap) {
            delete it.second;
        }
        for (auto& it : instance->m_typeMap) {
            delete it.second;
        }
        for (auto& it : instance->m_namespaceMap) {
            delete it.second;
        }

        m_global = nullptr;
    }

    void Registry::Create() {
        if (instance) {
            return;
        }
        instance           = new Registry();
        instance->m_global = new Namespace("");

        Registry::Add(instance->m_global);
    }

    void Registry::Reset() {
        if (!instance) {
            throw InvalidActionException("Registry::Reset - Registry has not been created");
        }
        Registry::Destroy();
        Registry::Create();
    }

    void Registry::Destroy() {
        if (!instance) {
            return;
        }
        delete instance;
        instance = nullptr;
    }

    void Registry::Add(DataType* tp) {
        if (!instance) {
            throw InvalidActionException("Registry::Add - Registry has not been created");
        }

        std::unique_lock l(instance->m_mutex);
        auto it = instance->m_typeMap.find(tp->getSymbolId());
        if (it != instance->m_typeMap.end()) {
            throw InputException(
                String::Format("Registry::Add - Type '%s' already registered", tp->getFullName().c_str())
            );
        }

        instance->m_typeMap.insert(std::pair<symbol_id, DataType*>(tp->getSymbolId(), tp));
    }

    void Registry::Add(DataType* tp, size_t nativeHash) {
        if (!instance) {
            throw InvalidActionException("Registry::Add - Registry has not been created");
        }

        std::unique_lock l(instance->m_mutex);
        auto it = instance->m_typeMap.find(tp->getSymbolId());
        if (it != instance->m_typeMap.end()) {
            throw InputException(
                String::Format("Registry::Add - Type '%s' already registered", tp->getFullName().c_str())
            );
        }

        instance->m_typeMap.insert(std::pair<symbol_id, DataType*>(tp->getSymbolId(), tp));
        instance->m_hostTypeMap.insert(std::pair<size_t, DataType*>(nativeHash, tp));
    }

    void Registry::Add(Function* fn) {
        if (!instance) {
            throw InvalidActionException("Registry::Add - Registry has not been created");
        }

        std::unique_lock l(instance->m_mutex);
        auto it = instance->m_funcMap.find(fn->getSymbolId());
        if (it != instance->m_funcMap.end()) {
            throw InputException(
                String::Format("Registry::Add - Function '%s' already registered", fn->getFullName().c_str())
            );
        }

        instance->m_funcMap.insert(std::pair<symbol_id, Function*>(fn->getSymbolId(), fn));
    }

    void Registry::Add(ValuePointer* val) {
        if (!instance) {
            throw InvalidActionException("Registry::Add - Registry has not been created");
        }

        std::unique_lock l(instance->m_mutex);
        auto it = instance->m_valueMap.find(val->getSymbolId());
        if (it != instance->m_valueMap.end()) {
            throw InputException(
                String::Format("Registry::Add - Value '%s' already registered", val->getFullName().c_str())
            );
        }

        instance->m_valueMap.insert(std::pair<symbol_id, ValuePointer*>(val->getSymbolId(), val));
    }

    void Registry::Add(Namespace* ns) {
        if (!instance) {
            throw InvalidActionException("Registry::Add - Registry has not been created");
        }

        std::unique_lock l(instance->m_mutex);
        auto it = instance->m_namespaceMap.find(ns->getSymbolId());
        if (it != instance->m_namespaceMap.end()) {
            throw InputException(
                String::Format("Registry::Add - Namespace '%s' already registered", ns->getFullName().c_str())
            );
        }

        instance->m_namespaceMap.insert(std::pair<symbol_id, Namespace*>(ns->getSymbolId(), ns));
    }

    void Registry::Remove(DataType* tp) {
        if (!instance) {
            throw InvalidActionException("Registry::Remove - Registry has not been created");
        }

        Remove(tp->getOwnNamespace());

        std::unique_lock l(instance->m_mutex);

        auto it = instance->m_typeMap.find(tp->getSymbolId());
        if (it != instance->m_typeMap.end()) {
            instance->m_typeMap.erase(it);
        }

        for (auto n = instance->m_hostTypeMap.begin(); n != instance->m_hostTypeMap.end(); n++) {
            if (n->second == tp) {
                instance->m_hostTypeMap.erase(n);
                break;
            }
        }
    }

    void Registry::Remove(Function* fn) {
        if (!instance) {
            throw InvalidActionException("Registry::Remove - Registry has not been created");
        }

        std::unique_lock l(instance->m_mutex);
        auto it = instance->m_funcMap.find(fn->getSymbolId());
        if (it != instance->m_funcMap.end()) {
            instance->m_funcMap.erase(it);
        }
    }

    void Registry::Remove(ValuePointer* val) {
        if (!instance) {
            throw InvalidActionException("Registry::Remove - Registry has not been created");
        }

        std::unique_lock l(instance->m_mutex);
        auto it = instance->m_valueMap.find(val->getSymbolId());
        if (it != instance->m_valueMap.end()) {
            instance->m_valueMap.erase(it);
        }
    }

    void Registry::Remove(Namespace* ns) {
        if (!instance) {
            throw InvalidActionException("Registry::Remove - Registry has not been created");
        }

        std::unique_lock l(instance->m_mutex);
        auto it = instance->m_namespaceMap.find(ns->getSymbolId());
        if (it != instance->m_namespaceMap.end()) {
            instance->m_namespaceMap.erase(it);
        }
    }

    DataType* Registry::GetType(symbol_id id) {
        if (!instance) {
            throw InvalidActionException("Registry::GetType - Registry has not been created");
        }
        std::shared_lock l(instance->m_mutex);

        auto it = instance->m_typeMap.find(id);
        if (it != instance->m_typeMap.end()) {
            return it->second;
        }

        return nullptr;
    }

    Function* Registry::GetFunc(symbol_id id) {
        if (!instance) {
            throw InvalidActionException("Registry::GetFunc - Registry has not been created");
        }
        std::shared_lock l(instance->m_mutex);

        auto it = instance->m_funcMap.find(id);
        if (it != instance->m_funcMap.end()) {
            return it->second;
        }

        return nullptr;
    }

    ValuePointer* Registry::GetValue(symbol_id id) {
        if (!instance) {
            throw InvalidActionException("Registry::GetValue - Registry has not been created");
        }
        std::shared_lock l(instance->m_mutex);

        auto it = instance->m_valueMap.find(id);
        if (it != instance->m_valueMap.end()) {
            return it->second;
        }

        return nullptr;
    }

    Namespace* Registry::GetNamespace(symbol_id id) {
        if (!instance) {
            throw InvalidActionException("Registry::GetNamespace - Registry has not been created");
        }
        std::shared_lock l(instance->m_mutex);

        auto it = instance->m_namespaceMap.find(id);
        if (it != instance->m_namespaceMap.end()) {
            return it->second;
        }

        return nullptr;
    }

    std::shared_lock<std::shared_mutex> Registry::ReadLock() {
        if (!instance) {
            throw InvalidActionException("Registry::ReadLock - Registry has not been created");
        }
        return std::shared_lock(instance->m_mutex);
    }

    Array<DataType*> Registry::Types() {
        if (!instance) {
            throw InvalidActionException("Registry::Types - Registry has not been created");
        }
        Array<DataType*> ret;

        for (auto& it : instance->m_typeMap) {
            ret.push(it.second);
        }

        return ret;
    }

    Array<Function*> Registry::Functions() {
        if (!instance) {
            throw InvalidActionException("Registry::Functions - Registry has not been created");
        }
        Array<Function*> ret;

        for (auto& it : instance->m_funcMap) {
            ret.push(it.second);
        }

        return ret;
    }

    Array<ValuePointer*> Registry::Values() {
        if (!instance) {
            throw InvalidActionException("Registry::Values - Registry has not been created");
        }
        Array<ValuePointer*> ret;

        for (auto& it : instance->m_valueMap) {
            ret.push(it.second);
        }

        return ret;
    }

    Namespace* Registry::GlobalNamespace() {
        if (!instance) {
            throw InvalidActionException("Registry::GlobalNamespace - Registry has not been created");
        }
        return instance->m_global;
    }

    FunctionType* Registry::Signature(DataType* retTp, const Array<DataType*>& argTps) {
        return Signature(retTp, const_cast<DataType**>(argTps.data()), argTps.size());
    }

    FunctionType* Registry::Signature(DataType* retTp, DataType** argTps, u32 argCount, bool* didExist) {
        String name = retTp->getFullName() + "(";

        for (u8 i = 0; i < argCount; i++) {
            if (i > 0) {
                name += ",";
            }
            name += argTps[i]->getFullName();
        }

        name += ")";

        symbol_id id = ISymbol::genSymbolID(ISymbol::genTypeSymbolName(nullptr, name));
        DataType* tp = GetType(id);
        if (tp) {
            if (didExist) {
                *didExist = true;
            }
            return (FunctionType*)tp;
        }

        if (didExist) {
            *didExist = false;
        }

        FunctionType* sig = new FunctionType(name, meta<void (*)()>());

        for (u8 i = 0; i < argCount; i++) {
            sig->m_args.push(FunctionType::Argument(i, argTps[i]));
        }

        sig->m_returnType = retTp;

        Add(sig);

        sig->initCallInterface(FFI_DEFAULT_ABI);

        return sig;
    }

    FunctionType* Registry::MethodSignature(DataType* retTp, DataType* selfTp, const Array<DataType*>& argTps) {
        return MethodSignature(retTp, selfTp, const_cast<DataType**>(argTps.data()), argTps.size());
    }

    FunctionType* Registry::MethodSignature(
        DataType* retTp, DataType* selfTp, DataType** argTps, u32 argCount, bool* didExist
    ) {
        String name = retTp->getFullName() + " " + selfTp->getFullName() + "::(";
        for (u8 i = 0; i < argCount; i++) {
            if (i > 0) {
                name += ",";
            }
            name += argTps[i]->getFullName();
        }

        name += ")";

        symbol_id id = ISymbol::genSymbolID(ISymbol::genTypeSymbolName(nullptr, name));
        DataType* tp = GetType(id);
        if (tp) {
            if (didExist) {
                *didExist = true;
            }
            return (FunctionType*)tp;
        }

        if (didExist) {
            *didExist = false;
        }

        struct dummy {};

        FunctionType* sig = new FunctionType(name, meta<void (dummy::*)()>());
        for (u8 i = 0; i < argCount; i++) {
            sig->m_args.push(FunctionType::Argument(i, argTps[i]));
        }

        sig->m_returnType = retTp;
        sig->m_thisType   = selfTp;

        Add(sig);

        sig->initCallInterface(FFI_DEFAULT_ABI);

        return sig;
    }
};