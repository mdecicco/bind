#pragma once
#include <bind/FuncMatch.h>
#include <bind/interfaces/ISymbol.h>
#include <bind/types.h>
#include <utils/Array.h>
#include <utils/Pointer.h>
#include <utils/String.h>
#include <utils/interfaces/IWithUserData.h>

#ifndef BIND_DATATYPE_USERDATA_SIZE
#define BIND_DATATYPE_USERDATA_SIZE 32
#endif

#define FFI_STATIC_BUILD
#define FFI_BUILDING
#include <ffi.h>

namespace bind {
    class Function;
    class PointerType;

    class DataType : public IWithFixedUserData<BIND_DATATYPE_USERDATA_SIZE>, public ISymbol {
        public:
            struct Property : public IWithFixedUserData<32> {
                public:
                    struct Flags {
                        public:
                            unsigned can_read : 1;
                            unsigned can_write : 1;
                            unsigned is_static : 1;
                            unsigned is_method : 1;
                            unsigned is_pseudo_method : 1;
                            unsigned is_ctor : 1;
                            unsigned is_dtor : 1;
                            unsigned is_inherited : 1;
                    };

                    Property(
                        i32 offset,
                        Flags flags,
                        DataType* type,
                        const String& name,
                        AccessFlags accessFlags = PublicAccess
                    );
                    Property(
                        const Pointer& address,
                        Flags flags,
                        DataType* type,
                        const String& name,
                        AccessFlags accessFlags = PublicAccess
                    );
                    Property(const Property& prop);

                    /**
                     * @brief Byte offset of this property in the type structure. If this
                     * property is static, or if it refers to a method, the value will be
                     * -1.
                     */
                    i32 offset;

                    /**
                     * @brief If this is a static property this will point to the value
                     * in memory. If this is a method/ctor/dtor property this will be a
                     * pointer to a `Function` object. If this is a normal property this
                     * will be a null pointer.
                     */
                    Pointer address;

                    /**
                     * @brief If this is a method, this will be an offset that must be
                     * applied to the `this` pointer that gets passed to the method.
                     *
                     * In most cases this will be 0, but if this is a method that was
                     * inherited from a base type, this will be the offset of the base
                     * type in the memory layout of this type.
                     */
                    u32 thisOffset;

                    /**
                     * @brief Info about this property
                     */
                    Flags flags;

                    /**
                     * @brief User defined access flags
                     */
                    AccessFlags accessFlags;

                    /**
                     * @brief The data type of this property. If this is a method/ctor/dtor
                     * property the data type will be a `FunctionType*`.
                     */
                    DataType* type;

                    /**
                     * @brief Name of the property
                     */
                    String name;
            };

            struct BaseType {
                public:
                    /**
                     * @brief The base type
                     */
                    DataType* type;

                    /**
                     * @brief The offset of the base type in this type's memory layout
                     */
                    u32 offset;
            };

            DataType(const String& name, const type_meta& meta, Namespace* ns);
            DataType(const String& name, const String& fullName, const type_meta& meta, Namespace* ns);
            virtual ~DataType();

            /**
             * @brief Returns the type metadata
             */
            const type_meta& getInfo() const;

            /**
             * @brief Returns the namespace that contains this type's methods, constructor,
             * destructor, and static properties
             */
            Namespace* getOwnNamespace() const;

            /**
             * @brief Returns the array of base types for this type
             */
            const Array<BaseType>& getBases() const;

            /**
             * @brief Casts a pointer to this type as a pointer to the specified base type
             *
             * @param baseType The base type to cast to
             * @return A pointer to the base type
             */
            void* castPointerAsBaseType(void* ptr, const DataType* baseType) const;

            /**
             * @brief Returns the metadata for the base with the specified type,
             * if it exists
             *
             * @param baseType The base type to search for
             * @return The metadata for the base type, or nullptr if it does not exist
             */
            const BaseType* getBaseInfo(const DataType* baseType) const;

            /**
             * @brief Returns a reference to this type's array of properties
             */
            const Array<Property>& getProps() const;

