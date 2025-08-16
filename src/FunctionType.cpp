#include <bind/FunctionType.h>
#include <utils/Array.hpp>
#include <utils/Exception.h>

namespace bind {
    type_meta func_meta(const type_meta& m) {
        type_meta ret   = m;
        ret.is_function = 1;
        ret.is_pointer  = 1;
        return ret;
    }

    FunctionType::Argument::Argument(u8 _index, DataType* _type) : index(_index), type(_type) {}

    FunctionType::FunctionType(const String& name, const type_meta& meta)
        : DataType(name, func_meta(meta), nullptr), m_returnType(nullptr), m_thisType(nullptr) {}

    FunctionType::~FunctionType() {}

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
        ffi_call(&m_cif, (void (*)())funcPtr.get(), retDest, args);
    }

    void FunctionType::initCallInterface(ffi_abi abi) {
        m_ffiArgTypes = m_args.map([](const Argument& a) {
            return a.type->getFFI();
        });

        if (ffi_prep_cif(&m_cif, abi, m_ffiArgTypes.size(), m_returnType->getFFI(), m_ffiArgTypes.data()) != FFI_OK) {
            throw GenericException("FunctionType::initCallInterface - Failed to prep FFI call interface");
        }

        // Probably a hack
        const type_meta& retMeta = m_returnType->getInfo();
        if (retMeta.size > 0 && !retMeta.is_opaque && (retMeta.size > 8 || !retMeta.is_trivial)) {
            m_cif.flags = FFI_TYPE_STRUCT;
        }
    }
};