#ifndef STATEMENT_INTERMEDIATE_REPRESENTATION_HPP
#define STATEMENT_INTERMEDIATE_REPRESENTATION_HPP

#include <memory>

#include "../common/abstract-syntax-tree/ASTStatement.hpp"
#include "../common/abstract-syntax-tree/ASTVariable.hpp"
#include "../common/abstract-syntax-tree/ASTPrintfSt.hpp"
#include "../common/abstract-syntax-tree/ASTIfSt.hpp"
#include "../common/abstract-syntax-tree/ASTCompoundSt.hpp"
#include "../common/abstract-syntax-tree/ASTAssignSt.hpp"
#include "../common/abstract-syntax-tree/ASTReturnSt.hpp"
#include "../common/abstract-syntax-tree/ASTWhileSt.hpp"
#include "../common/abstract-syntax-tree/ASTForSt.hpp"
#include "../common/abstract-syntax-tree/ASTDoWhileSt.hpp"
#include "../common/abstract-syntax-tree/ASTSwitchSt.hpp"

#include "../common/intermediate-representation-tree/IRStatement.hpp"
#include "../common/intermediate-representation-tree/IRVariable.hpp"
#include "../common/intermediate-representation-tree/IRPrintfSt.hpp"
#include "../common/intermediate-representation-tree/IRIfSt.hpp"
#include "../common/intermediate-representation-tree/IRCompoundSt.hpp"
#include "../common/intermediate-representation-tree/IRAssignSt.hpp"
#include "../common/intermediate-representation-tree/IRReturnSt.hpp"
#include "../common/intermediate-representation-tree/IRWhileSt.hpp"
#include "../common/intermediate-representation-tree/IRForSt.hpp"
#include "../common/intermediate-representation-tree/IRDoWhileSt.hpp"
#include "../common/intermediate-representation-tree/IRSwitchSt.hpp"
#include "expression_intermediate_representation.hpp"

class StatementIntermediateRepresentation {
public:
    StatementIntermediateRepresentation();
    std::unique_ptr<IRStatement> statement(const ASTStatement* _statement);
    
protected:
    ExpressionIntermediateRepresentation expIR;

    // statements
    std::unique_ptr<IRVariable> variable(const ASTVariable* _variable);
    std::unique_ptr<IRPrintfSt> printfStatement(const ASTPrintfSt* _printf);
    std::unique_ptr<IRIfSt> ifStatement(const ASTIfSt* _if);
    std::unique_ptr<IRCompoundSt> compoundStatement(const ASTCompoundSt* _compound);
    std::unique_ptr<IRAssignSt> assignmentStatement(const ASTAssignSt* _assignment);
    std::unique_ptr<IRReturnSt> returnStatement(const ASTReturnSt* _return);
    std::unique_ptr<IRWhileSt> whileStatement(const ASTWhileSt* _while);
    std::unique_ptr<IRForSt> forStatement(const ASTForSt* _for);
    std::unique_ptr<IRDoWhileSt> doWhileStatement(const ASTDoWhileSt* _dowhile);
    std::unique_ptr<IRSwitchSt> switchStatement(const ASTSwitchSt* _switch);
    std::unique_ptr<IRCaseSt> _case(const ASTCaseSt* _astCase);
    std::unique_ptr<IRDefaultSt> _default(const ASTDefaultSt* _astDefault);
    std::unique_ptr<IRSwitchBlock> switchBlock(const ASTSwitchBlock* _block);
};

#endif