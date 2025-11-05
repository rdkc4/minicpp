#ifndef OPTIMIZATION_DEAD_CODE_HPP
#define OPTIMIZATION_DEAD_CODE_HPP

#include "../common/intermediate-representation-tree/IRFunction.hpp"
#include "../common/intermediate-representation-tree/IRCompoundSt.hpp"
#include "../common/intermediate-representation-tree/IRReturnSt.hpp"
#include "../common/intermediate-representation-tree/IRIfSt.hpp"
#include "../common/intermediate-representation-tree/IRDoWhileSt.hpp"
#include "../common/intermediate-representation-tree/IRSwitchSt.hpp"

namespace Optimization {
    namespace DeadCode {
        void eliminateDeadCode(IRFunction* _function);
        bool eliminateDeadCode(IRStatement* _stmt);
        bool eliminateDeadCode(IRCompoundSt* _compound);
        bool eliminateDeadCode([[maybe_unused]] IRReturnSt* _return);
        bool eliminateDeadCode(IRIfSt* _if);
        bool eliminateDeadCode(IRDoWhileSt* _dowhile);
        bool eliminateDeadCode(IRSwitchSt* _switch);
        bool eliminateDeadCode(IRCaseSt* _case);
        bool eliminateDeadCode(IRDefaultSt* _default);
        bool eliminateDeadCode(IRSwitchBlock* _swBlock); 
        bool eliminateDeadCode([[maybe_unused]] IRNode* _node);
    };
};

#endif