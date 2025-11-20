#ifndef STATEMENT_CODE_GENERATOR_HPP
#define STATEMENT_CODE_GENERATOR_HPP

#include "../../common/intermediate-representation-tree/IRStatement.hpp"
#include "../../common/intermediate-representation-tree/IRVariable.hpp"
#include "../../common/intermediate-representation-tree/IRPrintfSt.hpp"
#include "../../common/intermediate-representation-tree/IRIfSt.hpp"
#include "../../common/intermediate-representation-tree/IRWhileSt.hpp"
#include "../../common/intermediate-representation-tree/IRForSt.hpp"
#include "../../common/intermediate-representation-tree/IRDoWhileSt.hpp"
#include "../../common/intermediate-representation-tree/IRCompoundSt.hpp"
#include "../../common/intermediate-representation-tree/IRAssignSt.hpp"
#include "../../common/intermediate-representation-tree/IRReturnSt.hpp"
#include "../../common/intermediate-representation-tree/IRSwitchSt.hpp"
#include "expression_code_generator.hpp"

/** 
 * @class StatementCodeGenerator
 * @brief code generator specialized for statements
*/
class StatementCodeGenerator {
public:
    /** 
     * @brief Creates the instance of the code generator specialized for statements
    */
    StatementCodeGenerator();

    /**
     * @brief generates asm code for the statement
     * @param _statement - const pointer to the irt statement
     * @returns void
    */
    void generateStatement(const IRStatement* _statement);

private:
    /// code generator specialized for expressions
    ExpressionCodeGenerator exprGenerator;

    /** 
     * @brief generates asm code for the variable declaration
     * @param _variable - const pointer to the irt variable declaration
     * @returns void
    */
    void generateVariable(const IRVariable* _variable);

    /** 
     * @brief generates asm code for the printf statement
     * @param _printf - const pointer to the irt printf statement
     * @returns void
    */
    void generatePrintfStatement(const IRPrintfSt* _printf);

    /** 
     * @brief generates asm code for the if-statement
     * @param _if - const pointer to the irt if-statement
     * @returns void
    */
    void generateIfStatement(const IRIfSt* _if);

    /** 
     * @brief generates asm code for the while-statement
     * @param _while - const pointer to the irt while-statement
     * @returns void
    */
    void generateWhileStatement(const IRWhileSt* _while);

    /** 
     * @brief generates asm code for the for-statement
     * @param _for - const pointer to the irt for-statement
     * @returns void
    */
    void generateForStatement(const IRForSt* _for);

    /** 
     * @brief generates asm code for the do-while statement
     * @param _dowhile - const pointer to the irt do-while statement
     * @returns void
    */
    void generateDoWhileStatement(const IRDoWhileSt* _dowhile);

    /** 
     * @brief generates asm code for the compound statement
     * @param _compound - const pointer to the irt compound statement
     * @returns void
    */
    void generateCompoundStatement(const IRCompoundSt* _compound);

    /** 
     * @brief generates asm code for the assignment statement
     * @param _assignment - const pointer to the irt assignment statement
     * @returns void
    */
    void generateAssignmentStatement(const IRAssignSt* _assignment);

    /** 
     * @brief generates asm code for the return statement
     * @param _return - const pointer to the irt return statement
     * @returns void
    */
    void generateReturnStatement(const IRReturnSt* _return);

    /** 
     * @brief generates asm code for the switch-statement
     * @param _switch - const pointer to the irt switch-statement
     * @returns void
    */
    void generateSwitchStatement(const IRSwitchSt* _switch);
};

#endif