#pragma once
#include <bind/interfaces/ISymbol.h>
#include <bind/util/EnumTypeBuilder.hpp>
#include <bind/util/ObjectTypeBuilder.hpp>
#include <bind/util/PrimitiveTypeBuilder.hpp>
#include <unordered_map>
#include <utils/String.h>
#include <utils/interfaces/IWithUserData.h>

namespace bind {
    class AliasType;

    class Namespace : public IWithFixedUserData<32>, public ISymbol {
        public:
            Namespace(const String& name);
            Namespace(Namespace* parent, const String& name);
            Namespace(DataType* type);
            Namespace(Namespace* parent, DataType* type);

            template <typename T>
            std::enable_if_t<is_type_valid_opaque_v<T>, DataType*> opaqueType(const String& name);

            template <typename T>
            std::enable_if_t<std::is_class_v<T>, ObjectTypeBuilder<T>> type(const String& name);

            template <typename T>
            std::enable_if_t<std::is_class_v<T>, ObjectTypeBuilder<T>> extend();

            template <typename T>
            std::enable_if_t<std::is_fundamental_v<T>, PrimitiveTypeBuilder<T>> type(const String& name);

            template <typename T>
            std::enable_if_t<std::is_fundamental_v<T>, PrimitiveTypeBuilder<T>> extend();

            template <typename T>
            std::enable_if_t<std::is_enum_v<T>, EnumTypeBuilder<T>> type(const String& name);

            template <typename T>
            std::enable_if_t<std::is_enum_v<T>, EnumTypeBuilder<T>> extend();

            template <typename T>
            ValuePointer* value(const String& name, T* val);

            template <typename Ret, typename... Args>
            Function* function(const String& name, Ret (*fn)(Args...));

            template <typename T>
            AliasType* alias(const String& name);

            AliasType* alias(const String& name, DataType* aliasOf);

            Namespace* getParent() const;
            DataType* getCorrespondingType() const;
            ISymbol* findSymbol(const String& symbolName) const;
            void findSymbols(const String& name, Array<ISymbol*>& outResults) const;
            const Array<ISymbol*>& getSymbols() const;
            void add(ISymbol* sym);
            void remove(ISymbol* sym);

        protected:
            Namespace* m_parent;
            DataType* m_forType;
            std::unordered_map<u64, ISymbol*> m_symbolMap;
            Array<ISymbol*> m_symbols;
    };
};