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
#include "ctx/code_generator_ctx.hpp"

/** 
 * @class StatementCodeGenerator
 * @brief code generator specialized for statements
*/
class StatementCodeGenerator {
public:
    /** 
     * @brief Creates the instance of the code generator specialized for statements
     * @param context - reference to a context of the function
    */
    StatementCodeGenerator(CodeGeneratorFunctionContext& context);

    /**
     * @brief generates asm code for the statement
     * @param stmt - const pointer to the irt statement
    */
    void generateStmt(const ir::IRStmt* stmt);

    /** 
     * @brief generates asm code for the variable declaration
     * @param variableDecl - const pointer to the irt variable declaration
    */
    void generateVariableDeclStmt(const ir::IRVariableDeclStmt* variableDecl);

    /** 
     * @brief generates asm code for the if-statement
     * @param ifStmt - const pointer to the irt if-statement
    */
    void generateIfStmt(const ir::IRIfStmt* ifStmt);

    /** 
     * @brief generates asm code for the while-statement
     * @param whileStmt - const pointer to the irt while-statement
    */
    void generateWhileStmt(const ir::IRWhileStmt* whileStmt);

    /** 
     * @brief generates asm code for the for-statement
     * @param forStmt - const pointer to the irt for-statement
    */
    void generateForStmt(const ir::IRForStmt* forStmt);

    /** 
     * @brief generates asm code for the do-while statement
     * @param dowhileStmt - const pointer to the irt do-while statement
    */
    void generateDoWhileStmt(const ir::IRDoWhileStmt* dowhileStmt);

    /** 
     * @brief generates asm code for the compound statement
     * @param compoundStmt - const pointer to the irt compound statement
    */
    void generateCompoundStmt(const ir::IRCompoundStmt* compoundStmt);

    /** 
     * @brief generates asm code for the assignment statement
     * @param assignStmt - const pointer to the irt assignment statement
    */
    void generateAssignStmt(const ir::IRAssignStmt* assignStmt);

    /** 
     * @brief generates asm code for the return statement
     * @param returnStmt - const pointer to the irt return statement
    */
    void generateReturnStmt(const ir::IRReturnStmt* returnStmt);

    /** 
     * @brief generates asm code for the function-call statement
     * @param callStmt - const pointer to the irt function-call statement
    */
    void generateFunctionCallStmt(const ir::IRFunctionCallStmt* callStmt);

    /** 
     * @brief generates asm code for the switch-statement
     * @param switchStmt - const pointer to the irt switch-statement
    */
    void generateSwitchStmt(const ir::IRSwitchStmt* switchStmt);

private:
    /// reference to a context of the function
    CodeGeneratorFunctionContext& ctx;

    /// code generator specialized for expressions
    ExpressionCodeGenerator exprGenerator;

};

#endif