#ifndef STATEMENT_CODE_GENERATOR_HPP
#define STATEMENT_CODE_GENERATOR_HPP

#include "../../common/intermediate-representation-tree/ir_stmt.hpp"
#include "../../common/intermediate-representation-tree/ir_variable_decl_stmt.hpp"
#include "../../common/intermediate-representation-tree/ir_if_stmt.hpp"
#include "../../common/intermediate-representation-tree/ir_while_stmt.hpp"
#include "../../common/intermediate-representation-tree/ir_for_stmt.hpp"
#include "../../common/intermediate-representation-tree/ir_dowhile_stmt.hpp"
#include "../../common/intermediate-representation-tree/ir_compound_stmt.hpp"
#include "../../common/intermediate-representation-tree/ir_assign_stmt.hpp"
#include "../../common/intermediate-representation-tree/ir_return_stmt.hpp"
#include "../../common/intermediate-representation-tree/ir_function_call_stmt.hpp"
#include "../../common/intermediate-representation-tree/ir_switch_stmt.hpp"
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
    */
    void generateStatement(const IRStmt* _statement);

    /** 
     * @brief generates asm code for the variable declaration
     * @param _variable - const pointer to the irt variable declaration
    */
    void generateVariable(const IRVariableDeclStmt* _variable);

    /** 
     * @brief generates asm code for the if-statement
     * @param _if - const pointer to the irt if-statement
    */
    void generateIfStatement(const IRIfStmt* _if);

    /** 
     * @brief generates asm code for the while-statement
     * @param _while - const pointer to the irt while-statement
    */
    void generateWhileStatement(const IRWhileStmt* _while);

    /** 
     * @brief generates asm code for the for-statement
     * @param _for - const pointer to the irt for-statement
    */
    void generateForStatement(const IRForStmt* _for);

    /** 
     * @brief generates asm code for the do-while statement
     * @param _dowhile - const pointer to the irt do-while statement
    */
    void generateDoWhileStatement(const IRDoWhileStmt* _dowhile);

    /** 
     * @brief generates asm code for the compound statement
     * @param _compound - const pointer to the irt compound statement
    */
    void generateCompoundStatement(const IRCompoundStmt* _compound);

    /** 
     * @brief generates asm code for the assignment statement
     * @param _assignment - const pointer to the irt assignment statement
    */
    void generateAssignmentStatement(const IRAssignStmt* _assignment);

    /** 
     * @brief generates asm code for the return statement
     * @param _return - const pointer to the irt return statement
    */
    void generateReturnStatement(const IRReturnStmt* _return);

    /** 
     * @brief generates asm code for the function-call statement
     * @param _call - const pointer to the irt function-call statement
    */
    void generateFunctionCallStatement(const IRFunctionCallStmt* _call);

    /** 
     * @brief generates asm code for the switch-statement
     * @param _switch - const pointer to the irt switch-statement
    */
    void generateSwitchStatement(const IRSwitchStmt* _switch);

private:
    /// code generator specialized for expressions
    ExpressionCodeGenerator exprGenerator;

};

#endif