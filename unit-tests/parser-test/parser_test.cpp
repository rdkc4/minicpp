#include <gtest/gtest.h>
#include <stdexcept>

#include "parser_fixture.hpp"

TEST_F(ParserFixture, ParseProgramSuccessful){
    input = {"int fun(){ return 123; } int main(){ int a = 1 + 2; return a; }" };
    ASSERT_NO_THROW(initParser());

    const size_t expectedFunctionCount{2};

    ASSERT_EQ(program->getNodeType(), ASTNodeType::PROGRAM);
    EXPECT_EQ(program->getFunctionCount(), expectedFunctionCount);
}

TEST_F(ParserFixture, ParseProgramThrows){
    input = {"main(){ int a = 1 + 2; return a; }"};
    ASSERT_THROW(initParser(), std::runtime_error);
}

TEST_F(FunctionParserFixture, FunctionMultipleParams){
    input = {"int rectArea(int a, int b){ return a * b; }"};
    ASSERT_NO_THROW(initParser());

    size_t expectedParamCount = 2;

    ASSERT_EQ(function->getNodeType(), ASTNodeType::FUNCTION);
    EXPECT_EQ(function->getParameterCount(), expectedParamCount);
}

TEST_F(FunctionParserFixture, FunctionSingleParam){
    input = {"int sq(int a){ return a * a; }"};
    ASSERT_NO_THROW(initParser());

    size_t expectedParamCount = 1;

    ASSERT_EQ(function->getNodeType(), ASTNodeType::FUNCTION);
    EXPECT_EQ(function->getParameterCount(), expectedParamCount);
}

TEST_F(FunctionParserFixture, FunctionNoParams){
    input = {"int main(){ return 0; }"};
    ASSERT_NO_THROW(initParser());

    size_t expectedParamCount = 0;

    ASSERT_EQ(function->getNodeType(), ASTNodeType::FUNCTION);
    EXPECT_EQ(function->getParameterCount(), expectedParamCount);
}

TEST_F(FunctionParserFixture, FunctionNoBodyThrows){
    input = {"int main() return 0;"};
    ASSERT_THROW(initParser(), std::runtime_error);
}

TEST_F(StatementParserFixture, VariableDirectInit){
    input = {"int x = a + fun(b,c);"};
    
    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(stmt->getNodeType(), ASTNodeType::VARIABLE_DECL_STMT);
    EXPECT_TRUE(static_cast<ASTVariableDeclStmt*>(stmt.get())->hasAssignExpr());
}

TEST_F(StatementParserFixture, VariableNoInit){
    input = {"int x;"};

    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(stmt->getNodeType(), ASTNodeType::VARIABLE_DECL_STMT);
    EXPECT_FALSE(static_cast<ASTVariableDeclStmt*>(stmt.get())->hasAssignExpr());
}

TEST_F(StatementParserFixture, EmptyCompoundStatement){
    input = {"{}"};

    const size_t expectedChildrenSize = 0;

    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(stmt->getNodeType(), ASTNodeType::COMPOUND_STMT);
    EXPECT_EQ(static_cast<ASTCompoundStmt*>(stmt.get())->getStmts().size(), expectedChildrenSize);
}

TEST_F(StatementParserFixture, CompoundStatement){
    input = {"{ int x = 3; int y = x; }"};

    const size_t expectedChildrenSize = 2;

    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(stmt->getNodeType(), ASTNodeType::COMPOUND_STMT);
    EXPECT_EQ(static_cast<ASTCompoundStmt*>(stmt.get())->getStmts().size(), expectedChildrenSize);
}

TEST_F(StatementParserFixture, AssignmentStatement){
    input = {"x = 123u;"};

    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(stmt->getNodeType(), ASTNodeType::ASSIGN_STMT);
    EXPECT_TRUE(static_cast<ASTAssignStmt*>(stmt.get())->getAssignedExpr()->getNodeType() == ASTNodeType::LITERAL_EXPR);
}

TEST_F(StatementParserFixture, ReturnStatementVoid){
    input = {"return;"};

    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(stmt->getNodeType(), ASTNodeType::RETURN_STMT);
    EXPECT_FALSE(static_cast<ASTReturnStmt*>(stmt.get())->hasReturnExpr());
}

TEST_F(StatementParserFixture, ReturnStatement){
    input = {"return fun(1, 2);"};

    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(stmt->getNodeType(), ASTNodeType::RETURN_STMT);
    EXPECT_TRUE(static_cast<ASTReturnStmt*>(stmt.get())->hasReturnExpr());
}

TEST_F(StatementParserFixture, IfStatementOnlyIf){
    input = {"if(a > b) return a;"};

    const size_t expectedConditionCount = 1;

    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(stmt->getNodeType(), ASTNodeType::IF_STMT);

    auto ifStmt{ static_cast<ASTIfStmt*>(stmt.get()) };
    EXPECT_EQ(ifStmt->getConditionExprs().size(), expectedConditionCount);
    EXPECT_FALSE(ifStmt->hasElseStmt());
}

TEST_F(StatementParserFixture, IfStatementIfElse){
    input = {"if(a > b) return a; else return b;"};

    const size_t expectedStatementCount = 2;

    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(stmt->getNodeType(), ASTNodeType::IF_STMT);

    auto ifStmt{ static_cast<ASTIfStmt*>(stmt.get()) };
    EXPECT_EQ(ifStmt->getStmts().size(), expectedStatementCount);
    EXPECT_TRUE(ifStmt->hasElseStmt());
}

