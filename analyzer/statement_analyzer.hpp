#ifndef STATEMENT_ANALYZER_HPP
#define STATEMENT_ANALYZER_HPP

#include <unordered_set>
#include <string>

#include "../common/abstract-syntax-tree/ASTStatement.hpp"
#include "../common/abstract-syntax-tree/ASTVariable.hpp"
#include "../common/abstract-syntax-tree/ASTPrintfSt.hpp"
#include "../common/abstract-syntax-tree/ASTIfSt.hpp"
#include "../common/abstract-syntax-tree/ASTWhileSt.hpp"
#include "../common/abstract-syntax-tree/ASTForSt.hpp"
#include "../common/abstract-syntax-tree/ASTDoWhileSt.hpp"
#include "../common/abstract-syntax-tree/ASTSwitchSt.hpp"
#include "../common/abstract-syntax-tree/ASTCompoundSt.hpp"
#include "../common/abstract-syntax-tree/ASTAssignSt.hpp"
#include "../common/abstract-syntax-tree/ASTReturnSt.hpp"
#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "defs/analyzer_defs.hpp"
#include "expression_analyzer.hpp"

/** 
 * @class StatementAnalyzer
 * @brief semantic analysis of the AST statement
*/
class StatementAnalyzer {
public:
    /** 
     * @brief Creates the instance of the analyzer specialized for statements
     * @param scopeManager - reference to the global scope manager
    */
    StatementAnalyzer(ScopeManager& scopeManager);

    /** 
     * @brief semantic check for any ast statement
     * @param _statement - const pointer to a statement node
     * @returns void
    */
    void checkStatement(const ASTStatement* _statement);

protected:
    /// reference to the global scope manager
    ScopeManager& globalScopeManager;
    /// analyzer specialized for the expression analysis
    ExpressionAnalyzer expressionAnalyzer;

    /** 
     * @brief getter for the context of the function that owns statements
     * @return reference to a context of the analyzer thread
    */
    AnalyzerThreadContext& getContext() noexcept;

    /** 
     * @brief semantic check for variable declaration
     * @param _variable - const pointer to a variable node
     * @returns void
    */
    void checkVariable(const ASTVariable* _variable);

    /** 
     * @brief semantic check for printf statement
     * @param _printf - const pointer to a printf statement node
     * @returns void
    */
    void checkPrintfStatement(const ASTPrintfSt* _printf);

    /** 
     * @brief semantic check for if-statement
     * @param _if - const pointer to an if-statement node
     * @returns void
    */
    void checkIfStatement(const ASTIfSt* _if);

    /** 
     * @brief semantic check for while-statement
     * @param _while - const pointer to a while-statement node
     * @returns void
    */
    void checkWhileStatement(const ASTWhileSt* _while);

    /** 
     * @brief semantic check for for-statement
     * @param _for - const pointer to a for-statement node
     * @returns void
    */
    void checkForStatement(const ASTForSt* _for);

    /** 
     * @brief semantic check for do-while-statement
     * @param _dowhile - const pointer to a do-while-statement node
     * @returns void
    */
    void checkDoWhileStatement(const ASTDoWhileSt* _dowhile);

    /** 
     * @brief semantic check for compound-statement
     * @param _compound - const pointer to a compound-statement node
     * @returns void
    */
    void checkCompoundStatement(const ASTCompoundSt* _compound);

    /** 
     * @brief semantic check for assignment-statement
     * @param _assignment - const pointer to an assignment-statement node
     * @returns void
    */
    void checkAssignmentStatement(const ASTAssignSt* _assignment);

    /** 
     * @brief semantic check for return-statement
     * @param _return - const pointer to a return-statement node
     * @returns void
    */
    void checkReturnStatement(const ASTReturnSt* _return);

    /** 
     * @brief semantic check for switch-statement
     * @param _switch - const pointer to a switch-statement node
     * @returns void
    */
    void checkSwitchStatement(const ASTSwitchSt* _switch);

    /** 
     * @brief semantic check for the cases of the switch-statement
     * @param _switch - const pointer to a switch-statement node
     * @returns void
    */
    template<typename T>
    void checkSwitchStatementCases(const ASTSwitchSt* _switch){
        auto& analyzerContext = getContext();
        std::unordered_set<T> set;
        Types expectedType{ std::is_same<T, int>::value ? Types::INT : Types::UNSIGNED };

        for(const auto& _case : _switch->getCases()){
            expressionAnalyzer.checkLiteral(_case->getLiteral());

            T val;
            // case type check
            Types type{ _case->getLiteral()->getType() };
            if(type != expectedType){
                analyzerContext.semanticErrors.push_back(
                    std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid case - type mismatch: expected '{}', got '{}'", 
                        _case->getToken().line, _case->getToken().column, typeToString.at(expectedType), typeToString.at(type))
                );
            }
            // case duplicate check
            else if(set.find(val = (std::is_same<T, int>::value ? std::stoi(_case->getLiteral()->getToken().value) 
                                                                : std::stoul(_case->getLiteral()->getToken().value))) != set.end()){
                analyzerContext.semanticErrors.push_back(
                    std::format("Line {}, Column {}: SEMANTIC ERROR -> duplicate case '{}'", 
                        _case->getToken().line, _case->getToken().column, _case->getLiteral()->getToken().value)
                );
            }
            else{
                for(const auto& _statement : _case->getSwitchBlock()->getStatements()){
                    checkStatement(_statement.get());
                }
                set.insert(val);
            }
        }
        if(_switch->hasDefault()){
            for(const auto& _statement : _switch->getDefault()->getSwitchBlock()->getStatements()){
                checkStatement(_statement.get());
            }
        }
    }
};

#endif