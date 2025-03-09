#include <bind/interfaces/ITypeBuilder.h>
#include <bind/DataType.h>
#include <bind/Function.h>
#include <bind/Registry.h>
#include <utils/Array.hpp>
#include <utils/Exception.h>
#include <utils/interfaces/IWithUserData.hpp>

namespace bind {
    ITypeBuilder::ITypeBuilder(const String& name, const type_meta& meta, Namespace* ns, size_t nativeHash) {
        m_type = new DataType(name, meta, ns);
        Registry::Add(m_type, nativeHash);
    }

    ITypeBuilder::ITypeBuilder(DataType* extend) {
        m_type = extend;
    }

    ITypeBuilder::~ITypeBuilder() {
        m_type = nullptr;
    }

    DataType::Property& ITypeBuilder::addProperty(
        i32 offset,
        DataType::Property::Flags flags,
        DataType* type,
        String name
    ) {
        DataType::Property* existing = m_type->m_props.find([&name](const DataType::Property& p) {
            if (p.flags.is_method || p.flags.is_pseudo_method || p.flags.is_ctor || p.flags.is_dtor) return false;
            return p.name == name;
        });

        if (existing) {
            throw Exception("Type '%s' already has a member named '%s'", m_type->getName().c_str(), name.c_str());
        }

        m_type->m_props.push(DataType::Property(offset, flags, type, name));
        
        if (offset >= 0) updateFFI();

        return m_type->m_props.last();
    }

    DataType::Property& ITypeBuilder::addProperty(
        const Pointer& address,
        DataType::Property::Flags flags,
        DataType* type,
        String name
    ) {
        if (flags.is_method || flags.is_pseudo_method) {
            if (flags.is_pseudo_method) ((Function*)address.get())->m_implicitArgCount = 1; // 'this' pointer
            else ((Function*)address.get())->m_implicitArgCount = 2; // `Function` pointer, 'this' pointer

            DataType::Property* existing = m_type->m_props.find([&name, &type](const DataType::Property& p) {
                if (!p.flags.is_method && !p.flags.is_pseudo_method) return false;
                if (p.name != name) return false;

                // todo:
                // It may be necessary to do extra work since a normal method and pseudo-method with the
                // same effective signature will have different signatures. Pseudo-method functions on
                // the host have an explicit 'this' argument that is reflected in its signature

                return p.type->isEqualTo(type);
            });

            if (existing) {
                throw Exception(
                    "Type '%s' already has a method named '%s' with the signature '%s'",
                    m_type->getName().c_str(),
                    name.c_str(),
                    type->getFullName().c_str()
                );
            }
        } else if (flags.is_ctor) {
            ((Function*)address.get())->m_implicitArgCount = 1; // 'this' pointer

            DataType::Property* existing = m_type->m_props.find([&name, &type](const DataType::Property& p) {
                if (!p.flags.is_ctor) return false;
                if (p.name != name) return false;
                return p.type->isEqualTo(type);
            });

            if (existing) {
                throw Exception(
                    "Type '%s' already has a constructor with the signature '%s'",
                    m_type->getName().c_str(),
                    type->getFullName().c_str()
                );
            }
        } else if (flags.is_dtor) {
            ((Function*)address.get())->m_implicitArgCount = 1; // 'this' pointer

            DataType::Property* existing = m_type->m_props.find([&name](const DataType::Property& p) {
                return p.flags.is_dtor == 1;
            });

            if (existing) {
                throw Exception("Type '%s' already has a destructor", m_type->getName().c_str());
            }
        } else {
            // static property
            DataType::Property* existing = m_type->m_props.find([&name](const DataType::Property& p) {
                if (p.flags.is_method || p.flags.is_pseudo_method || p.flags.is_ctor || p.flags.is_dtor) return false;
                return p.name == name;
            });

            if (existing) {
                throw Exception("Type '%s' already has a member named '%s'", m_type->getName().c_str(), name.c_str());
            }
        }

        m_type->m_props.push(DataType::Property(address, flags, type, name));
        return m_type->m_props.last();
    }
    
