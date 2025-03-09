#pragma once
#include <bind/types.h>
#include <utils/interfaces/IWithUserData.h>
#include <type_traits>

#define FFI_STATIC_BUILD
#define FFI_BUILDING
#include <ffi.h>

namespace bind {
    class Function;
    class Call;

    class ICallHandler {
        public:
            ICallHandler(Function* target);
            virtual ~ICallHandler();
            
            /**
             * @brief Calls a function
             * 
             * @param retDest Pointer to memory that will receive the return value. Memory
             * pointed to should have at least enough space to fit the return type. If the
             * target does not return a value then this parameter is ignored.
             * @param args Array of pointers to argument values. First element should be
             * 'this' pointer if this is a call to a type method
             */
            virtual void call(void* retDest, void** args) = 0;

        protected:
            Function* m_target;
    };

    class HostCallHandler : public ICallHandler {
        public:
            HostCallHandler(Function* target);
            
            virtual void call(void* retDest, void** args);
        
        protected:
            bool m_doProxyReturnVal;
            u16 m_retSz;
            ffi_cif* m_cif;
            void(*m_func)();
    };

    class HostThisCallHandler : public HostCallHandler {
        public:
            HostThisCallHandler(Function* target);
        
            virtual void call(void* retDest, void** args);
        
        protected:
            u32 m_explicitArgC;
    };
};