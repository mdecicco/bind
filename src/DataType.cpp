#include <bind/DataType.h>
#include <bind/AliasType.h>
#include <bind/FunctionType.h>
#include <bind/Namespace.h>
#include <utils/interfaces/IWithUserData.hpp>
#include <utils/Array.hpp>

namespace bind {
    DataType::Property::Property(
        i32 _offset,
        DataType::Property::Flags _flags,
        DataType* _type,
        const String& _name,
        AccessFlags _accessFlags
    ) : offset(_offset), flags(_flags), type(_type), name(_name), accessFlags(_accessFlags)
    {
        if (flags.is_ctor || flags.is_dtor || flags.is_method || flags.is_pseudo_method || flags.is_static) {
            if (offset != -1) {
                throw Exception("Only non-static object members can have offset >= 0");
            }
        } else {
            if (offset == -1) {
                throw Exception("Non-static object members must have offset >= 0");
            }
        }
    }

    DataType::Property::Property(
        const Pointer& _address,
        DataType::Property::Flags _flags,
        DataType* _type,
        const String& _name,
        AccessFlags _accessFlags
    ) : offset(-1), address(_address), flags(_flags), type(_type), name(_name), accessFlags(_accessFlags)
    {}
    
    DataType::Property::Property(const Property& prop)
        : offset(prop.offset), address(prop.address), flags(prop.flags),
          type(prop.type), name(prop.name), accessFlags(prop.accessFlags)
    {
    }

    DataType::DataType(const String& name, const type_meta& meta, Namespace* ns)
        : ISymbol(name, ISymbol::genFullSymbolName(ns, name), ISymbol::genTypeSymbolName(ns, name), SymbolType::DataType),
          m_info(meta), m_ownNamespace(new Namespace(ns, this)), m_pointerToSelf(nullptr)
    {
        Registry::Add(m_ownNamespace);
        if (ns) ns->add(this);
        else Registry::GlobalNamespace()->add(this);

        if (meta.is_primitive) {
            if (meta.is_integral) {
                if (meta.is_unsigned) {
                    switch (meta.size) {
                        case 1: { m_ffi = ffi_type_uint8; break; }
                        case 2: { m_ffi = ffi_type_uint16; break; }
                        case 4: { m_ffi = ffi_type_uint32; break; }
                        case 8: { m_ffi = ffi_type_uint64; break; }
                    }
                } else {
                    switch (meta.size) {
                        case 1: { m_ffi = ffi_type_sint8; break; }
                        case 2: { m_ffi = ffi_type_sint16; break; }
                        case 4: { m_ffi = ffi_type_sint32; break; }
                        case 8: { m_ffi = ffi_type_sint64; break; }
                    }
                }
            } else if (meta.is_floating_point) {
                if (meta.size == sizeof(f32)) m_ffi = ffi_type_float;
                else m_ffi = ffi_type_double;
            } else if (meta.size == 0) {
                m_ffi = ffi_type_void;
            }
        } else if (meta.is_pointer) {
            m_ffi = ffi_type_pointer;
        } else if (meta.size == 0) {
            m_ffi = ffi_type_void;
        } else {
            m_ffi.size = meta.size;
            m_ffi.alignment = 0;
            m_ffi.type = FFI_TYPE_STRUCT;
            m_ffiElems.push(nullptr);
        }
    }

    DataType::DataType(const String& name, const String& fullName, const type_meta& meta, Namespace* ns)
        : ISymbol(name, fullName, ISymbol::genTypeSymbolName(ns, name), SymbolType::DataType),
          m_info(meta), m_ownNamespace(new Namespace(ns, this)), m_pointerToSelf(nullptr)
    {
        Registry::Add(m_ownNamespace);
        if (ns) ns->add(this);
        else Registry::GlobalNamespace()->add(this);

        if (meta.is_primitive) {
            if (meta.is_integral) {
                if (meta.is_unsigned) {
                    switch (meta.size) {
                        case 1: { m_ffi = ffi_type_uint8; break; }
                        case 2: { m_ffi = ffi_type_uint16; break; }
                        case 4: { m_ffi = ffi_type_uint32; break; }
                        case 8: { m_ffi = ffi_type_uint64; break; }
                    }
                } else {
                    switch (meta.size) {
                        case 1: { m_ffi = ffi_type_sint8; break; }
                        case 2: { m_ffi = ffi_type_sint16; break; }
                        case 4: { m_ffi = ffi_type_sint32; break; }
                        case 8: { m_ffi = ffi_type_sint64; break; }
                    }
                }
            } else if (meta.is_floating_point) {
                if (meta.size == sizeof(f32)) m_ffi = ffi_type_float;
                else m_ffi = ffi_type_double;
            }
        } else if (meta.is_pointer) {
            m_ffi = ffi_type_pointer;
        } else if (meta.size == 0) {
            m_ffi = ffi_type_void;
        } else {
            m_ffi.size = meta.size;
            m_ffi.alignment = 0;
            m_ffi.type = FFI_TYPE_STRUCT;
            m_ffiElems.push(nullptr);
        }
    }

