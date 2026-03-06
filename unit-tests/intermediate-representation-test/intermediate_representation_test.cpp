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
    std::unique_ptr<ASTProgram> _astProgram = parser.parseProgram();

    IntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRProgram> _irProgram = intermediateRepresentation.formIR(_astProgram.get());
    const size_t functionCount = 2;

    ASSERT_TRUE(_irProgram->getNodeType() == IRNodeType::PROGRAM);
    ASSERT_TRUE(_irProgram->getFunctionCount() == functionCount);
}

TEST(IRTest, FormIRThrows){
    std::vector<std::string> input{"int main(){ return 1/0; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTProgram> _astProgram = parser.parseProgram();

    IntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRProgram> _irProgram = intermediateRepresentation.formIR(_astProgram.get());

    ASSERT_TRUE(intermediateRepresentation.hasErrors(_irProgram.get()));
    ASSERT_TRUE(intermediateRepresentation.getErrors("main")[0].contains("division by ZERO"));
}

TEST(IRTest, FunctionDeadCodeElimination){
    std::vector<std::string> input{"int fun(int x){ return 0; x = 1; return x; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTFunction> _astFunction = parser.function();

    std::unordered_map<std::string, std::vector<std::string>> exceptions;
    FunctionIntermediateRepresentationTest intermediateRepresentation{ exceptions };
    std::unique_ptr<IRFunction> _irFunction = intermediateRepresentation.function(_astFunction.get());

    const size_t expectedStmtCount = 1;

    ASSERT_TRUE(_irFunction->getNodeType() == IRNodeType::FUNCTION);
    ASSERT_EQ(_irFunction->getBody().size(), expectedStmtCount);
}

TEST(IRTest, FunctionDeadCodeEliminationIfBranching){
    std::vector<std::string> input{"int fun(int x){ if(x > 1) return 0; else return 1; x = x + 1; return x; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTFunction> _astFunction = parser.function();

    std::unordered_map<std::string, std::vector<std::string>> exceptions;
    FunctionIntermediateRepresentationTest intermediateRepresentation{ exceptions };
    std::unique_ptr<IRFunction> _irFunction = intermediateRepresentation.function(_astFunction.get());
    
    const size_t expectedStmtCount = 1;

    ASSERT_TRUE(_irFunction->getNodeType() == IRNodeType::FUNCTION);
    ASSERT_EQ(_irFunction->getBody().size(), expectedStmtCount);
}

TEST(IRTest, FunctionDeadCodeEliminationSwitchBranching){
    std::vector<std::string> input{"int fun(int x){ switch(x) { case 0: case 1: return 0; default: return 1; } x = x + 1; return x; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTFunction> _astFunction = parser.function();

    std::unordered_map<std::string, std::vector<std::string>> exceptions;
    FunctionIntermediateRepresentationTest intermediateRepresentation{ exceptions };
    std::unique_ptr<IRFunction> _irFunction = intermediateRepresentation.function(_astFunction.get());
    
    const size_t expectedStmtCount = 1; 
    
    ASSERT_TRUE(_irFunction->getNodeType() == IRNodeType::FUNCTION);
    ASSERT_TRUE(_irFunction->getBody().size() == expectedStmtCount);
}

TEST(IRTest, FunctionDeadCodeEliminationDoWhile){
    std::vector<std::string> input{"int fun(int x){ do{ if(x > 0) return 0; else return 1; x = x - 1; } while(x>0); return x; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTFunction> _astFunction = parser.function();

    std::unordered_map<std::string, std::vector<std::string>> exceptions;
    FunctionIntermediateRepresentationTest intermediateRepresentation{ exceptions };
    std::unique_ptr<IRFunction> _irFunction = intermediateRepresentation.function(_astFunction.get());

    const size_t expectedStmtCount = 1;

    ASSERT_TRUE(_irFunction->getNodeType() == IRNodeType::FUNCTION);
    ASSERT_EQ(_irFunction->getBody().size(), expectedStmtCount);
}

TEST(IRTest, CompoundStatementDeadCodeElimination){
    std::vector<std::string> input{"{ return 0; int x = 1; return x; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTCompoundSt> _astCompound = parser.compoundStatement();

    StatementIntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRCompoundSt> _irCompound = intermediateRepresentation.compoundStatement(_astCompound.get());

    const size_t expectedStmtCount = 1;

    ASSERT_TRUE(_irCompound->getNodeType() == IRNodeType::COMPOUND);
    ASSERT_TRUE(_irCompound->getStatements().size() == expectedStmtCount);
}

TEST(IRTest, AssignmentStatementGeneratesTemporaries){
    std::vector<std::string> input{"x = fun(fun(1, 2), 1);"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTAssignSt> _astAssign = parser.assignmentStatement();

    StatementIntermediateRepresentationTest intermediateRepresentation;

    auto& context = FunctionIntermediateRepresentationTest::getContext();
    context.init();
    const size_t expectedTemporariesCount = 2;

    std::unique_ptr<IRAssignSt> _irAssign = intermediateRepresentation.assignmentStatement(_astAssign.get());
    ASSERT_TRUE(_irAssign->getNodeType() == IRNodeType::ASSIGN);
    ASSERT_TRUE(context.temporaries == expectedTemporariesCount);

    context.reset();
}

TEST(IRTest, SwitchCaseDeadCodeElimination){
    std::vector<std::string> input{"switch(x){ case 0: return 1; abc = 123; abc = abc + 1; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTSwitchSt> _astSwitch = parser.switchStatement();

    StatementIntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRSwitchSt> _irSwitch = intermediateRepresentation.switchStatement(_astSwitch.get());
    const size_t expectedStmtCount = 1;

    ASSERT_TRUE(_irSwitch->getNodeType() == IRNodeType::SWITCH);
    auto _case = _irSwitch->getCaseAtN(0);
    ASSERT_EQ(_case->getSwitchBlock()->getStatements().size(), expectedStmtCount);
}

TEST(IRTest, NumExpConstantFolding){
    std::vector<std::string> input{"5 + 3 - 1 * 2"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ExpressionParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTExpression> _astExp = parser.numericalExpression();

    ExpressionIntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRExpression> _irExp = intermediateRepresentation.numericalExpression(_astExp.get());

    ASSERT_TRUE(_irExp->getNodeType() == IRNodeType::LITERAL);
    ASSERT_EQ(static_cast<IRLiteral*>(_irExp.get())->getValue(), "6");
}