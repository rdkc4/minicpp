#include <gtest/gtest.h>
#include <memory>

#include "intermediate_representation_test.hpp"
#include "../lexer-test/lexer_test.hpp"
#include "../parser-test/parser_test.hpp"

TEST(IRTest, FormIR){
    std::vector<std::string> input{"int fun(){ return 1; } int main(){ return fun(); }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTProgram> astProgram = parser.parseProgram();

    ThreadPool tp{1};
    IntermediateRepresentationTest intermediateRepresentation{tp};
    std::unique_ptr<IRProgram> irProgram = intermediateRepresentation.transformProgram(astProgram.get());
    const size_t functionCount = 2;

    ASSERT_TRUE(irProgram->getNodeType() == IRNodeType::PROGRAM);
    ASSERT_TRUE(irProgram->getFunctionCount() == functionCount);
}

TEST(IRTest, FormIRThrows){
    std::vector<std::string> input{"int main(){ return 1/0; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTProgram> astProgram = parser.parseProgram();

    ThreadPool tp{1};
    IntermediateRepresentationTest intermediateRepresentation{tp};
    std::unique_ptr<IRProgram> irProgram = intermediateRepresentation.transformProgram(astProgram.get());

    ASSERT_TRUE(intermediateRepresentation.hasErrors(irProgram.get()));
    ASSERT_TRUE(intermediateRepresentation.getErrors("main")[0].contains("division by ZERO"));
}

TEST(IRTest, FunctionDeadCodeElimination){
    std::vector<std::string> input{"int fun(int x){ return 0; x = 1; return x; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTProgram> astProgram = parser.parseProgram();

    ThreadPool tp{1};
    IntermediateRepresentationTest intermediateRepresentation{tp};
    std::unique_ptr<IRProgram> irProgram = intermediateRepresentation.transformProgram(astProgram.get());

    const size_t expectedStmtCount = 1;

    ASSERT_EQ(irProgram->getFunctionCount(), 1);
    ASSERT_EQ(irProgram->getFunctionAtN(0)->getBody().size(), expectedStmtCount);
}

TEST(IRTest, FunctionDeadCodeEliminationIfBranching){
    std::vector<std::string> input{"int fun(int x){ if(x > 1) return 0; else return 1; x = x + 1; return x; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTProgram> astProgram = parser.parseProgram();

    ThreadPool tp{1};
    IntermediateRepresentationTest intermediateRepresentation{tp};
    std::unique_ptr<IRProgram> irProgram = intermediateRepresentation.transformProgram(astProgram.get());
    
    const size_t expectedStmtCount = 1;

    ASSERT_EQ(irProgram->getFunctionCount(), 1);
    ASSERT_EQ(irProgram->getFunctionAtN(0)->getBody().size(), expectedStmtCount);
}

TEST(IRTest, FunctionDeadCodeEliminationSwitchBranching){
    std::vector<std::string> input{"int fun(int x){ switch(x) { case 0: case 1: return 0; default: return 1; } x = x + 1; return x; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTProgram> astProgram = parser.parseProgram();

    ThreadPool tp{1};
    IntermediateRepresentationTest intermediateRepresentation{tp};
    std::unique_ptr<IRProgram> irProgram = intermediateRepresentation.transformProgram(astProgram.get());
    
    const size_t expectedStmtCount = 1; 
    
    ASSERT_EQ(irProgram->getFunctionCount(), 1);
    ASSERT_TRUE(irProgram->getFunctionAtN(0)->getBody().size() == expectedStmtCount);
}

TEST(IRTest, FunctionDeadCodeEliminationDoWhile){
    std::vector<std::string> input{"int fun(int x){ do{ if(x > 0) return 0; else return 1; x = x - 1; } while(x>0); return x; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTProgram> astProgram = parser.parseProgram();

    ThreadPool tp{1};
    IntermediateRepresentationTest intermediateRepresentation{tp};
    std::unique_ptr<IRProgram> irProgram = intermediateRepresentation.transformProgram(astProgram.get());

    const size_t expectedStmtCount = 1;

    ASSERT_EQ(irProgram->getFunctionCount(), 1);
    ASSERT_EQ(irProgram->getFunctionAtN(0)->getBody().size(), expectedStmtCount);
}

TEST(IRTest, CompoundStatementDeadCodeElimination){
    std::vector<std::string> input{"{ return 0; int x = 1; return x; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTCompoundStmt> astCompoundStmt = parser.parseCompoundStmt();

    StatementIntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRCompoundStmt> irCompoundStmt = intermediateRepresentation.transformCompoundStmt(astCompoundStmt.get());

    const size_t expectedStmtCount = 1;

    ASSERT_TRUE(irCompoundStmt->getNodeType() == IRNodeType::COMPOUND);
    ASSERT_TRUE(irCompoundStmt->getStmts().size() == expectedStmtCount);
}

TEST(IRTest, AssignmentStatementGeneratesTemporaries){
    std::vector<std::string> input{"x = fun(fun(1, 2), 1);"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTAssignStmt> astAssignStmt = parser.parseAssignStmt();

    StatementIntermediateRepresentationTest intermediateRepresentation;

    auto& context = FunctionIntermediateRepresentationTest::getContext();
    context.init();
    const size_t expectedTemporariesCount = 2;

    std::unique_ptr<IRAssignStmt> irAssignStmt = intermediateRepresentation.transformAssignStmt(astAssignStmt.get());
    ASSERT_TRUE(irAssignStmt->getNodeType() == IRNodeType::ASSIGN);
    ASSERT_TRUE(context.temporaries == expectedTemporariesCount);

    context.reset();
}

TEST(IRTest, SwitchCaseDeadCodeElimination){
    std::vector<std::string> input{"switch(x){ case 0: return 1; abc = 123; abc = abc + 1; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTSwitchStmt> astSwitchStmt = parser.parseSwitchStmt();

    StatementIntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRSwitchStmt> irSwitchStmt = intermediateRepresentation.transformSwitchStmt(astSwitchStmt.get());
    const size_t expectedStmtCount = 1;

    ASSERT_TRUE(irSwitchStmt->getNodeType() == IRNodeType::SWITCH);
    auto caseStmt = irSwitchStmt->getCaseStmtAtN(0);
    ASSERT_EQ(caseStmt->getSwitchBlockStmt()->getStmts().size(), expectedStmtCount);
}

TEST(IRTest, ExprConstantFolding){
    std::vector<std::string> input{"5 + 3 - 1 * 2"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ExpressionParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTExpr> astExpr = parser.parseExpr();

    ExpressionIntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRExpr> irExpr = intermediateRepresentation.transformExpr(astExpr.get());

    ASSERT_TRUE(irExpr->getNodeType() == IRNodeType::LITERAL);
    ASSERT_EQ(static_cast<IRLiteralExpr*>(irExpr.get())->getValue(), "6");
}

TEST(IRTest, ConditionConstantFolding){
    std::vector<std::string> input{"5 > 3 && 2 < 3"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ExpressionParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTExpr> astExpr = parser.parseExpr();

    ExpressionIntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRExpr> irExpr = intermediateRepresentation.transformExpr(astExpr.get());

    ASSERT_TRUE(irExpr->getNodeType() == IRNodeType::LITERAL);
    ASSERT_EQ(static_cast<IRLiteralExpr*>(irExpr.get())->getValue(), "1");
}