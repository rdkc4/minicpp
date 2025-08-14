#include <gtest/gtest.h>
#include <stdexcept>

#include "../parser-test/parser_test.hpp"

TEST(ParserTest, ParseProgramSuccessful){
    LexerTest lexer{ "int fun(){ return 123; } int main(){ int a = 1 + 2; return a; }"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    const size_t expectedFunctionCount = 2;

    std::unique_ptr<ASTProgram> _program;
    ASSERT_NO_THROW(_program = parser.parseProgram());
    ASSERT_EQ(_program->getNodeType(), ASTNodeType::PROGRAM);
    ASSERT_EQ(_program->getFunctionCount(), expectedFunctionCount);
}

TEST(ParserTest, ParseProgramThrows){
    LexerTest lexer{ "main(){ int a = 1 + 2; return a; }"};
    lexer.tokenize();
    ParserTest parser{ lexer };

    std::unique_ptr<ASTProgram> _program;
    ASSERT_THROW(_program = parser.parseProgram(), std::runtime_error);
}

TEST(ParserTest, FunctionMultipleParams){
    LexerTest lexer{ "int rectArea(int a, int b){ return a * b; }"};
    lexer.tokenize();
    FunctionParserTest parser{ lexer };

    size_t expectedParamCount = 2;

    std::unique_ptr<ASTFunction> _function;
    ASSERT_NO_THROW(_function = parser.testFunction());
    ASSERT_EQ(_function->getNodeType(), ASTNodeType::FUNCTION);
    ASSERT_EQ(_function->getParameterCount(), expectedParamCount);
}

TEST(ParserTest, FunctionSingleParam){
    LexerTest lexer{ "int sq(int a){ return a * a; }"};
    lexer.tokenize();
    FunctionParserTest parser{ lexer };

    size_t expectedParamCount = 1;

    std::unique_ptr<ASTFunction> _function;
    ASSERT_NO_THROW(_function = parser.testFunction());
    ASSERT_EQ(_function->getNodeType(), ASTNodeType::FUNCTION);
    ASSERT_EQ(_function->getParameterCount(), expectedParamCount);
}

TEST(ParserTest, FunctionNoParams){
    LexerTest lexer{ "int main(){ return 0; }"};
    lexer.tokenize();
    FunctionParserTest parser{ lexer };

    size_t expectedParamCount = 0;

    std::unique_ptr<ASTFunction> _function;
    ASSERT_NO_THROW(_function = parser.testFunction());
    ASSERT_EQ(_function->getNodeType(), ASTNodeType::FUNCTION);
    ASSERT_EQ(_function->getParameterCount(), expectedParamCount);
}

TEST(ParserTest, FunctionNoBodyThrows){
    LexerTest lexer{ "int main() return 0;"};
    lexer.tokenize();
    FunctionParserTest parser{ lexer };

    std::unique_ptr<ASTFunction> _function;
    ASSERT_THROW(_function = parser.testFunction(), std::runtime_error);
}

TEST(ParserTest, VariableDirectInit){
    LexerTest lexer{ "int x = a + fun(b,c);"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };

    std::unique_ptr<ASTVariable> _variable;
    ASSERT_NO_THROW(_variable = parser.testVariable());
    ASSERT_EQ(_variable->getNodeType(), ASTNodeType::VARIABLE);
    ASSERT_TRUE(_variable->hasAssign());
}

TEST(ParserTest, VariableNoInit){
    LexerTest lexer{ "int x;"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };


    std::unique_ptr<ASTVariable> _variable;
    ASSERT_NO_THROW(_variable = parser.testVariable());
    ASSERT_EQ(_variable->getNodeType(), ASTNodeType::VARIABLE);
    ASSERT_FALSE(_variable->hasAssign());
}

TEST(ParserTest, PrintfStatement){
    LexerTest lexer{ "printf(x + y);"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };


    std::unique_ptr<ASTPrintfSt> _printf;
    ASSERT_NO_THROW(_printf = parser.testPrintfStatement());
    ASSERT_EQ(_printf->getNodeType(), ASTNodeType::PRINTF);
}

TEST(ParserTest, EmptyCompoundStatement){
    LexerTest lexer{ "{}"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };

    const size_t expectedChildrenSize = 0;

    std::unique_ptr<ASTCompoundSt> _compound;
    ASSERT_NO_THROW(_compound = parser.testCompoundStatement());
    ASSERT_EQ(_compound->getNodeType(), ASTNodeType::COMPOUND_STATEMENT);
    ASSERT_EQ(_compound->getStatements().size(), expectedChildrenSize);
}

TEST(ParserTest, CompoundStatement){
    LexerTest lexer{ "{ int x = 3; printf(x); }"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };

    const size_t expectedChildrenSize = 2;

    std::unique_ptr<ASTCompoundSt> _compound;
    ASSERT_NO_THROW(_compound = parser.testCompoundStatement());
    ASSERT_EQ(_compound->getNodeType(), ASTNodeType::COMPOUND_STATEMENT);
    ASSERT_EQ(_compound->getStatements().size(), expectedChildrenSize);
}

TEST(ParserTest, AssignmentStatement){
    LexerTest lexer{ "x = 123u;"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };

    std::unique_ptr<ASTAssignSt> _assignment;
    ASSERT_NO_THROW(_assignment = parser.testAssignmentStatement());
    ASSERT_EQ(_assignment->getNodeType(), ASTNodeType::ASSIGNMENT_STATEMENT);
    ASSERT_TRUE(_assignment->getExp()->getNodeType() == ASTNodeType::LITERAL);
}

TEST(ParserTest, ReturnStatementVoid){
    LexerTest lexer{ "return;"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };

    std::unique_ptr<ASTReturnSt> _return;
    ASSERT_NO_THROW(_return = parser.testReturnStatement());
    ASSERT_EQ(_return->getNodeType(), ASTNodeType::RETURN_STATEMENT);
    ASSERT_FALSE(_return->returns());
}

TEST(ParserTest, ReturnStatement){
    LexerTest lexer{ "return fun(1, 2);"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };

    std::unique_ptr<ASTReturnSt> _return;
    ASSERT_NO_THROW(_return = parser.testReturnStatement());
    ASSERT_EQ(_return->getNodeType(), ASTNodeType::RETURN_STATEMENT);
    ASSERT_TRUE(_return->returns());
}

TEST(ParserTest, IfStatementOnlyIf){
    LexerTest lexer{ "if(a > b) return a;"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };

    const size_t expectedConditionCount = 1;

    std::unique_ptr<ASTIfSt> _if;
    ASSERT_NO_THROW(_if = parser.testIfStatement());
    ASSERT_EQ(_if->getNodeType(), ASTNodeType::IF_STATEMENT);
    ASSERT_EQ(_if->getConditions().size(), expectedConditionCount);
    ASSERT_FALSE(_if->hasElse());
}

TEST(ParserTest, IfStatementIfElse){
    LexerTest lexer{ "if(a > b) return a; else return b;"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };

    const size_t expectedStatementCount = 2;

    std::unique_ptr<ASTIfSt> _if;
    ASSERT_NO_THROW(_if = parser.testIfStatement());
    ASSERT_EQ(_if->getNodeType(), ASTNodeType::IF_STATEMENT);
    ASSERT_EQ(_if->getStatements().size(), expectedStatementCount);
    ASSERT_TRUE(_if->hasElse());
}

TEST(ParserTest, IfStatementIfElseif){
    LexerTest lexer{ "if(a > b) return a; else if(a == b) return b;"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };

    const size_t expectedConditionCount = 2;

    std::unique_ptr<ASTIfSt> _if;
    ASSERT_NO_THROW(_if = parser.testIfStatement());
    ASSERT_EQ(_if->getNodeType(), ASTNodeType::IF_STATEMENT);
    ASSERT_EQ(_if->getConditions().size(), expectedConditionCount);
}

TEST(ParserTest, IfStatementIfElseifElse){
    LexerTest lexer{ "if(a > b) return a; else if(a == b) return 0; else return b;"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };

    const size_t expectedStatementCount = 3;

    std::unique_ptr<ASTIfSt> _if;
    ASSERT_NO_THROW(_if = parser.testIfStatement());
    ASSERT_EQ(_if->getNodeType(), ASTNodeType::IF_STATEMENT);
    ASSERT_EQ(_if->getStatements().size(), expectedStatementCount);
    ASSERT_TRUE(_if->hasElse());
}

TEST(ParserTest, WhileStatement){
    LexerTest lexer{ "while(a > b) b = b + 1;"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };

    std::unique_ptr<ASTWhileSt> _while;
    ASSERT_NO_THROW(_while = parser.testWhileStatement());
    ASSERT_EQ(_while->getNodeType(), ASTNodeType::WHILE_STATEMENT);
}

TEST(ParserTest, ForStatement){
    LexerTest lexer{ "for(i = 0; i < 5; i = i + 1) printf(i);"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };

    std::unique_ptr<ASTForSt> _for;
    ASSERT_NO_THROW(_for = parser.testForStatement());
    ASSERT_EQ(_for->getNodeType(), ASTNodeType::FOR_STATEMENT);
    ASSERT_TRUE(_for->hasInitializer());
    ASSERT_TRUE(_for->hasCondition());
    ASSERT_TRUE(_for->hasIncrementer());
}

TEST(ParserTest, DoWhileStatement){
    LexerTest lexer{ "do x = x + 1; while(x < 10);"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };

    std::unique_ptr<ASTDoWhileSt> _dowhile;
    ASSERT_NO_THROW(_dowhile = parser.testDoWhileStatement());
    ASSERT_EQ(_dowhile->getNodeType(), ASTNodeType::DO_WHILE_STATEMENT);
}

TEST(ParserTest, SwitchStatement){
    LexerTest lexer{ "switch(x){ case 1: break; default: return 1; }"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };

    const size_t expectedCaseCount = 1;

    std::unique_ptr<ASTSwitchSt> _switch;
    ASSERT_NO_THROW(_switch = parser.testSwitchStatement());
    ASSERT_EQ(_switch->getNodeType(), ASTNodeType::SWITCH_STATEMENT);
    ASSERT_EQ(_switch->getCases().size(), expectedCaseCount);
    ASSERT_TRUE(_switch->hasDefault());  
}

TEST(ParserTest, SwitchStatementNoDefault){
    LexerTest lexer{ "switch(x){ case 1: break; }"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };

    const size_t expectedCaseCount = 1;

    std::unique_ptr<ASTSwitchSt> _switch;
    ASSERT_NO_THROW(_switch = parser.testSwitchStatement());
    ASSERT_EQ(_switch->getNodeType(), ASTNodeType::SWITCH_STATEMENT);
    ASSERT_EQ(_switch->getCases().size(), expectedCaseCount);
    ASSERT_FALSE(_switch->hasDefault());   
}

TEST(ParserTest, SwitchStatementNoCaseThrows){
    LexerTest lexer{ "switch(x){ default: return 1; }"};
    lexer.tokenize();
    StatementParserTest parser{ lexer };

    std::unique_ptr<ASTSwitchSt> _switch;
    ASSERT_THROW(_switch = parser.testSwitchStatement(), std::runtime_error);
}

TEST(ParserTest, NumericalExpression){
    LexerTest lexer{ "1 + 2"};
    lexer.tokenize();
    ExpressionParserTest parser{ lexer };

    std::unique_ptr<ASTExpression> _numexp;
    ASSERT_NO_THROW(_numexp = parser.testNumericalExpression());
    ASSERT_EQ(_numexp->getNodeType(), ASTNodeType::BINARY_EXPRESSION);
    ASSERT_EQ(static_cast<ASTBinaryExpression*>(_numexp.get())->getOperator(), Operators::ADD);
}

TEST(ParserTest, RelationalExpression){
    LexerTest lexer{ "1 + 2 > a + b"};
    lexer.tokenize();
    ExpressionParserTest parser{ lexer };

    std::unique_ptr<ASTExpression> _relexp;
    ASSERT_NO_THROW(_relexp = parser.testRelationalExpression());
    ASSERT_EQ(_relexp->getNodeType(), ASTNodeType::BINARY_EXPRESSION);
    ASSERT_EQ(static_cast<ASTBinaryExpression*>(_relexp.get())->getOperator(), Operators::GREATER);
}

TEST(ParserTest, FunctionCallNoArgs){
    LexerTest lexer{ "fun()"};
    lexer.tokenize();
    ExpressionParserTest parser{ lexer };

    std::unique_ptr<ASTFunctionCall> _functionCall;
    ASSERT_NO_THROW(_functionCall = parser.testFunctionCall());
    ASSERT_EQ(_functionCall->getNodeType(), ASTNodeType::FUNCTION_CALL);
    ASSERT_TRUE(_functionCall->getArgumentCount() == 0);
    ASSERT_EQ(_functionCall->getToken().value, "fun");
}

TEST(ParserTest, FunctionCallSingleArg){
    LexerTest lexer{ "fun(x)"};
    lexer.tokenize();
    ExpressionParserTest parser{ lexer };

    const size_t expectedArgumentCount = 1;

    std::unique_ptr<ASTFunctionCall> _functionCall;
    ASSERT_NO_THROW(_functionCall = parser.testFunctionCall());
    ASSERT_EQ(_functionCall->getNodeType(), ASTNodeType::FUNCTION_CALL);
    ASSERT_EQ(_functionCall->getArgumentCount(), expectedArgumentCount);
    ASSERT_EQ(_functionCall->getToken().value, "fun");
}

TEST(ParserTest, FunctionCallMultipleArgs){
    LexerTest lexer{ "fun(x, y)"};
    lexer.tokenize();
    ExpressionParserTest parser{ lexer };

    const size_t expectedArgumentCount = 2;

    std::unique_ptr<ASTFunctionCall> _functionCall;
    ASSERT_NO_THROW(_functionCall = parser.testFunctionCall());
    ASSERT_EQ(_functionCall->getNodeType(), ASTNodeType::FUNCTION_CALL);
    ASSERT_EQ(_functionCall->getArgumentCount(), expectedArgumentCount);
    ASSERT_EQ(_functionCall->getToken().value, "fun");
}

TEST(ParserTest, FunctionCallInvalidArgs){
    LexerTest lexer{ "fun(x y)"};
    lexer.tokenize();
    ExpressionParserTest parser{ lexer };

    std::unique_ptr<ASTFunctionCall> _functionCall;
    ASSERT_THROW(_functionCall = parser.testFunctionCall(), std::runtime_error);
}