    EnumType::Field& ITypeBuilder::_addEnumValue(const String& name, u64 value) {
        ((EnumType*)m_type)->m_fields.push({ name });
        ((EnumType*)m_type)->m_fields.last().value.u = value;
        ((EnumType*)m_type)->m_fieldIndexMap.insert(std::pair<String, u32>(name, ((EnumType*)m_type)->m_fields.size() - 1));
        return ((EnumType*)m_type)->m_fields.last();
    }
    
    EnumType::Field& ITypeBuilder::_addEnumValue(const String& name, i64 value) {
        ((EnumType*)m_type)->m_fields.push({ name });
        ((EnumType*)m_type)->m_fields.last().value.i = value;
        ((EnumType*)m_type)->m_fieldIndexMap.insert(std::pair<String, u32>(name, ((EnumType*)m_type)->m_fields.size() - 1));
        return ((EnumType*)m_type)->m_fields.last();
    }

    void ITypeBuilder::updateFFI() {
        Array<DataType::Property*> orderedProps(m_type->m_props.size());
        for (u32 i = 0;i < m_type->m_props.size();i++) {
            auto& p = m_type->m_props[i];
            if (p.offset == -1) continue;
            orderedProps.push(&p);
        }
        orderedProps.sort([](DataType::Property* a, DataType::Property* b){
            return a->offset < b->offset;
        });

        m_type->m_ffiElems.clear();

        u32 expectedNextOffset = 0;

        u32 totalSize = 0;
        for (u32 i = 0;i < orderedProps.size();i++) {
            DataType::Property* p = orderedProps[i];
            if (p->offset == -1) continue;

            if (u32(p->offset) > expectedNextOffset) {
                u32 paddingAmount = p->offset - expectedNextOffset;
                totalSize += paddingAmount;
                while (paddingAmount > 0) {
                    if (paddingAmount >= sizeof(u64)) {
                        m_type->m_ffiElems.push(&ffi_type_uint64);
                        paddingAmount -= sizeof(u64);
                    } else if (paddingAmount >= sizeof(u32)) {
                        m_type->m_ffiElems.push(&ffi_type_uint32);
                        paddingAmount -= sizeof(u32);
                    } else if (paddingAmount >= sizeof(u16)) {
                        m_type->m_ffiElems.push(&ffi_type_uint16);
                        paddingAmount -= sizeof(u16);
                    } else if (paddingAmount >= sizeof(u8)) {
                        m_type->m_ffiElems.push(&ffi_type_uint8);
                        paddingAmount -= sizeof(u8);
                    }
                }
            }

            u16 size = p->type->getInfo().size;
            totalSize += size;
            expectedNextOffset = p->offset + size;
            m_type->m_ffiElems.push(p->type->getFFI());
        }

        if (m_type->getInfo().size > totalSize) {
            u32 remainingSize = m_type->getInfo().size - totalSize;
            while (remainingSize > 0) {
                if (remainingSize >= sizeof(u64)) {
                    m_type->m_ffiElems.push(&ffi_type_uint64);
                    remainingSize -= sizeof(u64);
                } else if (remainingSize >= sizeof(u32)) {
                    m_type->m_ffiElems.push(&ffi_type_uint32);
                    remainingSize -= sizeof(u32);
                } else if (remainingSize >= sizeof(u16)) {
                    m_type->m_ffiElems.push(&ffi_type_uint16);
                    remainingSize -= sizeof(u16);
                } else if (remainingSize >= sizeof(u8)) {
                    m_type->m_ffiElems.push(&ffi_type_uint8);
                    remainingSize -= sizeof(u8);
                }
            }
        }

        m_type->m_ffiElems.push(nullptr);
        
        m_type->m_ffi.size = m_type->m_ffi.alignment = 0;
        m_type->m_ffi.type = FFI_TYPE_STRUCT;
        m_type->m_ffi.elements = m_type->m_ffiElems.data();
    }

    ITypeBuilder::operator DataType*() const {
        return m_type;
    }

    DataType* ITypeBuilder::getType() const {
        return m_type;
    }
    
    type_meta& ITypeBuilder::getMeta() const {
        return m_type->m_info;
    }
};