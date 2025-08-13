#include <bind/Function.h>
#include <bind/FunctionType.h>
#include <bind/interfaces/ICallHandler.h>
#include <utils/Exception.h>
#include <utils/Pointer.hpp>

namespace bind {
    ICallHandler::ICallHandler(Function* target) : m_target(target) {}
    ICallHandler::~ICallHandler() {}

    HostCallHandler::HostCallHandler(Function* target) : ICallHandler(target) {
        m_cif = m_target->getSignature()->getCif();
        m_target->getAddress().get(&m_func);

        const type_meta& ri = m_target->getSignature()->getReturnType()->getInfo();
        m_retSz             = ri.size;
        m_doProxyReturnVal  = m_retSz > 0 && m_retSz < sizeof(ffi_arg);
    }

    void HostCallHandler::call(void* retDest, void** args) {
        if (m_doProxyReturnVal) {
            ffi_arg retVal = 0;
            ffi_call(m_cif, m_func, &retVal, args);

            switch (m_retSz) {
                case 8: {
                    *((u64*)retDest) = u64(retVal);
                    break;
                }
                case 4: {
                    *((u32*)retDest) = u32(retVal);
                    break;
                }
                case 2: {
                    *((u16*)retDest) = u16(retVal);
                    break;
                }
                case 1: {
                    *((u8*)retDest) = u8(retVal);
                    break;
                }
            }
        } else {
            ffi_call(m_cif, m_func, retDest, args);
        }
    }

    HostThisCallHandler::HostThisCallHandler(Function* target) : HostCallHandler(target) {
        m_target->getSignature()->getWrapperAddress().get(&m_func);
        m_explicitArgC = m_target->getSignature()->getArgs().size() - 2;
    }

    void HostThisCallHandler::call(void* retDest, void** args) {
        void* finalArgs[32];

        // wrapper signature: (method pointer, instance pointer, arguments...)
        finalArgs[0] = &m_target;
        finalArgs[1] = args[0];
        for (u32 i = 0; i < m_explicitArgC; i++) {
            finalArgs[i + 2] = args[i + 1];
        }

        if (m_doProxyReturnVal) {
            ffi_arg retVal = 0;
            ffi_call(m_cif, m_func, &retVal, finalArgs);

            switch (m_retSz) {
                case 8: {
                    *((u64*)retDest) = u64(retVal);
                    break;
                }
                case 4: {
                    *((u32*)retDest) = u32(retVal);
                    break;
                }
                case 2: {
                    *((u16*)retDest) = u16(retVal);
                    break;
                }
                case 1: {
                    *((u8*)retDest) = u8(retVal);
                    break;
                }
            }
        } else {
            ffi_call(m_cif, m_func, retDest, finalArgs);
        }
    }
};