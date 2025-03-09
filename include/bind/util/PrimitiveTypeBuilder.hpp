#pragma once
#include <bind/interfaces/ITypeBuilder.h>
#include <bind/interfaces/ICallHandler.h>
#include <bind/util/meta.hpp>
#include <bind/Registry.hpp>
#include <bind/Function.h>
#include <utils/Exception.h>
#include <utils/Pointer.hpp>

namespace bind {
    template <typename Prim>
    class PrimitiveTypeBuilder : public ITypeBuilder {
        public:
            PrimitiveTypeBuilder(const String& name, Namespace* ns)
                : ITypeBuilder(name, meta<Prim>(), ns, type_hash<Prim>()), m_hasDtor(false) { }
            PrimitiveTypeBuilder(DataType* extend) : ITypeBuilder(extend), m_hasDtor(false) { }

            template <typename... Args>
            DataType::Property& ctor(void (*fn)(Prim*, Args...)) {
                DataType::Property::Flags f = { 0 };
                f.can_read = 1;
                f.is_ctor = 1;

                Function* func = new Function(
                    ConstructorName,
                    fn,
                    Registry::Signature<void, Prim*, Args...>(),
                    m_type->getOwnNamespace()
                );

                func->setCallHandler(new HostCallHandler(func));

                Registry::Add(func);

                return addProperty(
                    Pointer(func),
                    f,
                    func->getSignature(),
                    ConstructorName
                );
            }

            DataType::Property& dtor(void (*fn)(Prim*)) {
                if (m_hasDtor) {
                    throw Exception(String::Format("PrimitiveTypeBuilder::dtor - Type '%s' already has a destructor", m_type->getFullName().c_str()));
                }

                DataType::Property::Flags f = { 0 };
                f.can_read = 1;
                f.is_dtor = 1;

                Function* func = new Function(
                    DestructorName,
                    fn,
                    Registry::Signature<void, Prim*>(),
                    m_type->getOwnNamespace()
                );

                func->setCallHandler(new HostCallHandler(func));

                Registry::Add(func);

                return addProperty(
                    Pointer(func),
                    f,
                    func->getSignature(),
                    DestructorName
                );
            }

            template <typename Ret, typename... Args>
            DataType::Property& method(const String& name, Ret (*fn)(Prim*, Args...)) {
                DataType::Property::Flags f = { 0 };
                f.can_read = 1;
                f.is_pseudo_method = 1;

                Function* func = new Function(
                    name,
                    fn,
                    Registry::Signature<Ret, Prim*, Args...>(),
                    m_type->getOwnNamespace()
                );

                func->setCallHandler(new HostCallHandler(func));

                Registry::Add(func);

                return addProperty(
                    Pointer(func),
                    f,
                    func->getSignature(),
                    name
                );
            }

            template <typename Ret, typename... Args>
            DataType::Property& staticMethod(const String& name, Ret (*fn)(Args...)) {
                DataType::Property::Flags f = { 0 };
                f.can_read = 1;
                f.is_method = 1;
                f.is_static = 1;

                Function* func = new Function(
                    name,
                    fn,
                    Registry::Signature<Ret, Args...>(),
                    m_type->getOwnNamespace()
                );

                func->setCallHandler(new HostCallHandler(func));

                Registry::Add(func);

                return addProperty(
                    Pointer(func),
                    f,
                    func->getSignature(),
                    name
                );
            }

            template <typename Ret>
            DataType::Property& opCast(const String& name, Ret (*fn)(Prim*)) {
                return method(CastOperatorName, fn);
            }

