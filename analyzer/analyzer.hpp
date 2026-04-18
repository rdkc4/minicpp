#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include <format>
#include <unordered_map>
#include <string>
#include <string_view>
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
#include "ctx/analyzer_ctx.hpp"
#include "../thread-pool/thread_pool.hpp"

/**
 * @namespace semantic
 * @brief module defining the elements related to semantic of the language
*/
namespace semantic {
    /**
     * @class Analyzer
     * @brief performs semantic analysis of the ast
    */
    class Analyzer : public syntax::ast::ASTVisitor {
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
        void visit(syntax::ast::ASTProgram* program) override;

        /**
         * @brief performs semantic analysis of the include directive
         * @param includeDir - pointer to the include directive
        */
        void visit(syntax::ast::ASTIncludeDir* includeDir) override;

        /**
         * @brief performs semantic analysis of the function
         * @param function - pointer to the function
        */
        void visit(syntax::ast::ASTFunction* function) override;

        /**
         * @brief performs semantic analysis of the parameter
         * @param parameter - pointer to the parameter
        */
        void visit(syntax::ast::ASTParameter* parameter) override;

        /**
         * @brief performs semantic analysis of the variable declaration
         * @param variableDecl - pointer to the variable declaration
        */
        void visit(syntax::ast::ASTVariableDeclStmt* variableDecl) override;

        /**
         * @brief performs semantic analysis of the assignment statement
         * @param assignStmt - pointer to the assignment statement
        */
        void visit(syntax::ast::ASTAssignStmt* assignStmt) override;

        /**
         * @brief performs semantic analysis of the compound statement
         * @param compoundStmt - pointer to the compound statement
        */
        void visit(syntax::ast::ASTCompoundStmt* compoundStmt) override;

        /**
         * @brief performs semantic analysis of the for statement
         * @param forStmt - pointer to the for statement
        */
        void visit(syntax::ast::ASTForStmt* forStmt) override;

        /**
         * @brief performs semantic analysis of the function call statement
         * @param callStmt - pointer to the function call statement
        */
        void visit(syntax::ast::ASTFunctionCallStmt* callStmt) override;

        /**
         * @brief performs semantic analysis of the if statement
         * @param ifStmt - pointer to the if statement
        */
        void visit(syntax::ast::ASTIfStmt* ifStmt) override;

        /**
         * @brief performs semantic analysis of the return statement
         * @param returnStmt - pointer to the return statement
        */
        void visit(syntax::ast::ASTReturnStmt* returnStmt) override;

        /**
         * @brief performs semantic analysis of the while statement
         * @param whileStmt - pointer to the while statement
        */
        void visit(syntax::ast::ASTWhileStmt* whileStmt) override;

        /**
         * @brief performs semantic analysis of the do-while statement
         * @param dowhileStmt - pointer to the do-while statement
        */
        void visit(syntax::ast::ASTDoWhileStmt* dowhileStmt) override;
        
        /**
         * @brief performs semantic analysis of the switch statement
         * @param switchStmt - pointer to the switch statement
        */
        void visit(syntax::ast::ASTSwitchStmt* switchStmt) override;

        /**
         * @brief performs semantic analysis of the case statement
         * @param caseStmt - pointer to the case statement
        */
        void visit(syntax::ast::ASTCaseStmt* caseStmt) override;

        /**
         * @brief performs semantic analysis of the default statement
         * @param defaultStmt - pointer to the default statement
        */
        void visit(syntax::ast::ASTDefaultStmt* defaultStmt) override;
        
        /**
         * @brief performs semantic analysis of the switch-block statement
         * @param switchBlockStmt - pointer to the switch-block statement
        */
        void visit(syntax::ast::ASTSwitchBlockStmt* switchBlockStmt) override;

        /**
         * @brief performs semantic analysis of the binary expression
         * @param binaryExpr - pointer to the binary expression
        */
        void visit(syntax::ast::ASTBinaryExpr* binaryExpr) override;

        /**
         * @brief performs semantic analysis of the function call expression
         * @param callExpr - pointer to the function call expression
        */
        void visit(syntax::ast::ASTFunctionCallExpr* callExpr) override;

        /**
         * @brief performs semantic analysis of the id expression
         * @param idExpr - pointer to the id expression
        */
        void visit(syntax::ast::ASTIdExpr* idExpr) override;

        /**
         * @brief performs semantic analysis of the literal expression
         * @param literalExpr - pointer to the literal expression
        */
        void visit(syntax::ast::ASTLiteralExpr* literalExpr) override;

        /**
         * @brief checks if program has semantic errors or not
         * @param program - const pointer to the program
         * @returns true if any error is caught, false otherwise
        */
        bool hasSemanticErrors(const syntax::ast::ASTProgram* program) const noexcept;

        /**
         * @brief merges semantic errors into a single message
         * @param program - const pointer to the program
         * @returns formatted errors
        */
        std::string getSemanticErrors(const syntax::ast::ASTProgram* program) const noexcept;

        /**
         * @brief performs semantic analysis of the signature of the function
         * @param function - const pointer to the function
        */
        void checkFunctionSignature(const syntax::ast::ASTFunction* function);

        /**
         * @brief defines parameters in the function's scope
         * @param function - const pointer to the function
        */
        void defineParameters(const syntax::ast::ASTFunction* function);

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

        /**
         * @brief reports new semantic error
         * @param token - const reference to a token that caused error
         * @param message - error message
        */
        inline void reportError(const syntax::Token& token, std::string_view message){
            analyzerContext.semanticErrors.emplace_back(
                formattedError(token, message)
            );
        }

        /**
         * @brief reports new semantic error
         * @param token - const reference to a token that caused error
         * @param message - error message
         * @param errorIdentifier - global error or name of the function where error occured
        */
        inline void reportError(const syntax::Token& token, std::string_view message, const std::string& errorIdentifier){
            semanticErrors[errorIdentifier].emplace_back(
                formattedError(token, message)
            );
        }

        /**
         * @brief generates formatted error
         * @param token - const reference to a token, for obtaining line and column where error occured
         * @param message - error message
        */
        inline std::string formattedError(const syntax::Token& token, std::string_view message){
            return std::format(
                "Line {}, Column {}: SEMANTIC ERROR -> {}",
                token.line, token.column, message
            );
        }

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

}

#endif