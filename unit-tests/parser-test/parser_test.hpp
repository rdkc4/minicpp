#ifndef PARSER_TEST_HPP
#define PARSER_TEST_HPP

#include "../../parser/parser.hpp"
#include "../lexer-test/lexer_test.hpp"

class ParserTest : public Parser {
    public:
        ParserTest(LexerTest& lexer) : Parser{ lexer } {}

        std::unique_ptr<ASTree> testFunction(){
            return function();
        }
        
        std::unique_ptr<ASTree> testVariable(){
            return variable();
        }

        std::unique_ptr<ASTree> testPrintfStatement(){
            return printfStatement();
        }

        std::unique_ptr<ASTree> testCompoundStatement(){
            return compoundStatement();
        }

        std::unique_ptr<ASTree> testAssignmentStatement(){
            return assignmentStatement();
        }

        std::unique_ptr<ASTree> testReturnStatement(){
            return returnStatement();
        }

        std::unique_ptr<ASTree> testIfStatement(){
            return ifStatement();
        }

        std::unique_ptr<ASTree> testWhileStatement(){
            return whileStatement();
        }

        std::unique_ptr<ASTree> testForStatement(){
            return forStatement();
        }

        std::unique_ptr<ASTree> testDoWhileStatement(){
            return doWhileStatement();
        }

        std::unique_ptr<ASTree> testSwitchStatement(){
            return switchStatement();
        }

        std::unique_ptr<ASTree> testNumericalExpression(){
            return numericalExpression();
        }

        std::unique_ptr<ASTree> testRelationalExpression(){
            return relationalExpression();
        }

        std::unique_ptr<ASTree> testFunctionCall(){
            return functionCall();
        }

};

#endif