            template <typename Ret, typename Rhs> DataType::Property& opAdd       (Ret (*fn)(Prim*, Rhs)) { return method("+" , fn); }
            template <typename Ret, typename Rhs> DataType::Property& opSub       (Ret (*fn)(Prim*, Rhs)) { return method("-" , fn); }
            template <typename Ret, typename Rhs> DataType::Property& opMul       (Ret (*fn)(Prim*, Rhs)) { return method("*" , fn); }
            template <typename Ret, typename Rhs> DataType::Property& opDiv       (Ret (*fn)(Prim*, Rhs)) { return method("/" , fn); }
            template <typename Ret, typename Rhs> DataType::Property& opMod       (Ret (*fn)(Prim*, Rhs)) { return method("%" , fn); }
            template <typename Ret, typename Rhs> DataType::Property& opAddEq     (Ret (*fn)(Prim*, Rhs)) { return method("+=", fn); }
            template <typename Ret, typename Rhs> DataType::Property& opSubEq     (Ret (*fn)(Prim*, Rhs)) { return method("-=", fn); }
            template <typename Ret, typename Rhs> DataType::Property& opMulEq     (Ret (*fn)(Prim*, Rhs)) { return method("*=", fn); }
            template <typename Ret, typename Rhs> DataType::Property& opDivEq     (Ret (*fn)(Prim*, Rhs)) { return method("/=", fn); }
            template <typename Ret, typename Rhs> DataType::Property& opModEq     (Ret (*fn)(Prim*, Rhs)) { return method("%=", fn); }
            template <typename Ret, typename Rhs> DataType::Property& opLogicalAnd(Ret (*fn)(Prim*, Rhs)) { return method("&&", fn); }
            template <typename Ret, typename Rhs> DataType::Property& opLogicalOr (Ret (*fn)(Prim*, Rhs)) { return method("||", fn); }
            template <typename Ret, typename Rhs> DataType::Property& opShiftLeft (Ret (*fn)(Prim*, Rhs)) { return method("<<", fn); }
            template <typename Ret, typename Rhs> DataType::Property& opShiftRight(Ret (*fn)(Prim*, Rhs)) { return method(">>", fn); }
            template <typename Ret, typename Rhs> DataType::Property& opAnd       (Ret (*fn)(Prim*, Rhs)) { return method("&" , fn); }
            template <typename Ret, typename Rhs> DataType::Property& opOr        (Ret (*fn)(Prim*, Rhs)) { return method("|" , fn); }
            template <typename Ret, typename Rhs> DataType::Property& opXOr       (Ret (*fn)(Prim*, Rhs)) { return method("^" , fn); }
            template <typename Ret, typename Rhs> DataType::Property& opAndEq     (Ret (*fn)(Prim*, Rhs)) { return method("&=", fn); }
            template <typename Ret, typename Rhs> DataType::Property& opOrEq      (Ret (*fn)(Prim*, Rhs)) { return method("|=", fn); }
            template <typename Ret, typename Rhs> DataType::Property& opXOrEq     (Ret (*fn)(Prim*, Rhs)) { return method("^=", fn); }
            template <typename Ret, typename Rhs> DataType::Property& opAssign    (Ret (*fn)(Prim*, Rhs)) { return method("=" , fn); }
            template <typename Ret, typename Rhs> DataType::Property& opEquality  (Ret (*fn)(Prim*, Rhs)) { return method("==", fn); }
            template <typename Ret, typename Rhs> DataType::Property& opInequality(Ret (*fn)(Prim*, Rhs)) { return method("!=", fn); }
            template <typename Ret, typename Rhs> DataType::Property& opGreater   (Ret (*fn)(Prim*, Rhs)) { return method(">" , fn); }
            template <typename Ret, typename Rhs> DataType::Property& opGreaterEq (Ret (*fn)(Prim*, Rhs)) { return method(">=", fn); }
            template <typename Ret, typename Rhs> DataType::Property& opLess      (Ret (*fn)(Prim*, Rhs)) { return method("<" , fn); }
            template <typename Ret, typename Rhs> DataType::Property& opLessEq    (Ret (*fn)(Prim*, Rhs)) { return method("<=", fn); }
            template <typename Ret>               DataType::Property& opPreInc    (Ret (*fn)(Prim*))      { return method("++", fn); }
            template <typename Ret>               DataType::Property& opPostInc   (Ret (*fn)(Prim*, i32)) { return method("++", fn); }
            template <typename Ret>               DataType::Property& opPreDec    (Ret (*fn)(Prim*))      { return method("--", fn); }
            template <typename Ret>               DataType::Property& opPostDec   (Ret (*fn)(Prim*, i32)) { return method("--", fn); }
            template <typename Ret>               DataType::Property& opNegate    (Ret (*fn)(Prim*))      { return method("-" , fn); }
            template <typename Ret>               DataType::Property& opNot       (Ret (*fn)(Prim*))      { return method("!" , fn); }
            template <typename Ret>               DataType::Property& opInvert    (Ret (*fn)(Prim*))      { return method("~" , fn); }

            template <typename T>
            DataType::Property& staticProp(const String& name, T* member) {
                DataType* tp = Registry::GetType<T>();
                if (!tp) {
                    throw Exception(String::Format(
                        "PrimitiveTypeBuilder::staticProp - Type '%s' for property '%s' of '%s' has not been registered",
                        type_name<T>(),
                        name.c_str(),
                        m_type->getFullName().c_str()
                    ));
                }

                DataType::Property::Flags f = { 0 };
                f.can_read = 1;
                f.can_write = 1;
                f.is_static = 1;

                Registry::Add(new ValuePointer(name, tp, member, m_type->getOwnNamespace()));
                return addProperty((void*)member, f, tp, name);
            }

        protected:
            bool m_hasDtor;
    };
};