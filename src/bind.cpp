#include <bind/bind.h>

namespace bind {
    AliasType* alias(const String& name, DataType* aliasOf) {
        AliasType* tp = new AliasType(name, aliasOf, nullptr);
        Registry::Add(tp);
        return tp;
    }
};