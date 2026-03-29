#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <mutex>

#include "../common/visitor/ast_visitor.hpp"
#include "../common/abstract-syntax-tree/ast_program.hpp"
#include "../common/abstract-syntax-tree/ast_include_dir.hpp"
#include "../common/abstract-syntax-tree/ast_function.hpp"
#include "../common/abstract-syntax-tree/ast_parameter.hpp"
#include "../common/abstract-syntax-tree/ast_variable_decl_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_assign_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_compound_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_for_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_function_call_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_if_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_return_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_while_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_dowhile_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_switch_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_case_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_default_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_switch_block_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_binary_expr.hpp"
#include "../common/abstract-syntax-tree/ast_function_call_expr.hpp"
#include "../common/abstract-syntax-tree/ast_id_expr.hpp"
#include "../common/abstract-syntax-tree/ast_literal_expr.hpp"
#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "defs/analyzer_defs.hpp"
#include "../thread-pool/thread_pool.hpp"

/**
 * @class Analyzer
 * @brief performs semantic analysis of the ast
*/
class Analyzer : public ASTVisitor {
public:
    /**
     * @brief creates the instance of the analyzer
     * @param scopeManager - reference to the scope manager
     * @param threadPool - reference to a thread pool
    */
    Analyzer(ScopeManager& scopeManager, ThreadPool& threadPool);

    /**
     * @brief performs semantic analysis of the program
     * @param program - pointer to the program
    */
    void visit(ASTProgram* program) override;

    /**
     * @brief performs semantic analysis of the include directive
     * @param includeDir - pointer to the include directive
    */
    void visit(ASTIncludeDir* includeDir) override;

    /**
     * @brief performs semantic analysis of the function
     * @param function - pointer to the function
    */
    void visit(ASTFunction* function) override;

    /**
     * @brief performs semantic analysis of the parameter
     * @param parameter - pointer to the parameter
    */
    void visit(ASTParameter* parameter) override;

    /**
     * @brief performs semantic analysis of the variable declaration
     * @param variableDecl - pointer to the variable declaration
    */
    void visit(ASTVariableDeclStmt* variableDecl) override;

    /**
     * @brief performs semantic analysis of the assignment statement
     * @param assignStmt - pointer to the assignment statement
    */
    void visit(ASTAssignStmt* assignStmt) override;

    /**
     * @brief performs semantic analysis of the compound statement
     * @param compoundStmt - pointer to the compound statement
    */
    void visit(ASTCompoundStmt* compoundStmt) override;

    /**
     * @brief performs semantic analysis of the for statement
     * @param forStmt - pointer to the for statement
    */
    void visit(ASTForStmt* forStmt) override;

    /**
     * @brief performs semantic analysis of the function call statement
     * @param callStmt - pointer to the function call statement
    */
    void visit(ASTFunctionCallStmt* callStmt) override;

    /**
     * @brief performs semantic analysis of the if statement
     * @param ifStmt - pointer to the if statement
    */
    void visit(ASTIfStmt* ifStmt) override;

    /**
     * @brief performs semantic analysis of the return statement
     * @param returnStmt - pointer to the return statement
    */
    void visit(ASTReturnStmt* returnStmt) override;

    /**
     * @brief performs semantic analysis of the while statement
     * @param whileStmt - pointer to the while statement
    */
    void visit(ASTWhileStmt* whileStmt) override;

    /**
     * @brief performs semantic analysis of the do-while statement
     * @param dowhileStmt - pointer to the do-while statement
    */
    void visit(ASTDoWhileStmt* dowhileStmt) override;
    
    /**
     * @brief performs semantic analysis of the switch statement
     * @param switchStmt - pointer to the switch statement
    */
    void visit(ASTSwitchStmt* switchStmt) override;

    /**
     * @brief performs semantic analysis of the case statement
     * @param caseStmt - pointer to the case statement
    */
    void visit(ASTCaseStmt* caseStmt) override;

    /**
     * @brief performs semantic analysis of the default statement
     * @param defaultStmt - pointer to the default statement
    */
    void visit(ASTDefaultStmt* defaultStmt) override;
    
    /**
     * @brief performs semantic analysis of the switch-block statement
     * @param switchBlockStmt - pointer to the switch-block statement
    */
    void visit(ASTSwitchBlockStmt* switchBlockStmt) override;

    /**
     * @brief performs semantic analysis of the binary expression
     * @param binaryExpr - pointer to the binary expression
    */
    void visit(ASTBinaryExpr* binaryExpr) override;

    /**
     * @brief performs semantic analysis of the function call expression
     * @param callExpr - pointer to the function call expression
    */
    void visit(ASTFunctionCallExpr* callExpr) override;

    /**
     * @brief performs semantic analysis of the id expression
     * @param idExpr - pointer to the id expression
    */
    void visit(ASTIdExpr* idExpr) override;

    /**
     * @brief performs semantic analysis of the literal expression
     * @param literalExpr - pointer to the literal expression
    */
    void visit(ASTLiteralExpr* literalExpr) override;

    /**
     * @brief checks if program has semantic errors or not
     * @param program - const pointer to the program
     * @returns true if any error is caught, false otherwise
    */
    bool hasSemanticErrors(const ASTProgram* program) const noexcept;

    /**
     * @brief merges semantic errors into a single message
     * @param program - const pointer to the program
     * @returns formatted errors
    */
    std::string getSemanticErrors(const ASTProgram* program) const noexcept;

    /**
     * @brief performs semantic analysis of the signature of the function
     * @param function - const pointer to the function
    */
    void checkFunctionSignature(const ASTFunction* function);

    /**
     * @brief defines parameters in the function's scope
     * @param function - const pointer to the function
    */
    void defineParameters(const ASTFunction* function);

private:
    /// mutex for semantic errors
    std::mutex errorMtx;

    /// thread pool for parallel function code analysis
    ThreadPool& threadPool;

    /**
     * @brief helper for literal expr validation
     * @param type - type of the literal
     * @param value - value of the literal
    */
    bool isInvalidLiteral(Type type, const std::string& value) const;

protected:
    /// thread local context of the function
    static thread_local AnalyzerThreadContext analyzerContext;

    /// scope manager containing function declarations
    ScopeManager& globalScopeManager;

    /// maps errors to the function or global scope
    std::unordered_map<std::string, std::vector<std::string>> semanticErrors;

    /// global error label
    static constexpr const char* globalError{ "__global" };

};

#endif