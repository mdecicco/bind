#pragma once
#include <bind/interfaces/ISymbol.h>
#include <bind/interfaces/ICallHandler.h>
#include <bind/FunctionType.h>
#include <utils/interfaces/IWithUserData.h>
#include <utils/Pointer.h>

namespace bind {
    class ICallHandler;

    class Function : public ISymbol, public IWithFixedUserData<32> {
        public:
            Function(const String& name, FunctionType* sig, Namespace* ns);
            Function(const String& name, const Pointer& address, FunctionType* sig, Namespace* ns);
            virtual ~Function();

            void setCallHandler(ICallHandler* callHandler);
            void call(void* retDest, void** args);
            
            template <typename... Args>
            void vcall(void* retDest, Args... args) {
                void* argPtrs[] = { (&args)..., nullptr };
                m_callHandler->call(retDest, argPtrs);
            }

            FunctionType* getSignature() const;
            ConstArrayView<FunctionType::Argument> getExplicitArgs() const;
            ICallHandler* getCallHandler() const;
            const Pointer& getAddress() const;
        
        protected:
            friend class ITypeBuilder;
            FunctionType* m_signature;
            ICallHandler* m_callHandler;
            u32 m_implicitArgCount;
            Pointer m_address;
    };
};