TEST_F(StatementParserFixture, IfStatementIfElseif){
    input = {"if(a > b) return a; else if(a == b) return b;"};

    const size_t expectedConditionCount = 2;

    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(stmt->getNodeType(), ASTNodeType::IF_STMT);
    EXPECT_EQ(static_cast<ASTIfStmt*>(stmt.get())->getConditionExprs().size(), expectedConditionCount);
}

TEST_F(StatementParserFixture, IfStatementIfElseifElse){
    input = {"if(a > b) return a; else if(a == b) return 0; else return b;"};

    const size_t expectedStatementCount = 3;

    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(stmt->getNodeType(), ASTNodeType::IF_STMT);

    auto ifStmt{ static_cast<ASTIfStmt*>(stmt.get()) };
    EXPECT_EQ(ifStmt->getStmts().size(), expectedStatementCount);
    EXPECT_TRUE(ifStmt->hasElseStmt());
}

TEST_F(StatementParserFixture, WhileStatement){
    input = {"while(a > b) b = b + 1;"};
    
    ASSERT_NO_THROW(initParser());
    EXPECT_EQ(stmt->getNodeType(), ASTNodeType::WHILE_STMT);
}

TEST_F(StatementParserFixture, ForStatement){
    input = {"for(i = 0; i < 5; i = i + 1) i = i + 1;"};
    
    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(stmt->getNodeType(), ASTNodeType::FOR_STMT);

    auto forStmt{ static_cast<ASTForStmt*>(stmt.get()) };
    EXPECT_TRUE(forStmt->hasInitializerStmt());
    EXPECT_TRUE(forStmt->hasConditionExpr());
    EXPECT_TRUE(forStmt->hasIncrementerStmt());
}

TEST_F(StatementParserFixture, DoWhileStatement){
    input = {"do x = x + 1; while(x < 10);"};
    
    ASSERT_NO_THROW(initParser());
    EXPECT_EQ(stmt->getNodeType(), ASTNodeType::DOWHILE_STMT);
}

TEST_F(StatementParserFixture, SwitchStatement){
    input = {"switch(x){ case 1: break; default: return 1; }"};

    const size_t expectedCaseCount = 1;

    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(stmt->getNodeType(), ASTNodeType::SWITCH_STMT);

    auto switchStmt{ static_cast<ASTSwitchStmt*>(stmt.get()) };
    EXPECT_EQ(switchStmt->getCaseStmts().size(), expectedCaseCount);
    EXPECT_TRUE(switchStmt->hasDefaultStmt());  
}

TEST_F(StatementParserFixture, SwitchStatementNoDefault){
    input = {"switch(x){ case 1: break; }"};

    const size_t expectedCaseCount = 1;

    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(stmt->getNodeType(), ASTNodeType::SWITCH_STMT);

    auto switchStmt{ static_cast<ASTSwitchStmt*>(stmt.get()) };
    EXPECT_EQ(switchStmt->getCaseStmts().size(), expectedCaseCount);
    EXPECT_FALSE(switchStmt->hasDefaultStmt());   
}

TEST_F(StatementParserFixture, SwitchStatementNoCaseThrows){
    input = {"switch(x){ default: return 1; }"};
    
    ASSERT_THROW(initParser(), std::runtime_error);
}

TEST_F(ExpressionParserFixture, Expression){
    input = {"1 + 2"};

    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(expr->getNodeType(), ASTNodeType::BINARY_EXPR);
    EXPECT_EQ(static_cast<ASTBinaryExpr*>(expr.get())->getOperator(), Operator::ADD);
}

TEST_F(ExpressionParserFixture, ConditionExpression){
    input = {"1 + 2 > a + b"};

    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(expr->getNodeType(), ASTNodeType::BINARY_EXPR);
    EXPECT_EQ(static_cast<ASTBinaryExpr*>(expr.get())->getOperator(), Operator::GREATER);
}

TEST_F(ExpressionParserFixture, FunctionCallNoArgs){
    input = {"fun()"};
    
    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(expr->getNodeType(), ASTNodeType::FUNCTION_CALL_EXPR);

    auto callExpr{ static_cast<ASTFunctionCallExpr*>(expr.get()) };
    EXPECT_TRUE(callExpr->getArgumentCount() == 0);
    EXPECT_EQ(callExpr->getToken().value, "fun");
}

TEST_F(ExpressionParserFixture, FunctionCallSingleArg){
    input = {"fun(x)"};

    const size_t expectedArgumentCount = 1;

    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(expr->getNodeType(), ASTNodeType::FUNCTION_CALL_EXPR);

    auto callExpr{ static_cast<ASTFunctionCallExpr*>(expr.get()) };
    EXPECT_EQ(callExpr->getArgumentCount(), expectedArgumentCount);
    EXPECT_EQ(callExpr->getToken().value, "fun");
}

TEST_F(ExpressionParserFixture, FunctionCallMultipleArgs){
    input = {"fun(x, y)"};

    const size_t expectedArgumentCount = 2;

    ASSERT_NO_THROW(initParser());
    ASSERT_EQ(expr->getNodeType(), ASTNodeType::FUNCTION_CALL_EXPR);

    auto callExpr{ static_cast<ASTFunctionCallExpr*>(expr.get()) };
    EXPECT_EQ(callExpr->getArgumentCount(), expectedArgumentCount);
    EXPECT_EQ(callExpr->getToken().value, "fun");
}

TEST_F(ExpressionParserFixture, FunctionCallInvalidArgs){
    input = {"fun(x y)"};

    ASSERT_THROW(initParser(), std::runtime_error);
}
