#pragma once
#include <bind/DataType.h>

#define FFI_STATIC_BUILD
#define FFI_BUILDING
#include <ffi.h>

namespace bind {
    class FunctionType : public DataType {
        public:
            struct Argument : public IWithFixedUserData<32> {
                Argument(u8 index, DataType* type);
                
                u8 index;
                DataType* type;
            };

            FunctionType(const String& name, const type_meta& meta);
            ~FunctionType();

            const Array<Argument>& getArgs() const;
            DataType* getReturnType() const;
            DataType* getThisType() const;
            const Pointer& getWrapperAddress() const;
            ffi_cif* getCif();

            /**
             * @brief Calls a function pointer that has this signature
             * 
             * @param funcPtr Pointer to the function
             * @param retDest Pointer to memory that will receive the return value, should
             * be `max(return type size, 8)` bytes.
             * @param args 
             */
            void call(const Pointer& funcPtr, void* retDest, void** args);
        
        protected:
            friend class Registry;
            void initCallInterface(ffi_abi abi);

            DataType* m_returnType;
            DataType* m_thisType;
            ffi_cif m_cif;
            Pointer m_wrapperAddress;
            Array<ffi_type*> m_ffiArgTypes;
            Array<Argument> m_args;
    };
};