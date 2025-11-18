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

/**
 * @class StatementIntermediateRepresentation
 * @brief turns ast statement into irt statement
*/
class StatementIntermediateRepresentation {
public:
    StatementIntermediateRepresentation();

    /**
     * @brief turns ast statement into irt statement
     * @param _statement - const pointer to the ast statement
     * @returns pointer to the irt statement
    */
    std::unique_ptr<IRStatement> statement(const ASTStatement* _statement);
    
protected:
    ExpressionIntermediateRepresentation expIR;

    /**
     * @brief turns ast variable declaration into irt variable declaration
     * @param _variable - const pointer to the ast variable declaration
     * @returns irt pointer to the variable declaratiom
    */
    std::unique_ptr<IRVariable> variable(const ASTVariable* _variable);

    /**
     * @brief turns ast printf statement into irt printf statement
     * @param _printf - const pointer to the ast printf statement
     * @returns pointer to the irt printf statement
    */
    std::unique_ptr<IRPrintfSt> printfStatement(const ASTPrintfSt* _printf);

    /**
     * @brief turns ast if-statement into irt if-statement
     * @param _if - const pointer to the ast if-statement
     * @returns pointer to the irt if-statement
    */
    std::unique_ptr<IRIfSt> ifStatement(const ASTIfSt* _if);

    /**
     * @brief turns ast compound statement into irt compound statement
     * @param _compound - const pointer to the ast compound statement
     * @returns pointer to the irt compound statement
    */
    std::unique_ptr<IRCompoundSt> compoundStatement(const ASTCompoundSt* _compound);

    /**
     * @brief turns ast assignment statement into irt assignment statement
     * @param _assignment - const pointer to the ast assignment statement
     * @returns pointer to the irt statement
    */
    std::unique_ptr<IRAssignSt> assignmentStatement(const ASTAssignSt* _assignment);

    /**
     * @brief turns ast return statement into irt return statement
     * @param _return - const pointer to the ast return statement
     * @returns pointer to the irt return statement
    */
    std::unique_ptr<IRReturnSt> returnStatement(const ASTReturnSt* _return);

    /**
     * @brief turns ast while-statement into irt while-statement
     * @param _while - const pointer to the ast while-statement
     * @returns pointer to the irt while-statement 
    */
    std::unique_ptr<IRWhileSt> whileStatement(const ASTWhileSt* _while);

    /**
     * @brief turns ast for-statement into irt for-statement
     * @param _for - const pointer to the ast for-statement
     * @returns pointer to the irt for-statement
    */
    std::unique_ptr<IRForSt> forStatement(const ASTForSt* _for);

    /**
     * @brief turns ast do-while statement into irt do-while statement
     * @param _dowhile - const pointer to the ast do-while statement
     * @returns pointer to the irt do-while statement
    */
    std::unique_ptr<IRDoWhileSt> doWhileStatement(const ASTDoWhileSt* _dowhile);

    /**
     * @brief turns ast switch-statement into irt switch-statement
     * @param _switch - const pointer to the ast switch-statement
     * @returns pointer to the irt switch-statement
    */
    std::unique_ptr<IRSwitchSt> switchStatement(const ASTSwitchSt* _switch);

    /**
     * @brief turns ast case into irt case
     * @param _astCase - const pointer to the ast case
     * @returns pointer to the irt case
    */
    std::unique_ptr<IRCaseSt> _case(const ASTCaseSt* _astCase);

    /**
     * @brief turns ast default case into irt default case
     * @param _astDefault - const pointer to the ast default case
     * @returns pointer to the irt default case
    */
    std::unique_ptr<IRDefaultSt> _default(const ASTDefaultSt* _astDefault);

    /**
     * @brief turns ast switch-block into irt switch-block
     * @param _block - const pointer to the ast switch-block
     * @returns pointer to the irt switch-block
    */
    std::unique_ptr<IRSwitchBlock> switchBlock(const ASTSwitchBlock* _block);
};

#endif
