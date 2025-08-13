#pragma once
#include <bind/Function.h>
#include <bind/Registry.hpp>
#include <bind/interfaces/ICallHandler.h>
#include <bind/interfaces/ITypeBuilder.h>
#include <bind/util/FuncWrap.hpp>
#include <bind/util/meta.hpp>
#include <utils/Exception.h>
#include <utils/Pointer.hpp>

namespace bind {
#define OP_BINDER(op, name)                              \
    template <typename Ret, typename Rhs>                \
    DataType::Property& name() {                         \
        return method<Ret, Rhs>(#op, &Cls::operator op); \
    }                                                    \
    template <typename Ret, typename Rhs>                \
    DataType::Property& name(Ret (Cls::*fn)(Rhs)) {      \
        return method<Ret, Rhs>(#op, fn);                \
    }                                                    \
    template <typename Ret, typename Rhs>                \
    DataType::Property& name(Ret (*fn)(Cls*, Rhs)) {     \
        return pseudoMethod(#op, fn);                    \
    }

    template <typename Cls>
    class ObjectTypeBuilder : public ITypeBuilder {
        public:
            ObjectTypeBuilder(const String& name, Namespace* ns)
                : ITypeBuilder(name, meta<Cls>(), ns, type_hash<Cls>()), m_hasDtor(false) {}
            ObjectTypeBuilder(DataType* extend) : ITypeBuilder(extend), m_hasDtor(false) {}

            template <typename BaseTp>
            DataType::BaseType& baseType()
                requires(std::is_base_of_v<BaseTp, Cls>)
            {
                DataType* baseType = Registry::GetType<BaseTp>();
                if (!baseType) {
                    throw InputException(String::Format(
                        "ObjectTypeBuilder::addBaseType - Base type '%s' has not been registered", type_name<BaseTp>()
                    ));
                }

                ptrdiff_t offset = ptrdiff_t(static_cast<const BaseTp*>(reinterpret_cast<const Cls*>(1u))) - 1u;
                return _addBaseType(baseType, u32(offset));
            }

            template <typename... Args>
            DataType::Property& ctor() {
                DataType::Property::Flags f = {0};
                f.can_read                  = 1;
                f.is_ctor                   = 1;

                Function* func = new Function(
                    ConstructorName,
                    &_constructor_wrapper<Cls, Args...>,
                    Registry::Signature<void, Cls*, Args...>(),
                    m_type->getOwnNamespace()
                );

                func->setCallHandler(new HostCallHandler(func));

                Registry::Add(func);

                return addProperty(Pointer(func), f, func->getSignature(), ConstructorName);
            }

            template <typename... Args>
            DataType::Property& pseudoCtor(void (*fn)(Cls*, Args...))
                requires(std::is_trivially_constructible_v<Cls, Args...>)
            {
                DataType::Property::Flags f = {0};
                f.can_read                  = 1;
                f.is_ctor                   = 1;

                Function* func = new Function(
                    ConstructorName, fn, Registry::Signature<void, Cls*, Args...>(), m_type->getOwnNamespace()
                );

                func->setCallHandler(new HostCallHandler(func));

                Registry::Add(func);

                return addProperty(Pointer(func), f, func->getSignature(), ConstructorName);
            }

            DataType::Property& dtor() {
                if (m_hasDtor) {
                    throw InvalidActionException(String::Format(
                        "ObjectTypeBuilder::dtor - Type '%s' already has a destructor", m_type->getFullName().c_str()
                    ));
                }

                DataType::Property::Flags f = {0};
                f.can_read                  = 1;
                f.is_dtor                   = 1;

                Function* func = new Function(
                    DestructorName,
                    &_destructor_wrapper<Cls>,
                    Registry::Signature<void, Cls*>(),
                    m_type->getOwnNamespace()
                );

                func->setCallHandler(new HostCallHandler(func));

                Registry::Add(func);

                return addProperty(Pointer(func), f, func->getSignature(), DestructorName);
            }

            template <typename Ret, typename... Args>
            DataType::Property& method(const String& name, Ret (Cls::*fn)(Args...)) {
                DataType::Property::Flags f = {0};
                f.can_read                  = 1;
                f.is_method                 = 1;

                Function* func =
                    new Function(name, fn, Registry::MethodSignature<Ret, Cls, Args...>(), m_type->getOwnNamespace());

                func->setCallHandler(new HostThisCallHandler(func));

                return addProperty(Pointer(func), f, func->getSignature(), name);
            }

            template <typename Ret, typename... Args>
            DataType::Property& method(const String& name, Ret (Cls::*fn)(Args...) const) {
                DataType::Property::Flags f = {0};
                f.can_read                  = 1;
                f.is_method                 = 1;

                Function* func =
                    new Function(name, fn, Registry::MethodSignature<Ret, Cls, Args...>(), m_type->getOwnNamespace());

                func->setCallHandler(new HostThisCallHandler(func));

                return addProperty(Pointer(func), f, func->getSignature(), name);
            }

            template <typename Ret, typename... Args>
            DataType::Property& pseudoMethod(const String& name, Ret (*fn)(Cls*, Args...)) {
                DataType::Property::Flags f = {0};
                f.can_read                  = 1;
                f.is_pseudo_method          = 1;

                Function* func =
                    new Function(name, fn, Registry::Signature<Ret, Cls*, Args...>(), m_type->getOwnNamespace());

                func->setCallHandler(new HostCallHandler(func));

                return addProperty(Pointer(func), f, func->getSignature(), name);
            }

            template <typename Ret, typename... Args>
            DataType::Property& staticMethod(const String& name, Ret (*fn)(Args...)) {
                DataType::Property::Flags f = {0};
                f.can_read                  = 1;
                f.is_method                 = 1;
                f.is_static                 = 1;

                Function* func = new Function(name, fn, Registry::Signature<Ret, Args...>(), m_type->getOwnNamespace());

                func->setCallHandler(new HostCallHandler(func));

                return addProperty(Pointer(func), f, func->getSignature(), name);
            }

            template <typename DestTp>
            std::enable_if_t<std::is_member_function_pointer_v<decltype(&Cls::operator DestTp)>, DataType::Property&>
            opCast() {
                return method(CastOperatorName, &Cls::operator DestTp);
            }

            template <typename Ret>
            DataType::Property& opCast(const String& name, Ret (*fn)(Cls*)) {
                return pseudoMethod(CastOperatorName, fn);
            }

            OP_BINDER(+, opAdd);
            OP_BINDER(-, opSub);
            OP_BINDER(*, opMul);
            OP_BINDER(/, opDiv);
            OP_BINDER(%, opMod);
            OP_BINDER(+=, opAddEq);
            OP_BINDER(-=, opSubEq);
            OP_BINDER(*=, opMulEq);
            OP_BINDER(/=, opDivEq);
            OP_BINDER(%=, opModEq);

            OP_BINDER(&&, opLogicalAnd);
            OP_BINDER(||, opLogicalOr);
            OP_BINDER(<<, opShiftLeft);
            OP_BINDER(>>, opShiftRight);
            OP_BINDER(&, opAnd);
            OP_BINDER(|, opOr);
            OP_BINDER(^, opXOr);
            OP_BINDER(&=, opAndEq);
            OP_BINDER(|=, opOrEq);
            OP_BINDER(^=, opXOrEq);

            OP_BINDER(=, opAssign);
            OP_BINDER(==, opEquality);
            OP_BINDER(!=, opInequality);
            OP_BINDER(>, opGreater);
            OP_BINDER(>=, opGreaterEq);
            OP_BINDER(<, opLess);
            OP_BINDER(<=, opLessEq);

            template <typename Ret>
            DataType::Property& opPreInc() {
                return method<Ret>("++", &Cls::operator++);
            }
            template <typename Ret>
            DataType::Property& opPreInc(Ret (Cls::*fn)()) {
                return method("++", fn);
            }
            template <typename Ret>
            DataType::Property& opPreInc(Ret (*fn)(Cls*)) {
                return pseudoMethod("++", fn);
            }

            template <typename Ret>
            DataType::Property& opPostInc() {
                return method<Ret, i32>("++", &Cls::operator++);
            }
            template <typename Ret>
            DataType::Property& opPostInc(Ret (Cls::*fn)(i32)) {
                return method("++", fn);
            }
            template <typename Ret>
            DataType::Property& opPostInc(Ret (*fn)(Cls*, i32)) {
                return pseudoMethod("++", fn);
            }

            template <typename Ret>
            DataType::Property& opPreDec() {
                return method<Ret>("--", &Cls::operator--);
            }
            template <typename Ret>
            DataType::Property& opPreDec(Ret (Cls::*fn)()) {
                return method("--", fn);
            }
            template <typename Ret>
            DataType::Property& opPreDec(Ret (*fn)(Cls*)) {
                return pseudoMethod("--", fn);
            }

            template <typename Ret>
            DataType::Property& opPostDec() {
                return method<Ret, i32>("--", &Cls::operator--);
            }
            template <typename Ret>
            DataType::Property& opPostDec(Ret (Cls::*fn)(i32)) {
                return method("--", fn);
            }
            template <typename Ret>
            DataType::Property& opPostDec(Ret (*fn)(Cls*, i32)) {
                return pseudoMethod("--", fn);
            }

            template <typename Ret>
            DataType::Property& opNegate() {
                return method<Ret>("-", &Cls::operator-);
            }
            template <typename Ret>
            DataType::Property& opNegate(Ret (Cls::*fn)()) {
                return method("-", fn);
            }
            template <typename Ret>
            DataType::Property& opNegate(Ret (*fn)(Cls*)) {
                return pseudoMethod("-", fn);
            }

            template <typename Ret>
            DataType::Property& opNot() {
                return method<Ret>("!", &Cls::operator!);
            }
            template <typename Ret>
            DataType::Property& opNot(Ret (Cls::*fn)()) {
                return method("!", fn);
            }
            template <typename Ret>
            DataType::Property& opNot(Ret (*fn)(Cls*)) {
                return pseudoMethod("!", fn);
            }

            template <typename Ret>
            DataType::Property& opInvert() {
                return method<Ret>("~", &Cls::operator~);
            }
            template <typename Ret>
            DataType::Property& opInvert(Ret (Cls::*fn)()) {
                return method("~", fn);
            }
            template <typename Ret>
            DataType::Property& opInvert(Ret (*fn)(Cls*)) {
                return pseudoMethod("~", fn);
            }

            template <typename T>
            DataType::Property& prop(const String& name, T Cls::* member) {
                DataType* tp = Registry::GetType<T>();
                if (!tp) {
                    throw InputException(String::Format(
                        "ObjectTypeBuilder::prop - Type '%s' for property '%s' of '%s' has not been registered",
                        type_name<T>(),
                        name.c_str(),
                        m_type->getFullName().c_str()
                    ));
                }

                DataType::Property::Flags f = {0};
                f.can_read                  = 1;
                f.can_write                 = 1;

                i32 offset = i32((u8*)&((Cls*)nullptr->*member) - (u8*)nullptr);
                return addProperty(offset, f, tp, name);
            }

            template <typename T>
            DataType::Property& staticProp(const String& name, T* member) {
                DataType* tp = Registry::GetType<T>();
                if (!tp) {
                    throw InputException(String::Format(
                        "ObjectTypeBuilder::staticProp - Type '%s' for property '%s' of '%s' has not been registered",
                        type_name<T>(),
                        name.c_str(),
                        m_type->getFullName().c_str()
                    ));
                }

                DataType::Property::Flags f = {0};
                f.can_read                  = 1;
                f.can_write                 = 1;
                f.is_static                 = 1;

                Registry::Add(new ValuePointer(name, tp, member, m_type->getOwnNamespace()));
                return addProperty((void*)member, f, tp, name);
            }

        protected:
            bool m_hasDtor;
    };

#undef OP_BINDER
};