#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>

#include "intermediate_representation_test.hpp"
#include "../lexer-test/lexer_test.hpp"
#include "../parser-test/parser_test.hpp"

TEST(IRTest, FormIR){
    LexerTest lexer{"int fun(){ return 1; } int main(){ return fun(); }"};
    lexer.tokenize();

    ParserTest parser{lexer};
    std::unique_ptr<ASTree> ast = parser.parseProgram();

    IntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRTree> irt;
    const size_t expectedChildrenSize = 2;

    ASSERT_NO_THROW(irt = intermediateRepresentation.formIR(ast.get()));
    ASSERT_TRUE(irt->getNodeType() == IRNodeType::PROGRAM);
    ASSERT_TRUE(irt->getChildren().size() == expectedChildrenSize);
}

TEST(IRTest, FormIRThrows){
    LexerTest lexer{"int main(){ return 1/0; }"};
    lexer.tokenize();

    ParserTest parser{lexer};
    std::unique_ptr<ASTree> ast = parser.parseProgram();

    IntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRTree> irt;

    ASSERT_THROW(irt = intermediateRepresentation.formIR(ast.get()), std::runtime_error);
    ASSERT_TRUE(intermediateRepresentation.getErrors("main")[0].contains("division by ZERO"));
}

TEST(IRTest, FunctionDeadCodeElimination){
    LexerTest lexer{"int fun(int x){ return 0; x = 1; return x; }"};
    lexer.tokenize();

    ParserTest parser{lexer};
    std::unique_ptr<ASTree> ast = parser.testFunction();

    IntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRTree> irt;
    const size_t expectedChildrenSize = 2; // first child is PARAMETERS, second child is return statement

    ASSERT_NO_THROW(irt = intermediateRepresentation.testFunction(ast.get()));
    ASSERT_TRUE(irt->getNodeType() == IRNodeType::FUNCTION);
    ASSERT_TRUE(irt->getChildren().size() == expectedChildrenSize);
}

TEST(IRTest, FunctionDeadCodeEliminationBranching){
    LexerTest lexer{"int fun(int x){ if(x > 1) return 0; else return 1; x = x + 1; return x; }"};
    lexer.tokenize();

    ParserTest parser{lexer};
    std::unique_ptr<ASTree> ast = parser.testFunction();

    IntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRTree> irt;
    const size_t expectedChildrenSize = 2; // first child is PARAMETERS, second child is if statement
    
    ASSERT_NO_THROW(irt = intermediateRepresentation.testFunction(ast.get()));
    std::cout << irt->getChildren().size() << "\n";
    ASSERT_TRUE(irt->getNodeType() == IRNodeType::FUNCTION);
    ASSERT_TRUE(irt->getChildren().size() == expectedChildrenSize);
}

TEST(IRTest, CompoundStatementDeadCodeElimination){
    LexerTest lexer{"{ return 0; int x = 1; return x; }"};
    lexer.tokenize();

    ParserTest parser{lexer};
    std::unique_ptr<ASTree> ast = parser.testCompoundStatement();

    IntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRTree> irt;
    const size_t expectedChildrenSize = 1;

    ASSERT_NO_THROW(irt = intermediateRepresentation.testCompoundStatement(ast.get()));
    ASSERT_TRUE(irt->getNodeType() == IRNodeType::COMPOUND);
    ASSERT_TRUE(irt->getChildren().size() == expectedChildrenSize);
}

TEST(IRTest, AssignmentStatementGeneratesTemporaries){
    LexerTest lexer{"x = fun(fun(1, 2), 1)"};
    lexer.tokenize();

    ParserTest parser{lexer};
    std::unique_ptr<ASTree> ast = parser.testAssignmentStatement();

    IntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRTree> irt;

    intermediateRepresentation.getIRContext().init();
    const size_t expectedTemporariesCount = 2;

    ASSERT_NO_THROW(irt = intermediateRepresentation.testAssignmentStatement(ast.get()));
    ASSERT_TRUE(irt->getNodeType() == IRNodeType::ASSIGN);
    ASSERT_TRUE(intermediateRepresentation.getIRContext().temporaries == expectedTemporariesCount);

    intermediateRepresentation.getIRContext().reset();
}

TEST(IRTest, SwitchCaseDeadCodeElimination){
    LexerTest lexer{"switch(x){ case 0: return 1; abc = 123; abc = abc + 1; } "};
    lexer.tokenize();

    ParserTest parser{lexer};
    std::unique_ptr<ASTree> ast = parser.testSwitchStatement();

    IntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRTree> irt;
    const size_t expectedChildrenSize = 2; // first child is literal, second child is return statement

    ASSERT_NO_THROW(irt = intermediateRepresentation.testSwitchStatement(ast.get()));
    ASSERT_TRUE(irt->getNodeType() == IRNodeType::SWITCH);
    auto _case = irt->getChild(1);
    ASSERT_TRUE(_case->getChildren().size() == expectedChildrenSize);
}

TEST(IRTest, NumExpConstantFolding){
    LexerTest lexer{"5 + 3 - 1 * 2"};
    lexer.tokenize();

    ParserTest parser{lexer};
    std::unique_ptr<ASTree> ast = parser.testNumericalExpression();

    IntermediateRepresentationTest intermediateRepresentation;
    std::unique_ptr<IRTree> irt;

    ASSERT_NO_THROW(irt = intermediateRepresentation.testNumericalExpression(ast.get()));
    ASSERT_TRUE(irt->getNodeType() == IRNodeType::LITERAL);
    ASSERT_TRUE(irt->getValue() == "6");
}