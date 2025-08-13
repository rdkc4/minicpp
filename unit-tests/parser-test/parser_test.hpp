#ifndef PARSER_TEST_HPP
#define PARSER_TEST_HPP

#include "../../parser/parser.hpp"
#include "../lexer-test/lexer_test.hpp"

class ParserTest : public Parser {
    public:
        ParserTest(LexerTest& lexer) : Parser{ lexer } {}

        std::unique_ptr<ASTFunction> testFunction(){
            return function();
        }
        
        std::unique_ptr<ASTVariable> testVariable(){
            return variable();
        }

        std::unique_ptr<ASTPrintfSt> testPrintfStatement(){
            return printfStatement();
        }

        std::unique_ptr<ASTCompoundSt> testCompoundStatement(){
            return compoundStatement();
        }

        std::unique_ptr<ASTAssignSt> testAssignmentStatement(){
            return assignmentStatement();
        }

        std::unique_ptr<ASTReturnSt> testReturnStatement(){
            return returnStatement();
        }

        std::unique_ptr<ASTIfSt> testIfStatement(){
            return ifStatement();
        }

        std::unique_ptr<ASTWhileSt> testWhileStatement(){
            return whileStatement();
        }

        std::unique_ptr<ASTForSt> testForStatement(){
            return forStatement();
        }

        std::unique_ptr<ASTDoWhileSt> testDoWhileStatement(){
            return doWhileStatement();
        }

        std::unique_ptr<ASTSwitchSt> testSwitchStatement(){
            return switchStatement();
        }

        std::unique_ptr<ASTExpression> testNumericalExpression(){
            return numericalExpression();
        }

        std::unique_ptr<ASTExpression> testRelationalExpression(){
            return relationalExpression();
        }

        std::unique_ptr<ASTFunctionCall> testFunctionCall(){
            return functionCall();
        }

};

#endif