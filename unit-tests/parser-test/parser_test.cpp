#include <gtest/gtest.h>
#include <stdexcept>

#include "../lexer-test//lexer_test.hpp"
#include "../parser-test/parser_test.hpp"

TEST(ParserTest, ParseProgramSuccessful){
    std::vector<std::string> input{"int fun(){ return 123; } int main(){ int a = 1 + 2; return a; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };

    const size_t expectedFunctionCount = 2;

    std::unique_ptr<ASTProgram> _program;
    ASSERT_NO_THROW(_program = parser.parseProgram());
    ASSERT_EQ(_program->getNodeType(), ASTNodeType::PROGRAM);
    ASSERT_EQ(_program->getFunctionCount(), expectedFunctionCount);
}

TEST(ParserTest, ParseProgramThrows){
    std::vector<std::string> input{"main(){ int a = 1 + 2; return a; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTProgram> _program;
    ASSERT_THROW(_program = parser.parseProgram(), std::runtime_error);
}

TEST(ParserTest, FunctionMultipleParams){
    std::vector<std::string> input{"int rectArea(int a, int b){ return a * b; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };

    size_t expectedParamCount = 2;

    std::unique_ptr<ASTFunction> _function;
    ASSERT_NO_THROW(_function = parser.function());
    ASSERT_EQ(_function->getNodeType(), ASTNodeType::FUNCTION);
    ASSERT_EQ(_function->getParameterCount(), expectedParamCount);
}

TEST(ParserTest, FunctionSingleParam){
    std::vector<std::string> input{"int sq(int a){ return a * a; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };

    size_t expectedParamCount = 1;

    std::unique_ptr<ASTFunction> _function;
    ASSERT_NO_THROW(_function = parser.function());
    ASSERT_EQ(_function->getNodeType(), ASTNodeType::FUNCTION);
    ASSERT_EQ(_function->getParameterCount(), expectedParamCount);
}

TEST(ParserTest, FunctionNoParams){
    std::vector<std::string> input{"int main(){ return 0; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };

    size_t expectedParamCount = 0;

    std::unique_ptr<ASTFunction> _function;
    ASSERT_NO_THROW(_function = parser.function());
    ASSERT_EQ(_function->getNodeType(), ASTNodeType::FUNCTION);
    ASSERT_EQ(_function->getParameterCount(), expectedParamCount);
}

TEST(ParserTest, FunctionNoBodyThrows){
    std::vector<std::string> input{"int main() return 0;"};
    LexerTest lexer{ { input } };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTFunction> _function;
    ASSERT_THROW(_function = parser.function(), std::runtime_error);
}

TEST(ParserTest, VariableDirectInit){
    std::vector<std::string> input{"int x = a + fun(b,c);"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTVariable> _variable;
    ASSERT_NO_THROW(_variable = parser.variable());
    ASSERT_EQ(_variable->getNodeType(), ASTNodeType::VARIABLE);
    ASSERT_TRUE(_variable->hasAssign());
}

TEST(ParserTest, VariableNoInit){
    std::vector<std::string> input{"int x;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };


    std::unique_ptr<ASTVariable> _variable;
    ASSERT_NO_THROW(_variable = parser.variable());
    ASSERT_EQ(_variable->getNodeType(), ASTNodeType::VARIABLE);
    ASSERT_FALSE(_variable->hasAssign());
}

TEST(ParserTest, EmptyCompoundStatement){
    std::vector<std::string> input{"{}"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    const size_t expectedChildrenSize = 0;

    std::unique_ptr<ASTCompoundSt> _compound;
    ASSERT_NO_THROW(_compound = parser.compoundStatement());
    ASSERT_EQ(_compound->getNodeType(), ASTNodeType::COMPOUND_STATEMENT);
    ASSERT_EQ(_compound->getStatements().size(), expectedChildrenSize);
}

TEST(ParserTest, CompoundStatement){
    std::vector<std::string> input{"{ int x = 3; int y = x; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    const size_t expectedChildrenSize = 2;

    std::unique_ptr<ASTCompoundSt> _compound;
    ASSERT_NO_THROW(_compound = parser.compoundStatement());
    ASSERT_EQ(_compound->getNodeType(), ASTNodeType::COMPOUND_STATEMENT);
    ASSERT_EQ(_compound->getStatements().size(), expectedChildrenSize);
}

TEST(ParserTest, AssignmentStatement){
    std::vector<std::string> input{"x = 123u;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTAssignSt> _assignment;
    ASSERT_NO_THROW(_assignment = parser.assignmentStatement());
    ASSERT_EQ(_assignment->getNodeType(), ASTNodeType::ASSIGNMENT_STATEMENT);
    ASSERT_TRUE(_assignment->getExp()->getNodeType() == ASTNodeType::LITERAL);
}

TEST(ParserTest, ReturnStatementVoid){
    std::vector<std::string> input{"return;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTReturnSt> _return;
    ASSERT_NO_THROW(_return = parser.returnStatement());
    ASSERT_EQ(_return->getNodeType(), ASTNodeType::RETURN_STATEMENT);
    ASSERT_FALSE(_return->returns());
}

TEST(ParserTest, ReturnStatement){
    std::vector<std::string> input{"return fun(1, 2);"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTReturnSt> _return;
    ASSERT_NO_THROW(_return = parser.returnStatement());
    ASSERT_EQ(_return->getNodeType(), ASTNodeType::RETURN_STATEMENT);
    ASSERT_TRUE(_return->returns());
}

TEST(ParserTest, IfStatementOnlyIf){
    std::vector<std::string> input{"if(a > b) return a;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    const size_t expectedConditionCount = 1;

    std::unique_ptr<ASTIfSt> _if;
    ASSERT_NO_THROW(_if = parser.ifStatement());
    ASSERT_EQ(_if->getNodeType(), ASTNodeType::IF_STATEMENT);
    ASSERT_EQ(_if->getConditions().size(), expectedConditionCount);
    ASSERT_FALSE(_if->hasElse());
}

TEST(ParserTest, IfStatementIfElse){
    std::vector<std::string> input{"if(a > b) return a; else return b;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    const size_t expectedStatementCount = 2;

    std::unique_ptr<ASTIfSt> _if;
    ASSERT_NO_THROW(_if = parser.ifStatement());
    ASSERT_EQ(_if->getNodeType(), ASTNodeType::IF_STATEMENT);
    ASSERT_EQ(_if->getStatements().size(), expectedStatementCount);
    ASSERT_TRUE(_if->hasElse());
}

TEST(ParserTest, IfStatementIfElseif){
    std::vector<std::string> input{"if(a > b) return a; else if(a == b) return b;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    const size_t expectedConditionCount = 2;

    std::unique_ptr<ASTIfSt> _if;
    ASSERT_NO_THROW(_if = parser.ifStatement());
    ASSERT_EQ(_if->getNodeType(), ASTNodeType::IF_STATEMENT);
    ASSERT_EQ(_if->getConditions().size(), expectedConditionCount);
}

TEST(ParserTest, IfStatementIfElseifElse){
    std::vector<std::string> input{"if(a > b) return a; else if(a == b) return 0; else return b;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    const size_t expectedStatementCount = 3;

    std::unique_ptr<ASTIfSt> _if;
    ASSERT_NO_THROW(_if = parser.ifStatement());
    ASSERT_EQ(_if->getNodeType(), ASTNodeType::IF_STATEMENT);
    ASSERT_EQ(_if->getStatements().size(), expectedStatementCount);
    ASSERT_TRUE(_if->hasElse());
}

TEST(ParserTest, WhileStatement){
    std::vector<std::string> input{"while(a > b) b = b + 1;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTWhileSt> _while;
    ASSERT_NO_THROW(_while = parser.whileStatement());
    ASSERT_EQ(_while->getNodeType(), ASTNodeType::WHILE_STATEMENT);
}

TEST(ParserTest, ForStatement){
    std::vector<std::string> input{"for(i = 0; i < 5; i = i + 1) i = i + 1;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTForSt> _for;
    ASSERT_NO_THROW(_for = parser.forStatement());
    ASSERT_EQ(_for->getNodeType(), ASTNodeType::FOR_STATEMENT);
    ASSERT_TRUE(_for->hasInitializer());
    ASSERT_TRUE(_for->hasCondition());
    ASSERT_TRUE(_for->hasIncrementer());
}

TEST(ParserTest, DoWhileStatement){
    std::vector<std::string> input{"do x = x + 1; while(x < 10);"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTDoWhileSt> _dowhile;
    ASSERT_NO_THROW(_dowhile = parser.doWhileStatement());
    ASSERT_EQ(_dowhile->getNodeType(), ASTNodeType::DO_WHILE_STATEMENT);
}

TEST(ParserTest, SwitchStatement){
    std::vector<std::string> input{"switch(x){ case 1: break; default: return 1; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    const size_t expectedCaseCount = 1;

    std::unique_ptr<ASTSwitchSt> _switch;
    ASSERT_NO_THROW(_switch = parser.switchStatement());
    ASSERT_EQ(_switch->getNodeType(), ASTNodeType::SWITCH_STATEMENT);
    ASSERT_EQ(_switch->getCases().size(), expectedCaseCount);
    ASSERT_TRUE(_switch->hasDefault());  
}

TEST(ParserTest, SwitchStatementNoDefault){
    std::vector<std::string> input{"switch(x){ case 1: break; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    const size_t expectedCaseCount = 1;

    std::unique_ptr<ASTSwitchSt> _switch;
    ASSERT_NO_THROW(_switch = parser.switchStatement());
    ASSERT_EQ(_switch->getNodeType(), ASTNodeType::SWITCH_STATEMENT);
    ASSERT_EQ(_switch->getCases().size(), expectedCaseCount);
    ASSERT_FALSE(_switch->hasDefault());   
}

TEST(ParserTest, SwitchStatementNoCaseThrows){
    std::vector<std::string> input{"switch(x){ default: return 1; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTSwitchSt> _switch;
    ASSERT_THROW(_switch = parser.switchStatement(), std::runtime_error);
}

TEST(ParserTest, NumericalExpression){
    std::vector<std::string> input{"1 + 2"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    ExpressionParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTExpression> _numexp;
    ASSERT_NO_THROW(_numexp = parser.numericalExpression());
    ASSERT_EQ(_numexp->getNodeType(), ASTNodeType::BINARY_EXPRESSION);
    ASSERT_EQ(static_cast<ASTBinaryExpression*>(_numexp.get())->getOperator(), Operators::ADD);
}

TEST(ParserTest, RelationalExpression){
    std::vector<std::string> input{"1 + 2 > a + b"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    ExpressionParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTExpression> _relexp;
    ASSERT_NO_THROW(_relexp = parser.relationalExpression());
    ASSERT_EQ(_relexp->getNodeType(), ASTNodeType::BINARY_EXPRESSION);
    ASSERT_EQ(static_cast<ASTBinaryExpression*>(_relexp.get())->getOperator(), Operators::GREATER);
}

TEST(ParserTest, FunctionCallNoArgs){
    std::vector<std::string> input{"fun()"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    ExpressionParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTFunctionCall> _functionCall;
    ASSERT_NO_THROW(_functionCall = parser.functionCall());
    ASSERT_EQ(_functionCall->getNodeType(), ASTNodeType::FUNCTION_CALL);
    ASSERT_TRUE(_functionCall->getArgumentCount() == 0);
    ASSERT_EQ(_functionCall->getToken().value, "fun");
}

TEST(ParserTest, FunctionCallSingleArg){
    std::vector<std::string> input{"fun(x)"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    ExpressionParserTest parser{ tokenConsumer };

    const size_t expectedArgumentCount = 1;

    std::unique_ptr<ASTFunctionCall> _functionCall;
    ASSERT_NO_THROW(_functionCall = parser.functionCall());
    ASSERT_EQ(_functionCall->getNodeType(), ASTNodeType::FUNCTION_CALL);
    ASSERT_EQ(_functionCall->getArgumentCount(), expectedArgumentCount);
    ASSERT_EQ(_functionCall->getToken().value, "fun");
}

TEST(ParserTest, FunctionCallMultipleArgs){
    std::vector<std::string> input{"fun(x, y)"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    ExpressionParserTest parser{ tokenConsumer };

    const size_t expectedArgumentCount = 2;

    std::unique_ptr<ASTFunctionCall> _functionCall;
    ASSERT_NO_THROW(_functionCall = parser.functionCall());
    ASSERT_EQ(_functionCall->getNodeType(), ASTNodeType::FUNCTION_CALL);
    ASSERT_EQ(_functionCall->getArgumentCount(), expectedArgumentCount);
    ASSERT_EQ(_functionCall->getToken().value, "fun");
}

TEST(ParserTest, FunctionCallInvalidArgs){
    std::vector<std::string> input{"fun(x y)"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    ExpressionParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTFunctionCall> _functionCall;
    ASSERT_THROW(_functionCall = parser.functionCall(), std::runtime_error);
}
