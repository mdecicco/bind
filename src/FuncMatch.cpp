#include <bind/FuncMatch.h>
#include <utils/Array.hpp>

namespace bind {
    FuncMatch::FuncMatch(const String& _name)
        : name(_name), returnType(nullptr), accessMask(FullAccessRights),
          returnTypeStrict(false), argTypesStrict(false), doCheckArgs(false)
    {
    }

    FuncMatch& FuncMatch::retTp(DataType* tp, bool strict) {
        returnType = tp;
        returnTypeStrict = strict;
        return *this;
    }

    FuncMatch& FuncMatch::argTps(const Array<DataType*>& types, bool strict) {
        argTypes = types;
        argTypesStrict = strict;
        doCheckArgs = true;
        return *this;
    }
    
    FuncMatch& FuncMatch::noArgs() {
        doCheckArgs = true;
        return *this;
    }

    FuncMatch& FuncMatch::access(AccessFlags mask) {
        accessMask = mask;
        return *this;
    }
};