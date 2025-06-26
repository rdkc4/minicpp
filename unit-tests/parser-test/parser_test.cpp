#include <gtest/gtest.h>
#include <stdexcept>

#include "../parser-test/parser_test.hpp"

TEST(ParserTest, ParseProgramSuccessful){
    LexerTest lexer{ "int fun(){ return 123; } int main(){ int a = 1 + 2; return a; }"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedChildrenSize = 2;

    std::unique_ptr<ASTree> _program;
    ASSERT_NO_THROW(_program = parser.parseProgram());
    ASSERT_EQ(_program->getNodeType(), ASTNodeType::PROGRAM);
    ASSERT_EQ(_program->getChild(0)->getChildren().size(), expectedChildrenSize);
}

TEST(ParserTest, ParseProgramThrows){
    LexerTest lexer{ "main(){ int a = 1 + 2; return a; }"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    std::unique_ptr<ASTree> _program;
    ASSERT_THROW(_program = parser.parseProgram(), std::runtime_error);
}

TEST(ParserTest, FunctionMultipleParams){
    LexerTest lexer{ "int rectArea(int a, int b){ return a * b; }"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    size_t expectedParamCount = 2;

    std::unique_ptr<ASTree> _function;
    ASSERT_NO_THROW(_function = parser.testFunction());
    ASSERT_EQ(_function->getNodeType(), ASTNodeType::FUNCTION);
    ASSERT_EQ(_function->getChild(0)->getChildren().size(), expectedParamCount);
}

TEST(ParserTest, FunctionSingleParam){
    LexerTest lexer{ "int sq(int a){ return a * a; }"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    size_t expectedParamCount = 1;

    std::unique_ptr<ASTree> _function;
    ASSERT_NO_THROW(_function = parser.testFunction());
    ASSERT_EQ(_function->getNodeType(), ASTNodeType::FUNCTION);
    ASSERT_EQ(_function->getChild(0)->getChildren().size(), expectedParamCount);
}

TEST(ParserTest, FunctionNoParams){
    LexerTest lexer{ "int main(){ return 0; }"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    size_t expectedParamCount = 0;

    std::unique_ptr<ASTree> _function;
    ASSERT_NO_THROW(_function = parser.testFunction());
    ASSERT_EQ(_function->getNodeType(), ASTNodeType::FUNCTION);
    ASSERT_EQ(_function->getChild(0)->getChildren().size(), expectedParamCount);
}

TEST(ParserTest, FunctionNoBodyThrows){
    LexerTest lexer{ "int main() return 0;"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    std::unique_ptr<ASTree> _function;
    ASSERT_THROW(_function = parser.testFunction(), std::runtime_error);
}

TEST(ParserTest, VariableDirectInit){
    LexerTest lexer{ "int x = a + fun(b,c);"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    std::unique_ptr<ASTree> _variable;
    ASSERT_NO_THROW(_variable = parser.testVariable());
    ASSERT_EQ(_variable->getNodeType(), ASTNodeType::VARIABLE);
    ASSERT_TRUE(_variable->getChildren().size() == 1);
}

TEST(ParserTest, VariableNoInit){
    LexerTest lexer{ "int x;"};
    lexer.tokenize();
    ParserTest parser{ lexer };


    std::unique_ptr<ASTree> _variable;
    ASSERT_NO_THROW(_variable = parser.testVariable());
    ASSERT_EQ(_variable->getNodeType(), ASTNodeType::VARIABLE);
    ASSERT_TRUE(_variable->getChildren().empty());
}

TEST(ParserTest, PrintfStatement){
    LexerTest lexer{ "printf(x + y);"};
    lexer.tokenize();
    ParserTest parser{ lexer };


    std::unique_ptr<ASTree> _printf;
    ASSERT_NO_THROW(_printf = parser.testPrintfStatement());
    ASSERT_EQ(_printf->getNodeType(), ASTNodeType::PRINTF);
}

TEST(ParserTest, EmptyCompoundStatement){
    LexerTest lexer{ "{}"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedChildrenSize = 0;

    std::unique_ptr<ASTree> _compound;
    ASSERT_NO_THROW(_compound = parser.testCompoundStatement());
    ASSERT_EQ(_compound->getNodeType(), ASTNodeType::COMPOUND_STATEMENT);
    ASSERT_EQ(_compound->getChildren().size(), expectedChildrenSize);
}

TEST(ParserTest, CompoundStatement){
    LexerTest lexer{ "{ int x = 3; printf(x); }"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedChildrenSize = 2;

    std::unique_ptr<ASTree> _compound;
    ASSERT_NO_THROW(_compound = parser.testCompoundStatement());
    ASSERT_EQ(_compound->getNodeType(), ASTNodeType::COMPOUND_STATEMENT);
    ASSERT_EQ(_compound->getChildren().size(), expectedChildrenSize);
}

TEST(ParserTest, AssignmentStatement){
    LexerTest lexer{ "x = 123u;"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    std::unique_ptr<ASTree> _assignment;
    ASSERT_NO_THROW(_assignment = parser.testAssignmentStatement());
    ASSERT_EQ(_assignment->getNodeType(), ASTNodeType::ASSIGNMENT_STATEMENT);
    ASSERT_TRUE(!_assignment->getChildren().empty());
}

TEST(ParserTest, ReturnStatementVoid){
    LexerTest lexer{ "return;"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    std::unique_ptr<ASTree> _return;
    ASSERT_NO_THROW(_return = parser.testReturnStatement());
    ASSERT_EQ(_return->getNodeType(), ASTNodeType::RETURN_STATEMENT);
    ASSERT_TRUE(_return->getChildren().empty());
}

TEST(ParserTest, ReturnStatement){
    LexerTest lexer{ "return fun(1, 2);"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    std::unique_ptr<ASTree> _return;
    ASSERT_NO_THROW(_return = parser.testReturnStatement());
    ASSERT_EQ(_return->getNodeType(), ASTNodeType::RETURN_STATEMENT);
    ASSERT_TRUE(!_return->getChildren().empty());
}

TEST(ParserTest, IfStatementOnlyIf){
    LexerTest lexer{ "if(a > b) return a;"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedChildrenSize = 2;

    std::unique_ptr<ASTree> _if;
    ASSERT_NO_THROW(_if = parser.testIfStatement());
    ASSERT_EQ(_if->getNodeType(), ASTNodeType::IF_STATEMENT);
    ASSERT_EQ(_if->getChildren().size(), expectedChildrenSize);
}

TEST(ParserTest, IfStatementIfElse){
    LexerTest lexer{ "if(a > b) return a; else return b;"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedChildrenSize = 3;

    std::unique_ptr<ASTree> _if;
    ASSERT_NO_THROW(_if = parser.testIfStatement());
    ASSERT_EQ(_if->getNodeType(), ASTNodeType::IF_STATEMENT);
    ASSERT_EQ(_if->getChildren().size(), expectedChildrenSize);
}

TEST(ParserTest, IfStatementIfElseif){
    LexerTest lexer{ "if(a > b) return a; else if(a == b) return b;"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedChildrenSize = 4;

    std::unique_ptr<ASTree> _if;
    ASSERT_NO_THROW(_if = parser.testIfStatement());
    ASSERT_EQ(_if->getNodeType(), ASTNodeType::IF_STATEMENT);
    ASSERT_EQ(_if->getChildren().size(), expectedChildrenSize);
}

TEST(ParserTest, IfStatementIfElseifElse){
    LexerTest lexer{ "if(a > b) return a; else if(a == b) return 0; else return b;"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedChildrenSize = 5;

    std::unique_ptr<ASTree> _if;
    ASSERT_NO_THROW(_if = parser.testIfStatement());
    ASSERT_EQ(_if->getNodeType(), ASTNodeType::IF_STATEMENT);
    ASSERT_EQ(_if->getChildren().size(), expectedChildrenSize);
}

TEST(ParserTest, WhileStatement){
    LexerTest lexer{ "while(a > b) b = b + 1;"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedChildrenSize = 2;

    std::unique_ptr<ASTree> _while;
    ASSERT_NO_THROW(_while = parser.testWhileStatement());
    ASSERT_EQ(_while->getNodeType(), ASTNodeType::WHILE_STATEMENT);
    ASSERT_EQ(_while->getChildren().size(), expectedChildrenSize);
}

TEST(ParserTest, ForStatement){
    LexerTest lexer{ "for(i = 0; i < 5; i = i + 1) printf(i);"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedChildrenSize = 4;

    std::unique_ptr<ASTree> _for;
    ASSERT_NO_THROW(_for = parser.testForStatement());
    ASSERT_EQ(_for->getNodeType(), ASTNodeType::FOR_STATEMENT);
    ASSERT_EQ(_for->getChildren().size(), expectedChildrenSize);   
}

TEST(ParserTest, DoWhileStatement){
    LexerTest lexer{ "do x = x + 1; while(x < 10);"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedChildrenSize = 2;

    std::unique_ptr<ASTree> _dowhile;
    ASSERT_NO_THROW(_dowhile = parser.testDoWhileStatement());
    ASSERT_EQ(_dowhile->getNodeType(), ASTNodeType::DO_WHILE_STATEMENT);
    ASSERT_EQ(_dowhile->getChildren().size(), expectedChildrenSize);   
}

TEST(ParserTest, SwitchStatement){
    LexerTest lexer{ "switch(x){ case 1: break; default: return 1; }"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedChildrenSize = 3;

    std::unique_ptr<ASTree> _switch;
    ASSERT_NO_THROW(_switch = parser.testSwitchStatement());
    ASSERT_EQ(_switch->getNodeType(), ASTNodeType::SWITCH_STATEMENT);
    ASSERT_EQ(_switch->getChildren().size(), expectedChildrenSize);   
}

TEST(ParserTest, SwitchStatementNoDefault){
    LexerTest lexer{ "switch(x){ case 1: break; }"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedChildrenSize = 2;

    std::unique_ptr<ASTree> _switch;
    ASSERT_NO_THROW(_switch = parser.testSwitchStatement());
    ASSERT_EQ(_switch->getNodeType(), ASTNodeType::SWITCH_STATEMENT);
    ASSERT_EQ(_switch->getChildren().size(), expectedChildrenSize);   
}

TEST(ParserTest, SwitchStatementNoCaseThrows){
    LexerTest lexer{ "switch(x){ default: return 1; }"};
    lexer.tokenize();
    ParserTest parser{ lexer };


    std::unique_ptr<ASTree> _switch;
    ASSERT_THROW(_switch = parser.testSwitchStatement(), std::runtime_error);
}

TEST(ParserTest, NumericalExpression){
    LexerTest lexer{ "1 + 2"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedChildrenSize = 2;

    std::unique_ptr<ASTree> _numexp;
    ASSERT_NO_THROW(_numexp = parser.testNumericalExpression());
    ASSERT_EQ(_numexp->getNodeType(), ASTNodeType::NUMERICAL_EXPRESSION);
    ASSERT_EQ(_numexp->getChildren().size(), expectedChildrenSize);
    ASSERT_EQ(_numexp->getToken().value, "+");
}

TEST(ParserTest, RelationalExpression){
    LexerTest lexer{ "1 + 2 > a + b"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedChildrenSize = 2;

    std::unique_ptr<ASTree> _numexp;
    ASSERT_NO_THROW(_numexp = parser.testRelationalExpression());
    ASSERT_EQ(_numexp->getNodeType(), ASTNodeType::RELATIONAL_EXPRESSION);
    ASSERT_EQ(_numexp->getChildren().size(), expectedChildrenSize);
    ASSERT_EQ(_numexp->getToken().value, ">");
}

TEST(ParserTest, FunctionCallNoArgs){
    LexerTest lexer{ "fun()"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    std::unique_ptr<ASTree> _functionCall;
    ASSERT_NO_THROW(_functionCall = parser.testFunctionCall());
    ASSERT_EQ(_functionCall->getNodeType(), ASTNodeType::FUNCTION_CALL);
    ASSERT_TRUE(_functionCall->getChild(0)->getChildren().empty());
    ASSERT_EQ(_functionCall->getToken().value, "fun");
}

TEST(ParserTest, FunctionCallSingleArg){
    LexerTest lexer{ "fun(x)"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedChildrenSize = 1;

    std::unique_ptr<ASTree> _functionCall;
    ASSERT_NO_THROW(_functionCall = parser.testFunctionCall());
    ASSERT_EQ(_functionCall->getNodeType(), ASTNodeType::FUNCTION_CALL);
    ASSERT_EQ(_functionCall->getChild(0)->getChildren().size(), expectedChildrenSize);
    ASSERT_EQ(_functionCall->getToken().value, "fun");
}

TEST(ParserTest, FunctionCallMultipleArgs){
    LexerTest lexer{ "fun(x, y)"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedChildrenSize = 2;

    std::unique_ptr<ASTree> _functionCall;
    ASSERT_NO_THROW(_functionCall = parser.testFunctionCall());
    ASSERT_EQ(_functionCall->getNodeType(), ASTNodeType::FUNCTION_CALL);
    ASSERT_EQ(_functionCall->getChild(0)->getChildren().size(), expectedChildrenSize);
    ASSERT_EQ(_functionCall->getToken().value, "fun");
}

TEST(ParserTest, FunctionCallInvalidArgs){
    LexerTest lexer{ "fun(x y)"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    std::unique_ptr<ASTree> _functionCall;
    ASSERT_THROW(_functionCall = parser.testFunctionCall(), std::runtime_error);
}
