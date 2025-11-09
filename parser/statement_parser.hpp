#ifndef STATEMENT_PARSER_HPP
#define STATEMENT_PARSER_HPP

#include <memory>

#include "../common/abstract-syntax-tree/ASTStatement.hpp"
#include "../common/abstract-syntax-tree/ASTVariable.hpp"
#include "../common/abstract-syntax-tree/ASTPrintfSt.hpp"
#include "../common/abstract-syntax-tree/ASTCompoundSt.hpp"
#include "../common/abstract-syntax-tree/ASTAssignSt.hpp"
#include "../common/abstract-syntax-tree/ASTReturnSt.hpp"
#include "../common/abstract-syntax-tree/ASTIfSt.hpp"
#include "../common/abstract-syntax-tree/ASTWhileSt.hpp"
#include "../common/abstract-syntax-tree/ASTDoWhileSt.hpp"
#include "../common/abstract-syntax-tree/ASTForSt.hpp"
#include "../common/abstract-syntax-tree/ASTSwitchSt.hpp"

#include "expression_parser.hpp"
#include "token_consumer.hpp"

/** 
 * @class StatementParser
 * @brief used for parsing statements
*/
class StatementParser {
public:
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
     * | PRINTF_STATEMENT
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
     * | SWITCH_STATEMENT
     * @return pointer to the statement node
    */
    std::unique_ptr<ASTStatement> statement();

private:
    ExpressionParser expParser;
    TokenConsumer& tokenConsumer;

protected:
    /** 
     * @brief parses variable declaration
     * @details VARIABLE_DECL : TYPE ID (ASSIGN NUMERICAL_EXPRESSION)? SEMICOLON
     * @returns pointer to a variable declaration node
    */
    std::unique_ptr<ASTVariable> variable();

    /** 
     * @brief parses printf statement
     * @details PRINTF_STATEMENT: PRINTF LPAREN NUMERICAL_EXPRESSION RPAREN SEMICOLON;
     * @returns pointer to a printf statement node
    */
    std::unique_ptr<ASTPrintfSt> printfStatement();

    /** 
     * @brief parses compound statement
     * @details COMPOUND_STATEMENT : LBRACKET (STATEMENT)? RBRACKET
     * @returns pointer to a compound statement node
    */
    std::unique_ptr<ASTCompoundSt> compoundStatement();

    /** 
     * @brief parses assignment statement
     * @param expectsSemicolon - flag whether assignment statement ends with semicolon
     * @details ASSIGNMENT_STATEMENT : ID ASSIGN NUMERICAL_EXPRESSION SEMICOLON
     * @returns pointer to an assignment statement node
    */
    std::unique_ptr<ASTAssignSt> assignmentStatement(bool expectsSemicolon = true);

    /** 
     * @brief parses return statement
     * @details RETURN_STATEMENT : RETURN (NUMERICAL_EXPRESSION)? SEMICOLON
     * @returns pointer to a return statement node
    */
    std::unique_ptr<ASTReturnSt> returnStatement();

    /** 
     * @brief parses if statement
     * @details
     *
     * IF_STATEMENT
     *
     * : IF LPAREN RELATIONAL_EXPRESSION RPAREN STATEMENT
     *
     * | IF_STATEMENT ELSE IF STATEMENT
     *
     * | IF_STATEMENT ELSE STATEMENT
     * @returns pointer to an if statement node
    */
    std::unique_ptr<ASTIfSt> ifStatement();

    /** 
     * @brief parses while statement
     * @details WHILE_STATEMENT : WHILE LPAREN RELATIONAL_EXPRESSION RPAREN STATEMENT
     * @returns pointer to a while statement node
    */
    std::unique_ptr<ASTWhileSt> whileStatement();

    /** 
     * @brief parses for statement
     * @details FOR_STATEMENT : FOR LPAREN (ASSIGN_STATEMENT SEMICOLON)? (RELATIONAL_EXPRESSION SEMICOLON)? (ASSIGNMENT_STATEMENT)? RPAREN STATEMENT
     * @returns pointer to a for statement node
    */
    std::unique_ptr<ASTForSt> forStatement();

    /** 
     * @brief parses do-while statement
     * @details DO_WHILE_STATEMENT : DO STATEMENT WHILE LPAREN RELATIONAL_EXPRESSION RPAREN SEMICOLON
     * @returns pointer to a do-while statement node
    */
    std::unique_ptr<ASTDoWhileSt> doWhileStatement();

    /** 
     * @brief parses switch statement
     * @details SWITCH_STATEMENT : SWITCH LPAREN ID RPAREN LBRACKET (_CASE)+ (_DEFAULT)? RBRACKET
     * @returns pointer to a switch statement node
    */
    std::unique_ptr<ASTSwitchSt> switchStatement();

    /** 
     * @brief parses swtich-case block
     * @details SWITCH_CASE_BLOCK : (STATEMENT)*
     * @returns pointer to a switch-case block node
    */
    std::unique_ptr<ASTSwitchBlock> switchCaseBlock();

    /** 
     * @brief parses case of the switch statement
     * @details _CASE : CASE LITERAL COLON SWITCH_CASE_BLOCK (_BREAK)?
     * @returns pointer to a case statement node
    */
    std::unique_ptr<ASTCaseSt> _case();

    /** 
     * @brief parses default case of the switch statement
     * @details _DEFAULT : DEFAULT COLON SWITCH_CASE_BLOCK (_BREAK)?
     * @returns pointer to a default statement node
    */
    std::unique_ptr<ASTDefaultSt> _default();

    /** 
     * @brief parses break statement
     * @details _BREAK : BREAK SEMICOLON
     * @returns void
    */
    void _break();

};

#endif