    DataType::~DataType() {
        // namespace will be deleted by the registry
    }

    const type_meta& DataType::getInfo() const {
        return m_info;
    }
    
    Namespace* DataType::getOwnNamespace() const {
        return m_ownNamespace;
    }

    const Array<DataType::Property>& DataType::getProps() const {
        return m_props;
    }
    
    Array<const DataType::Property*> DataType::findProperties(const String& name, AccessFlags accessMask) const {
        Array<const Property*> ret;

        for (u32 i = 0;i < m_props.size();i++) {
            const Property& p = m_props[i];
            if (p.accessFlags != PublicAccess && (p.accessFlags & accessMask) != p.accessFlags) continue;
            if (p.name == name) ret.push(&p);
        }

        return ret;
    }
    
    Array<Function*> DataType::findMethods(const FuncMatch& search, Function** singleStrictMatch) const {
        if (singleStrictMatch) *singleStrictMatch = nullptr;

        Array<Function*> ret;

        u32 strictMatchCount = 0;

        for (u32 i = 0;i < m_props.size();i++) {
            const Property& p = m_props[i];
            if (p.flags.is_method == 0 && p.flags.is_pseudo_method == 0) continue;
            if (p.accessFlags != PublicAccess && (p.accessFlags & search.accessMask) != p.accessFlags) continue;

            FunctionType* sig = (FunctionType*)p.type;
            
            if (p.name.size() != search.name.size()) continue;
            if (p.name != search.name) continue;

            bool isStrictMatch = true;
            
            if (search.returnType) {
                if (search.returnTypeStrict) {
                    if (!sig->getReturnType()->isEqualTo(search.returnType)) continue;
                } else {
                    if (!sig->getReturnType()->isConvertibleTo(search.returnType)) continue;

                    if (strictMatchCount <= 1 && singleStrictMatch && !sig->getReturnType()->isEqualTo(search.returnType)) {
                        isStrictMatch = false;
                    }
                }
            }

            if (search.doCheckArgs) {
                auto args = ((Function*)p.address.get())->getExplicitArgs();

                if (search.argTypes.size() != args.size()) continue;

                bool isMatch = true;
                for (u32 a = 0;a < args.size();a++) {
                    DataType* providedType = search.argTypes[a];

                    if (search.argTypesStrict) {
                        if (!args[a].type->isEqualTo(providedType)) {
                            isMatch = false;
                            break;
                        }
                    } else {
                        if (!args[a].type->isConvertibleTo(providedType)) {
                            isMatch = false;
                            break;
                        }

                        if (strictMatchCount <= 1 && isStrictMatch && singleStrictMatch && !args[a].type->isEqualTo(providedType)) {
                            isStrictMatch = false;
                        }
                    }
                }

                if (!isMatch) continue;
            }

            ret.push((Function*)p.address.get());

            if (isStrictMatch && singleStrictMatch) {
                if (strictMatchCount == 0) *singleStrictMatch = (Function*)p.address.get();
                else *singleStrictMatch = nullptr;

                strictMatchCount++;
            }
        }

        return ret;
    }

    Array<Function*> DataType::findConstructors(
        const Array<DataType*>& argTypes,
        bool strict,
        AccessFlags accessMask,
        Function** singleStrictMatch
    ) const {
        if (singleStrictMatch) *singleStrictMatch = nullptr;

        Array<Function*> ret;

        u32 strictMatchCount = 0;

        for (u32 i = 0;i < m_props.size();i++) {
            const Property& p = m_props[i];
            if (p.flags.is_ctor == 0) continue;
            if (p.accessFlags != PublicAccess && (p.accessFlags & accessMask) != p.accessFlags) continue;

            bool isStrictMatch = true;

            FunctionType* sig = (FunctionType*)p.type;
            
            if (p.name.size() != ConstructorNameLen) continue;
            if (p.name != ConstructorName) continue;
            
            auto args = ((Function*)p.address.get())->getExplicitArgs();
            if (argTypes.size() != args.size()) continue;

            bool isMatch = true;
            for (u32 a = 0;a < args.size();a++) {
                if (strict) {
                    if (!args[a].type->isEqualTo(argTypes[a])) {
                        isMatch = false;
                        break;
                    }
                } else {
                    if (!args[a].type->isConvertibleTo(argTypes[a])) {
                        isMatch = false;
                        break;
                    }

                    if (strictMatchCount <= 1 && isStrictMatch && singleStrictMatch && !args[a].type->isEqualTo(argTypes[a])) {
                        isStrictMatch = false;
                    }
                }
            }

            if (!isMatch) continue;

            ret.push((Function*)p.address.get());

            if (isStrictMatch && singleStrictMatch) {
                if (strictMatchCount == 0) *singleStrictMatch = (Function*)p.address.get();
                else *singleStrictMatch = nullptr;

                strictMatchCount++;
            }
        }

        return ret;
    }

