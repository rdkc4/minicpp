#include "optimization.hpp"

#include "../common/intermediate-representation-tree/IRFunction.hpp"
#include "../common/intermediate-representation-tree/IRVariable.hpp"
#include "../common/intermediate-representation-tree/IRCompoundSt.hpp"
#include "../common/intermediate-representation-tree/IRIfSt.hpp"
#include "../common/intermediate-representation-tree/IRForSt.hpp"
#include "../common/intermediate-representation-tree/IRWhileSt.hpp"
#include "../common/intermediate-representation-tree/IRDoWhileSt.hpp"
#include "../common/intermediate-representation-tree/IRPrintfSt.hpp"
#include "../common/intermediate-representation-tree/IRAssignSt.hpp"
#include "../common/intermediate-representation-tree/IRReturnSt.hpp"
#include "../common/intermediate-representation-tree/IRSwitchSt.hpp"
#include "../common/intermediate-representation-tree/IRFunctionCall.hpp"

namespace Optimization {
    namespace StackMemory {
        constexpr size_t regSize = 8;

        size_t computeStackMemory(const IRFunction* _function);
        size_t computeStackMemory(const IRTemporary* _temporary);
        size_t computeStackMemory(const IRStatement* _stmt);
        size_t computeStackMemory(const IRVariable* _variable);
        size_t computeStackMemory(const IRCompoundSt* _compound);
        size_t computeStackMemory(const IRIfSt* _if);
        size_t computeStackMemory(const IRForSt* _for);
        size_t computeStackMemory(const IRWhileSt* _while);
        size_t computeStackMemory(const IRDoWhileSt* _dowhile);
        size_t computeStackMemory(const IRPrintfSt* _printf);
        size_t computeStackMemory(const IRAssignSt* _assign);
        size_t computeStackMemory(const IRReturnSt* _return);
        size_t computeStackMemory(const IRSwitchSt* _switch);
        size_t computeStackMemory(const IRSwitchBlock* _swBlock);
        size_t computeStackMemory(const IRFunctionCall* _functionCall);
        size_t computeStackMemory([[maybe_unused]] const IRNode* _node);
    };
};