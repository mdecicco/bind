#include <bind/interfaces/ISymbol.h>

namespace bind {
    class DataType;

    class ValuePointer : public ISymbol {
        public:
            ValuePointer(const String& name, DataType* type, void* address, Namespace* ns);
            ~ValuePointer();

            DataType* getType() const;
            void* getAddress() const;

        protected:
            DataType* m_type;
            void* m_address;
    };
};