            /**
             * @brief Returns an array of all properties that have the specified name which
             * are accessible with the given access mask
             *
             * @param name Property name to search for
             * @param accessMask Access rights to filter the search results with
             * @return Array of properties that match the search query
             */
            Array<const Property*> findProperties(const String& name, AccessFlags accessMask = FullAccessRights) const;

            /**
             * @brief Returns an array of methods (static or not) that match the search query
             *
             * @param search Search query info
             * @param singleStrictMatch If the query found only one method that perfectly
             * matches the search query then this output parameter will be set to that function.
             *
             * @return Array of methods (static or not) that match the search query
             */
            Array<Function*> findMethods(const FuncMatch& search, Function** singleStrictMatch = nullptr) const;

            /**
             * @brief Returns an array of constructors with matching arguments
             *
             * @param argTypes Argument types to match with
             * @param strict Whether or not to require strict argument type matches, if `false`
             * then constructors will match if the specified arguments are either strictly equal
             * to or convertible to a constructor's arguments
             * @param accessMask Access rights to filter the search results to
             * @param singleStrictMatch If the query found only one method that perfectly
             * matches the search query then this output parameter will be set to that function.
             *
             * @return Array of constructors that match the search query
             */
            Array<Function*> findConstructors(
                const Array<DataType*>& argTypes,
                bool strict                  = false,
                AccessFlags accessMask       = FullAccessRights,
                Function** singleStrictMatch = nullptr
            ) const;

            /**
             * @brief Returns an array of constructors
             *
             * @param accessMask Access rights to filter the constructors by
             *
             * @return Array of constructors
             */
            Array<Function*> getConstructors(AccessFlags accessMask = FullAccessRights) const;

            /**
             * @brief Returns the destructor function for this type, if one is defined
             *
             * @param accessMask Access rights
             *
             * @return Destructor function, or nullptr if no destructor is defined
             */
            Function* getDestructor(AccessFlags accessMask = FullAccessRights);

            /**
             * @brief Returns a conversion operator that converts from this type to the
             * specified type
             *
             * @param resultType Data type to convert to
             * @param accessMask Access rights to filter the search with
             *
             * @return Conversion operator function, if one is found
             */
            Function* findConversionOperator(DataType* resultType, AccessFlags accessMask = FullAccessRights) const;

            /**
             * @brief Checks if this type is convertible to some data type. A data type
             * is convertible to another data type if one of the following is true:
             *
             * - Both this type and the other type are primitives or pointers
             *
             * - This type is equal to the other type
             *
             * - This type has a cast operator override that returns the other type
             *
             * - The other type has a constructor which takes exactly one parameter,
             *   and that parameter's type is this type
             *
             * - The other type is trivially copyable, fully defined, and every member
             * property of the other type has a counterpart on this type which has the same
             * name and has a type which can be converted to the destination property's type
             *
             * @return Returns true if this type is convertible to the other type
             */
            virtual bool isConvertibleTo(DataType* to, AccessFlags accessMask = FullAccessRights) const;

            /**
             * @brief Checks if this type is equal to some data type. A data type is equal to
             * another data type if both of their effective types have the same ID.
             *
             * @return Returns true if both data types are the same
             */
            bool isEqualTo(DataType* to) const;

            /**
             * @brief Checks if this type is able to be constructed with the provided argument types
             *
             * @return Returns true if this type is constructable with the provided argument types
             */
            bool isConstructableWith(
                const Array<DataType*>& args, AccessFlags accessMask = FullAccessRights, bool strict = false
            ) const;

            /**
             * @brief Follows any chain of aliases to get the data type being referred to.
             *
             * @return The effective data type that this data type refers to, if this data
             *         type is an alias. Otherwise this function returns this data type.
             */
            DataType* getEffectiveType() const;

            /**
             * @brief Get a pointer type that points to this type
             */
            PointerType* getPointerType();

            /**
             * @brief Gets a pointer to this type's ffi_type, for use with libffi
             */
            ffi_type* getFFI();

        protected:
            friend class ITypeBuilder;

            type_meta m_info;
            Namespace* m_ownNamespace;
            PointerType* m_pointerToSelf;
            Array<Property> m_props;
            Array<BaseType> m_bases;
            ffi_type m_ffi;
            Array<ffi_type*> m_ffiElems;
    };
};