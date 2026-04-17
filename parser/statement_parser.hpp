#ifndef STATEMENT_PARSER_HPP
#define STATEMENT_PARSER_HPP

#include <memory>

#include "../common/abstract-syntax-tree/ast_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_variable_decl_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_compound_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_assign_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_return_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_if_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_while_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_dowhile_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_for_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_switch_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_function_call_stmt.hpp"

#include "expression_parser.hpp"
#include "token_consumer.hpp"

/** 
 * @class StatementParser
 * @brief used for parsing statements
*/
class StatementParser {
public:
    /** 
     * @brief Creates new instance of the parser specialized for statements
     * @param consumer - reference to token handler wrapped around the lexer 
    */
    StatementParser(TokenConsumer& consumer);

    /** 
     * @brief parses statement
     * @details 
     *
     * STATEMENT 
     * 
     * : VARIABLE_DECL
     *
     * | RETURN_STATEMENT
     *
     * | IF_STATEMENT
     *
     * | COMPOUND_STATEMENT
     *
     * | WHILE_STATEMENT
     *
     * | FOR_STATEMENT
     *
     * | DO_WHILE_STATEMENT
     *
     * | FUNCTION_CALL_STATEMENT
     *
     * | SWITCH_STATEMENT
     * @return pointer to the statement node
    */
    std::unique_ptr<syntax::ast::ASTStmt> parseStmt();

    /** 
     * @brief parses variable declaration
     * @details VARIABLE_DECL : TYPE ID (ASSIGN EXPRESSION)? SEMICOLON
     * @returns pointer to a variable declaration node
    */
    std::unique_ptr<syntax::ast::ASTVariableDeclStmt> parseVariableDeclStmt();

    /** 
     * @brief parses compound statement
     * @details COMPOUND_STATEMENT : LBRACE (STATEMENT)? RBRACE
     * @returns pointer to a compound statement node
    */
    std::unique_ptr<syntax::ast::ASTCompoundStmt> parseCompoundStmt();

    /** 
     * @brief parses assignment statement
     * @param expectsSemicolon - flag whether assignment statement ends with semicolon
     * @details ASSIGNMENT_STATEMENT : ID ASSIGN EXPRESSION SEMICOLON
     * @returns pointer to an assignment statement node
    */
    std::unique_ptr<syntax::ast::ASTAssignStmt> parseAssignStmt(bool expectsSemicolon = true);

    /** 
     * @brief parses return statement
     * @details RETURN_STATEMENT : RETURN (EXPRESSION)? SEMICOLON
     * @returns pointer to a return statement node
    */
    std::unique_ptr<syntax::ast::ASTReturnStmt> parseReturnStmt();

    /** 
     * @brief parses if statement
     * @details
     *
     * IF_STATEMENT
     *
     * : IF LPAREN EXPRESSION RPAREN STATEMENT
     *
     * | IF_STATEMENT ELSE IF STATEMENT
     *
     * | IF_STATEMENT ELSE STATEMENT
     * @returns pointer to an if statement node
    */
    std::unique_ptr<syntax::ast::ASTIfStmt> parseIfStmt();

    /** 
     * @brief parses while statement
     * @details WHILE_STATEMENT : WHILE LPAREN EXPRESSION RPAREN STATEMENT
     * @returns pointer to a while statement node
    */
    std::unique_ptr<syntax::ast::ASTWhileStmt> parseWhileStmt();

    /** 
     * @brief parses for statement
     * @details FOR_STATEMENT : FOR LPAREN (ASSIGN_STATEMENT SEMICOLON)? (EXPRESSION SEMICOLON)? (ASSIGNMENT_STATEMENT)? RPAREN STATEMENT
     * @returns pointer to a for statement node
    */
    std::unique_ptr<syntax::ast::ASTForStmt> parseForStmt();

    /** 
     * @brief parses do-while statement
     * @details DO_WHILE_STATEMENT : DO STATEMENT WHILE LPAREN EXPRESSION RPAREN SEMICOLON
     * @returns pointer to a do-while statement node
    */
    std::unique_ptr<syntax::ast::ASTDoWhileStmt> parseDoWhileStmt();

    /** 
     * @brief parses function-call statement
     * @details FUNCTION_CALL_STATEMENT : FUNCTION_CALL SEMICOLON
     * @returns pointer to a function-call statement node
    */
    std::unique_ptr<syntax::ast::ASTFunctionCallStmt> parseFunctionCallStmt();

    /** 
     * @brief parses switch statement
     * @details SWITCH_STATEMENT : SWITCH LPAREN ID RPAREN LBRACE (_CASE)+ (_DEFAULT)? RBRACE
     * @returns pointer to a switch statement node
    */
    std::unique_ptr<syntax::ast::ASTSwitchStmt> parseSwitchStmt();

    /** 
     * @brief parses swtich-case block
     * @details SWITCH_CASE_BLOCK : (STATEMENT)*
     * @returns pointer to a switch-case block node
    */
    std::unique_ptr<syntax::ast::ASTSwitchBlockStmt> parseSwitchBlockStmt();

    /** 
     * @brief parses case of the switch statement
     * @details _CASE : CASE LITERAL COLON SWITCH_CASE_BLOCK (_BREAK)?
     * @returns pointer to a case statement node
    */
    std::unique_ptr<syntax::ast::ASTCaseStmt> parseCaseStmt();

    /** 
     * @brief parses default case of the switch statement
     * @details _DEFAULT : DEFAULT COLON SWITCH_CASE_BLOCK (_BREAK)?
     * @returns pointer to a default statement node
    */
    std::unique_ptr<syntax::ast::ASTDefaultStmt> parseDefaultStmt();

    /** 
     * @brief parses break statement
     * @details _BREAK : BREAK SEMICOLON
    */
    void parseBreakStmt();

private:
    /// parser specialized for expressions
    ExpressionParser exprParser;
    
    /// reference to a token handler wrapped around the lexer
    TokenConsumer& tokenConsumer;

};

#endif