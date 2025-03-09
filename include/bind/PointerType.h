#pragma once
#include <bind/DataType.h>

namespace bind {
    class PointerType : public DataType {
        public:
            PointerType(DataType* pointsTo);
            ~PointerType();

            DataType* getDestinationType() const;
        
        protected:
            friend class Registry;

            DataType* m_destTp;
            DataType* m_baseType;
    };
};