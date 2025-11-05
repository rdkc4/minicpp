#include <gtest/gtest.h>
#include <memory>

#include "intermediate_representation_test.hpp"
#include "../lexer-test/lexer_test.hpp"
#include "../parser-test/parser_test.hpp"

TEST(IRTest, FormIR){
    LexerTest lexer{"int fun(){ return 1; } int main(){ return fun(); }"};
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
    LexerTest lexer{"int main(){ return 1/0; }"};
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
    LexerTest lexer{"int fun(int x){ return 0; x = 1; return x; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTFunction> _astFunction = parser.testFunction();

    std::unordered_map<std::string, std::vector<std::string>> exceptions;
    FunctionIntermediateRepresentationTest intermediateRepresentation{ exceptions };
    std::unique_ptr<IRFunction> _irFunction = intermediateRepresentation.testFunction(_astFunction.get());

    const size_t expectedStmtCount = 1;

    ASSERT_TRUE(_irFunction->getNodeType() == IRNodeType::FUNCTION);
    ASSERT_EQ(_irFunction->getBody().size(), expectedStmtCount);
}

TEST(IRTest, FunctionDeadCodeEliminationIfBranching){
    LexerTest lexer{"int fun(int x){ if(x > 1) return 0; else return 1; x = x + 1; return x; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTFunction> _astFunction = parser.testFunction();

    std::unordered_map<std::string, std::vector<std::string>> exceptions;
    FunctionIntermediateRepresentationTest intermediateRepresentation{ exceptions };
    std::unique_ptr<IRFunction> _irFunction = intermediateRepresentation.testFunction(_astFunction.get());
    
    const size_t expectedStmtCount = 1;

    ASSERT_TRUE(_irFunction->getNodeType() == IRNodeType::FUNCTION);
    ASSERT_EQ(_irFunction->getBody().size(), expectedStmtCount);
}

TEST(IRTest, FunctionDeadCodeEliminationSwitchBranching){
    LexerTest lexer{"int fun(int x){ switch(x) { case 0: case 1: return 0; default: return 1; } x = x + 1; return x; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTFunction> _astFunction = parser.testFunction();

    std::unordered_map<std::string, std::vector<std::string>> exceptions;
    FunctionIntermediateRepresentationTest intermediateRepresentation{ exceptions };
    std::unique_ptr<IRFunction> _irFunction = intermediateRepresentation.testFunction(_astFunction.get());
    
    const size_t expectedStmtCount = 1; 
    
    ASSERT_TRUE(_irFunction->getNodeType() == IRNodeType::FUNCTION);
    ASSERT_TRUE(_irFunction->getBody().size() == expectedStmtCount);
}

TEST(IRTest, FunctionDeadCodeEliminationDoWhile){
    LexerTest lexer{"int fun(int x){ do{ if(x > 0) return 0; else return 1; x = x - 1; } while(x>0); return x; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTFunction> _astFunction = parser.testFunction();

    std::unordered_map<std::string, std::vector<std::string>> exceptions;
    FunctionIntermediateRepresentationTest intermediateRepresentation{ exceptions };
    std::unique_ptr<IRFunction> _irFunction = intermediateRepresentation.testFunction(_astFunction.get());

    const size_t expectedStmtCount = 1;

    ASSERT_TRUE(_irFunction->getNodeType() == IRNodeType::FUNCTION);
    ASSERT_EQ(_irFunction->getBody().size(), expectedStmtCount);
}

TEST(IRTest, CompoundStatementDeadCodeElimination){
    LexerTest lexer{"{ return 0; int x = 1; return x; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTCompoundSt> _astCompound = parser.testCompoundStatement();

    StatementIntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRCompoundSt> _irCompound = intermediateRepresentation.testCompoundStatement(_astCompound.get());

    const size_t expectedStmtCount = 1;

    ASSERT_TRUE(_irCompound->getNodeType() == IRNodeType::COMPOUND);
    ASSERT_TRUE(_irCompound->getStatements().size() == expectedStmtCount);
}

TEST(IRTest, AssignmentStatementGeneratesTemporaries){
    LexerTest lexer{"x = fun(fun(1, 2), 1);"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTAssignSt> _astAssign = parser.testAssignmentStatement();

    StatementIntermediateRepresentationTest intermediateRepresentation;

    auto& context = FunctionIntermediateRepresentationTest::getContext();
    context.init();
    const size_t expectedTemporariesCount = 2;

    std::unique_ptr<IRAssignSt> _irAssign = intermediateRepresentation.testAssignmentStatement(_astAssign.get());
    ASSERT_TRUE(_irAssign->getNodeType() == IRNodeType::ASSIGN);
    ASSERT_TRUE(context.temporaries == expectedTemporariesCount);

    context.reset();
}

TEST(IRTest, SwitchCaseDeadCodeElimination){
    LexerTest lexer{"switch(x){ case 0: return 1; abc = 123; abc = abc + 1; } "};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTSwitchSt> _astSwitch = parser.testSwitchStatement();

    StatementIntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRSwitchSt> _irSwitch = intermediateRepresentation.testSwitchStatement(_astSwitch.get());
    const size_t expectedStmtCount = 1;

    ASSERT_TRUE(_irSwitch->getNodeType() == IRNodeType::SWITCH);
    auto _case = _irSwitch->getCaseAtN(0);
    ASSERT_EQ(_case->getSwitchBlock()->getStatements().size(), expectedStmtCount);
}

TEST(IRTest, NumExpConstantFolding){
    LexerTest lexer{"5 + 3 - 1 * 2"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ExpressionParserTest parser{ tokenConsumer };
    std::unique_ptr<ASTExpression> _astExp = parser.testNumericalExpression();

    ExpressionIntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRExpression> _irExp = intermediateRepresentation.testNumericalExpression(_astExp.get());

    ASSERT_TRUE(_irExp->getNodeType() == IRNodeType::LITERAL);
    ASSERT_EQ(static_cast<IRLiteral*>(_irExp.get())->getValue(), "6");
}