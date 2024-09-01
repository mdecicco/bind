#include <bind/FunctionType.h>
#include <utils/Exception.h>
#include <utils/Array.hpp>

namespace bind {
    type_meta func_meta(const type_meta& m) {
        type_meta ret = m;
        ret.is_function = 1;
        return ret;
    }

    FunctionType::Argument::Argument(u8 _index, DataType* _type)
        : index(_index), type(_type)
    {
    }

    FunctionType::FunctionType(const String& name, const type_meta& meta)
        : DataType(name, func_meta(meta), nullptr), m_returnType(nullptr), m_thisType(nullptr)
    {
    }

    FunctionType::~FunctionType() {
    }

    const Array<FunctionType::Argument>& FunctionType::getArgs() const {
        return m_args;
    }

    DataType* FunctionType::getReturnType() const {
        return m_returnType;
    }

    DataType* FunctionType::getThisType() const {
        return m_thisType;
    }
    
    const Pointer& FunctionType::getWrapperAddress() const {
        return m_wrapperAddress;
    }
    
    ffi_cif* FunctionType::getCif() {
        return &m_cif;
    }

    void FunctionType::call(const Pointer& funcPtr, void* retDest, void** args) {
        ffi_call(&m_cif, (void(*)())funcPtr.get(), retDest, args);
    }

    void FunctionType::initCallInterface(ffi_abi abi) {
        m_ffiArgTypes.reserve(m_args.size() + 1);
        if (m_thisType) {
            // actually calling a method wrapper, not the method directly
            m_ffiArgTypes.push(&ffi_type_pointer); // function pointer
            m_ffiArgTypes.push(&ffi_type_pointer); // 'this' pointer
        }
        m_ffiArgTypes.append(m_args.map([](const Argument& a) { return a.type->getFFI(); }));

        if (ffi_prep_cif(&m_cif, abi, m_ffiArgTypes.size(), m_returnType->getFFI(), m_ffiArgTypes.data()) != FFI_OK) {
            throw Exception("FunctionType::initCallInterface - Failed to prep FFI call interface");
        }
    }
};