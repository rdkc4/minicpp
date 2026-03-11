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

    std::unique_ptr<ASTProgram> program;
    ASSERT_NO_THROW(program = parser.parseProgram());
    ASSERT_EQ(program->getNodeType(), ASTNodeType::PROGRAM);
    ASSERT_EQ(program->getFunctionCount(), expectedFunctionCount);
}

TEST(ParserTest, ParseProgramThrows){
    std::vector<std::string> input{"main(){ int a = 1 + 2; return a; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTProgram> program;
    ASSERT_THROW(program = parser.parseProgram(), std::runtime_error);
}

TEST(ParserTest, FunctionMultipleParams){
    std::vector<std::string> input{"int rectArea(int a, int b){ return a * b; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };

    size_t expectedParamCount = 2;

    std::unique_ptr<ASTFunction> function;
    ASSERT_NO_THROW(function = parser.parseFunction());
    ASSERT_EQ(function->getNodeType(), ASTNodeType::FUNCTION);
    ASSERT_EQ(function->getParameterCount(), expectedParamCount);
}

TEST(ParserTest, FunctionSingleParam){
    std::vector<std::string> input{"int sq(int a){ return a * a; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };

    size_t expectedParamCount = 1;

    std::unique_ptr<ASTFunction> function;
    ASSERT_NO_THROW(function = parser.parseFunction());
    ASSERT_EQ(function->getNodeType(), ASTNodeType::FUNCTION);
    ASSERT_EQ(function->getParameterCount(), expectedParamCount);
}

TEST(ParserTest, FunctionNoParams){
    std::vector<std::string> input{"int main(){ return 0; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };

    size_t expectedParamCount = 0;

    std::unique_ptr<ASTFunction> function;
    ASSERT_NO_THROW(function = parser.parseFunction());
    ASSERT_EQ(function->getNodeType(), ASTNodeType::FUNCTION);
    ASSERT_EQ(function->getParameterCount(), expectedParamCount);
}

TEST(ParserTest, FunctionNoBodyThrows){
    std::vector<std::string> input{"int main() return 0;"};
    LexerTest lexer{ { input } };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTFunction> function;
    ASSERT_THROW(function = parser.parseFunction(), std::runtime_error);
}

TEST(ParserTest, VariableDirectInit){
    std::vector<std::string> input{"int x = a + fun(b,c);"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTVariableDeclStmt> variableDecl;
    ASSERT_NO_THROW(variableDecl = parser.parseVariableDeclStmt());
    ASSERT_EQ(variableDecl->getNodeType(), ASTNodeType::VARIABLE);
    ASSERT_TRUE(variableDecl->hasAssign());
}

TEST(ParserTest, VariableNoInit){
    std::vector<std::string> input{"int x;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };


    std::unique_ptr<ASTVariableDeclStmt> variableDecl;
    ASSERT_NO_THROW(variableDecl = parser.parseVariableDeclStmt());
    ASSERT_EQ(variableDecl->getNodeType(), ASTNodeType::VARIABLE);
    ASSERT_FALSE(variableDecl->hasAssign());
}

TEST(ParserTest, EmptyCompoundStatement){
    std::vector<std::string> input{"{}"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    const size_t expectedChildrenSize = 0;

    std::unique_ptr<ASTCompoundStmt> compoundStmt;
    ASSERT_NO_THROW(compoundStmt = parser.parseCompoundStmt());
    ASSERT_EQ(compoundStmt->getNodeType(), ASTNodeType::COMPOUND_STATEMENT);
    ASSERT_EQ(compoundStmt->getStatements().size(), expectedChildrenSize);
}

TEST(ParserTest, CompoundStatement){
    std::vector<std::string> input{"{ int x = 3; int y = x; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    const size_t expectedChildrenSize = 2;

    std::unique_ptr<ASTCompoundStmt> compoundStmt;
    ASSERT_NO_THROW(compoundStmt = parser.parseCompoundStmt());
    ASSERT_EQ(compoundStmt->getNodeType(), ASTNodeType::COMPOUND_STATEMENT);
    ASSERT_EQ(compoundStmt->getStatements().size(), expectedChildrenSize);
}

TEST(ParserTest, AssignmentStatement){
    std::vector<std::string> input{"x = 123u;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTAssignStmt> assignStmt;
    ASSERT_NO_THROW(assignStmt = parser.parseAssignStmt());
    ASSERT_EQ(assignStmt->getNodeType(), ASTNodeType::ASSIGNMENT_STATEMENT);
    ASSERT_TRUE(assignStmt->getExp()->getNodeType() == ASTNodeType::LITERAL);
}

TEST(ParserTest, ReturnStatementVoid){
    std::vector<std::string> input{"return;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTReturnStmt> returnStmt;
    ASSERT_NO_THROW(returnStmt = parser.parseReturnStmt());
    ASSERT_EQ(returnStmt->getNodeType(), ASTNodeType::RETURN_STATEMENT);
    ASSERT_FALSE(returnStmt->returns());
}

TEST(ParserTest, ReturnStatement){
    std::vector<std::string> input{"return fun(1, 2);"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTReturnStmt> returnStmt;
    ASSERT_NO_THROW(returnStmt = parser.parseReturnStmt());
    ASSERT_EQ(returnStmt->getNodeType(), ASTNodeType::RETURN_STATEMENT);
    ASSERT_TRUE(returnStmt->returns());
}

TEST(ParserTest, IfStatementOnlyIf){
    std::vector<std::string> input{"if(a > b) return a;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    const size_t expectedConditionCount = 1;

    std::unique_ptr<ASTIfStmt> ifStmt;
    ASSERT_NO_THROW(ifStmt = parser.parseIfStmt());
    ASSERT_EQ(ifStmt->getNodeType(), ASTNodeType::IF_STATEMENT);
    ASSERT_EQ(ifStmt->getConditions().size(), expectedConditionCount);
    ASSERT_FALSE(ifStmt->hasElse());
}

TEST(ParserTest, IfStatementIfElse){
    std::vector<std::string> input{"if(a > b) return a; else return b;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    const size_t expectedStatementCount = 2;

    std::unique_ptr<ASTIfStmt> ifStmt;
    ASSERT_NO_THROW(ifStmt = parser.parseIfStmt());
    ASSERT_EQ(ifStmt->getNodeType(), ASTNodeType::IF_STATEMENT);
    ASSERT_EQ(ifStmt->getStatements().size(), expectedStatementCount);
    ASSERT_TRUE(ifStmt->hasElse());
}

TEST(ParserTest, IfStatementIfElseif){
    std::vector<std::string> input{"if(a > b) return a; else if(a == b) return b;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    const size_t expectedConditionCount = 2;

    std::unique_ptr<ASTIfStmt> ifStmt;
    ASSERT_NO_THROW(ifStmt = parser.parseIfStmt());
    ASSERT_EQ(ifStmt->getNodeType(), ASTNodeType::IF_STATEMENT);
    ASSERT_EQ(ifStmt->getConditions().size(), expectedConditionCount);
}

TEST(ParserTest, IfStatementIfElseifElse){
    std::vector<std::string> input{"if(a > b) return a; else if(a == b) return 0; else return b;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    const size_t expectedStatementCount = 3;

    std::unique_ptr<ASTIfStmt> ifStmt;
    ASSERT_NO_THROW(ifStmt = parser.parseIfStmt());
    ASSERT_EQ(ifStmt->getNodeType(), ASTNodeType::IF_STATEMENT);
    ASSERT_EQ(ifStmt->getStatements().size(), expectedStatementCount);
    ASSERT_TRUE(ifStmt->hasElse());
}

TEST(ParserTest, WhileStatement){
    std::vector<std::string> input{"while(a > b) b = b + 1;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTWhileStmt> whileStmt;
    ASSERT_NO_THROW(whileStmt = parser.parseWhileStmt());
    ASSERT_EQ(whileStmt->getNodeType(), ASTNodeType::WHILE_STATEMENT);
}

TEST(ParserTest, ForStatement){
    std::vector<std::string> input{"for(i = 0; i < 5; i = i + 1) i = i + 1;"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTForStmt> forStmt;
    ASSERT_NO_THROW(forStmt = parser.parseForStmt());
    ASSERT_EQ(forStmt->getNodeType(), ASTNodeType::FOR_STATEMENT);
    ASSERT_TRUE(forStmt->hasInitializer());
    ASSERT_TRUE(forStmt->hasCondition());
    ASSERT_TRUE(forStmt->hasIncrementer());
}

TEST(ParserTest, DoWhileStatement){
    std::vector<std::string> input{"do x = x + 1; while(x < 10);"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTDoWhileStmt> dowhileStmt;
    ASSERT_NO_THROW(dowhileStmt = parser.parseDoWhileStmt());
    ASSERT_EQ(dowhileStmt->getNodeType(), ASTNodeType::DO_WHILE_STATEMENT);
}

TEST(ParserTest, SwitchStatement){
    std::vector<std::string> input{"switch(x){ case 1: break; default: return 1; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    const size_t expectedCaseCount = 1;

    std::unique_ptr<ASTSwitchStmt> switchStmt;
    ASSERT_NO_THROW(switchStmt = parser.parseSwitchStmt());
    ASSERT_EQ(switchStmt->getNodeType(), ASTNodeType::SWITCH_STATEMENT);
    ASSERT_EQ(switchStmt->getCases().size(), expectedCaseCount);
    ASSERT_TRUE(switchStmt->hasDefault());  
}

TEST(ParserTest, SwitchStatementNoDefault){
    std::vector<std::string> input{"switch(x){ case 1: break; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    const size_t expectedCaseCount = 1;

    std::unique_ptr<ASTSwitchStmt> switchStmt;
    ASSERT_NO_THROW(switchStmt = parser.parseSwitchStmt());
    ASSERT_EQ(switchStmt->getNodeType(), ASTNodeType::SWITCH_STATEMENT);
    ASSERT_EQ(switchStmt->getCases().size(), expectedCaseCount);
    ASSERT_FALSE(switchStmt->hasDefault());   
}

TEST(ParserTest, SwitchStatementNoCaseThrows){
    std::vector<std::string> input{"switch(x){ default: return 1; }"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTSwitchStmt> switchStmt;
    ASSERT_THROW(switchStmt = parser.parseSwitchStmt(), std::runtime_error);
}

TEST(ParserTest, NumericalExpression){
    std::vector<std::string> input{"1 + 2"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    ExpressionParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTExpr> numericalExpr;
    ASSERT_NO_THROW(numericalExpr = parser.parseNumericalExpr());
    ASSERT_EQ(numericalExpr->getNodeType(), ASTNodeType::BINARY_EXPRESSION);
    ASSERT_EQ(static_cast<ASTBinaryExpr*>(numericalExpr.get())->getOperator(), Operator::ADD);
}

TEST(ParserTest, RelationalExpression){
    std::vector<std::string> input{"1 + 2 > a + b"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    ExpressionParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTExpr> relationalExpr;
    ASSERT_NO_THROW(relationalExpr = parser.parseRelationalExpr());
    ASSERT_EQ(relationalExpr->getNodeType(), ASTNodeType::BINARY_EXPRESSION);
    ASSERT_EQ(static_cast<ASTBinaryExpr*>(relationalExpr.get())->getOperator(), Operator::GREATER);
}

TEST(ParserTest, FunctionCallNoArgs){
    std::vector<std::string> input{"fun()"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    ExpressionParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTFunctionCallExpr> callExpr;
    ASSERT_NO_THROW(callExpr = parser.parseFunctionCallExpr());
    ASSERT_EQ(callExpr->getNodeType(), ASTNodeType::FUNCTION_CALL);
    ASSERT_TRUE(callExpr->getArgumentCount() == 0);
    ASSERT_EQ(callExpr->getToken().value, "fun");
}

TEST(ParserTest, FunctionCallSingleArg){
    std::vector<std::string> input{"fun(x)"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    ExpressionParserTest parser{ tokenConsumer };

    const size_t expectedArgumentCount = 1;

    std::unique_ptr<ASTFunctionCallExpr> callExpr;
    ASSERT_NO_THROW(callExpr = parser.parseFunctionCallExpr());
    ASSERT_EQ(callExpr->getNodeType(), ASTNodeType::FUNCTION_CALL);
    ASSERT_EQ(callExpr->getArgumentCount(), expectedArgumentCount);
    ASSERT_EQ(callExpr->getToken().value, "fun");
}

TEST(ParserTest, FunctionCallMultipleArgs){
    std::vector<std::string> input{"fun(x, y)"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    ExpressionParserTest parser{ tokenConsumer };

    const size_t expectedArgumentCount = 2;

    std::unique_ptr<ASTFunctionCallExpr> callExpr;
    ASSERT_NO_THROW(callExpr = parser.parseFunctionCallExpr());
    ASSERT_EQ(callExpr->getNodeType(), ASTNodeType::FUNCTION_CALL);
    ASSERT_EQ(callExpr->getArgumentCount(), expectedArgumentCount);
    ASSERT_EQ(callExpr->getToken().value, "fun");
}

TEST(ParserTest, FunctionCallInvalidArgs){
    std::vector<std::string> input{"fun(x y)"};
    LexerTest lexer{ input };
    lexer.tokenize();
    TokenConsumer tokenConsumer { lexer };
    ExpressionParserTest parser{ tokenConsumer };

    std::unique_ptr<ASTFunctionCallExpr> callExpr;
    ASSERT_THROW(callExpr = parser.parseFunctionCallExpr(), std::runtime_error);
}