    Array<Function*> DataType::getConstructors(AccessFlags accessMask) const {
        Array<Function*> ret;

        for (u32 i = 0;i < m_props.size();i++) {
            const Property& p = m_props[i];
            if (p.flags.is_ctor == 0) continue;
            if (p.accessFlags != PublicAccess && (p.accessFlags & accessMask) != p.accessFlags) continue;

            FunctionType* sig = (FunctionType*)p.type;
            
            if (p.name.size() != ConstructorNameLen) continue;
            if (p.name != ConstructorName) continue;

            ret.push((Function*)p.address.get());
        }

        return ret;
    }

    Function* DataType::getDestructor(AccessFlags accessMask) {
        for (u32 i = 0;i < m_props.size();i++) {
            const Property& p = m_props[i];
            if (p.flags.is_dtor == 0) continue;
            if (p.accessFlags != PublicAccess && (p.accessFlags & accessMask) != p.accessFlags) continue;

            return (Function*)p.address.get();
        }

        return nullptr;
    }

    Function* DataType::findConversionOperator(DataType* resultType, AccessFlags accessMask) const {
        for (u32 i = 0;i < m_props.size();i++) {
            const Property& p = m_props[i];
            if (p.flags.is_method == 0 && p.flags.is_pseudo_method == 0) continue;
            if (p.accessFlags != PublicAccess && (p.accessFlags & accessMask) != p.accessFlags) continue;

            FunctionType* sig = (FunctionType*)p.type;
            
            if (((Function*)p.address.get())->getExplicitArgs().size() != 0) continue;
            if (p.name.size() != CastOperatorNameLen) continue;
            if (p.name != CastOperatorName) continue;

            if (!sig->getReturnType()->isEqualTo(resultType)) continue;

            return (Function*)p.address.get();
        }

        return nullptr;
    }

    bool DataType::isConvertibleTo(DataType* _to, AccessFlags accessMask) const {
        if (!_to) return false;
        DataType* to = _to->getEffectiveType();

        if (to->m_info.size == 0) {
            if (m_info.size == 0) return true;
            return false;
        }

        if (m_info.size == 0) {
            if (to->m_info.size == 0) return true;
            return false;
        }

        // all primitives are convertible to each other
        if (m_info.is_primitive && to->m_info.is_primitive) return true;
        if (m_info.is_pointer && to->m_info.is_pointer) return true;

        // no conversion necessary
        if (isEqualTo(to)) return true;

        auto castMethods = findMethods(
            FuncMatch(CastOperatorName)
            .retTp(to, true)
            .noArgs()
            .access(accessMask)
        );

        // conversion via cast operator
        if (castMethods.size() == 1) return true;

        DataType* self = getEffectiveType();

        auto copyCtors = to->findConstructors({ self }, true, accessMask);

        // conversion via copy constructor
        if (copyCtors.size() == 1) return true;

        //
        // See if this type can be shoehorned into the other type
        //
        if (!to->m_info.is_trivially_copyable) {
            // Nope, other type requires a constructor to be called
            return false;
        }

        u32 accountedSize = 0;

        for (u32 i = 0;i < to->m_props.size();i++) {
            auto& rProp = to->m_props[i];
            if (rProp.offset == -1) continue;
            accountedSize += rProp.type->getInfo().size;

            bool found = false;
            for (u32 j = 0;j < m_props.size();j++) {
                auto& sProp = m_props[j];
                if (sProp.offset == -1) continue;
                if (sProp.name != rProp.name) continue;
                if (!sProp.type->isConvertibleTo(rProp.type)) {
                    // Nope, this property of the other type cannot be
                    // converted to from our property with the same name
                    return false;
                }

                found = true;
                break;
            }

            if (!found) {
                // Nope, this property of the other type has no counterpart
                // on this type
                return false;
            }
        }

        if (accountedSize != to->m_info.size) {
            // Nope, there are fields in the destination type that haven't been
            // bound
            return false;
        }

        // Yup
        return true;
    }

    bool DataType::isEqualTo(DataType* to) const {
        if (!to) return false;
        return getEffectiveType()->getSymbolId() == to->getEffectiveType()->getSymbolId();
    }

    bool DataType::isConstructableWith(const Array<DataType*>& args, AccessFlags accessMask, bool strict) const {
        auto ctors = getEffectiveType()->findConstructors(args, strict, accessMask);
        return ctors.size() == 1;
    }

    DataType* DataType::getEffectiveType() const {
        if (m_info.is_alias == 0) return const_cast<DataType*>(this);
        return ((AliasType*)this)->getBaseType();
    }

    PointerType* DataType::getPointerType() {
        if (m_pointerToSelf) return m_pointerToSelf;

        m_pointerToSelf = new PointerType(this);
        Registry::Add(m_pointerToSelf);
        return m_pointerToSelf;
    }
    
    ffi_type* DataType::getFFI() {
        return &m_ffi;
    }
};