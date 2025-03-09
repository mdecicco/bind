#pragma once
#include <bind/types.h>
#include <utils/String.h>
#include <utils/Array.h>

namespace bind {
    class DataType;

    class FuncMatch {
        public:
            FuncMatch(const String& name);

            /**
             * @brief Specifies a return type to match with
             * 
             * @param tp Return type to filter results by
             * @param strict Whether or not to require a strict return type match. If `false` then
             * return types that are convertible to `tp` will also match
             * 
             * @return reference to this
             */
            FuncMatch& retTp(DataType* tp, bool strict = true);

            /**
             * @brief Specifies argument types to match with, and implicitly an argument count to
             * match with
             * 
             * @param types Argument types to filter results by
             * @param strict Whether or not to require strict argument type matches. If `false`
             * then argument types that are convertible to the corresponding argument types of
             * potential results will also match
             * 
             * @return reference to this
             */
            FuncMatch& argTps(const Array<DataType*>& types, bool strict = true);

            /**
             * @brief Specifies to filter results to functions with no arguments
             * 
             * @return reference to this
             */
            FuncMatch& noArgs();

            /**
             * @brief Specifies access rights to filter the search results with
             * 
             * @return reference to this
             */
            FuncMatch& access(AccessFlags mask);

            String name;
            DataType* returnType;
            Array<DataType*> argTypes;
            AccessFlags accessMask;
            bool returnTypeStrict;
            bool argTypesStrict;
            bool doCheckArgs;
    };
};