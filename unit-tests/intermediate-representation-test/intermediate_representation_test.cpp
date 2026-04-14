#include <gtest/gtest.h>
#include <memory>

#include "intermediate_representation_fixture.hpp"

TEST_F(IntermediateRepresentationFixture, FormIR){
    input = {"int fun(){ return 1; } int main(){ return fun(); }"};
    initIR();

    constexpr size_t functionCount{2};

    ASSERT_TRUE(irProgram->getNodeType() == IR::defs::IRNodeType::PROGRAM);
    EXPECT_TRUE(irProgram->getFunctionCount() == functionCount);
}

TEST_F(IntermediateRepresentationFixture, FormIRThrows){
    input = {"int main(){ return 1/0; }"};
    initIR();

    ASSERT_TRUE(intermediateRepresentation->hasErrors(irProgram.get()));
    EXPECT_TRUE(intermediateRepresentation->getErrors("main")[0].contains("division by ZERO"));
}

TEST_F(IntermediateRepresentationFixture, FunctionDeadCodeElimination){
    input = {"int main(){ return 0; int x = 1; return x; }"};
    initIR();

    constexpr size_t expectedStmtCount{1};

    ASSERT_EQ(irProgram->getFunctionCount(), 1);
    EXPECT_EQ(irProgram->getFunctionAtN(0)->getBody().size(), expectedStmtCount);
}

TEST_F(IntermediateRepresentationFixture, FunctionDeadCodeEliminationIfBranching){
    input = {"int main(){ int x; if(x) return 0; else return 1; x = 2; return x; }"};
    initIR();
    
    constexpr size_t expectedStmtCount{2};

    ASSERT_EQ(irProgram->getFunctionCount(), 1);
    EXPECT_EQ(irProgram->getFunctionAtN(0)->getBody().size(), expectedStmtCount);
}

TEST_F(IntermediateRepresentationFixture, FunctionDeadCodeEliminationSwitchBranching){
    input = {"int main(){ int x; switch(x) { case 0: case 1: return 0; default: return 1; } x = 5; return x; }"};
    initIR();
    
    constexpr size_t expectedStmtCount{2}; 
    
    ASSERT_EQ(irProgram->getFunctionCount(), 1);
    EXPECT_TRUE(irProgram->getFunctionAtN(0)->getBody().size() == expectedStmtCount);
}

TEST_F(IntermediateRepresentationFixture, FunctionDeadCodeEliminationDoWhile){
    input = {"int main(){ int x; do{ if(x) return 0; else return 1; x = 5; } while(x); return x; }"};
    initIR();

    constexpr size_t expectedStmtCount{2};

    ASSERT_EQ(irProgram->getFunctionCount(), 1);
    EXPECT_EQ(irProgram->getFunctionAtN(0)->getBody().size(), expectedStmtCount);
}

TEST_F(StatementIntermediateRepresentationFixture, CompoundStatementDeadCodeElimination){
    input = {"{ return 0; if(1 > 2) return 1; }"};
    scopeManager.pushSymbol(Symbol{"tmp", Kind::FUN, Type::INT});
    initIR();

    constexpr size_t expectedStmtCount{1};

    ASSERT_TRUE(irStmt->getNodeType() == IR::defs::IRNodeType::COMPOUND);
    EXPECT_EQ(static_cast<IR::node::IRCompoundStmt*>(irStmt.get())->getStmts().size(), expectedStmtCount);
}

TEST_F(StatementIntermediateRepresentationFixture, AssignmentStatementGeneratesTemporaries){
    input = {"x = fun(fun(1, 2), 1);"};

    auto& context = FunctionIntermediateRepresentationTest::getContext();
    context.init();
    initIR();

    constexpr size_t expectedTemporariesCount{2};

    ASSERT_TRUE(irStmt->getNodeType() == IR::defs::IRNodeType::ASSIGN);
    EXPECT_TRUE(context.temporaries == expectedTemporariesCount);

    context.reset();
}

TEST_F(StatementIntermediateRepresentationFixture, SwitchCaseDeadCodeElimination){
    input = {"switch(x){ case 0: return 1; abc = 123; abc = 222; }"};
    initIR();

    constexpr size_t expectedStmtCount{1};

    ASSERT_TRUE(irStmt->getNodeType() == IR::defs::IRNodeType::SWITCH);

    auto caseStmt{ static_cast<IR::node::IRSwitchStmt*>(irStmt.get())->getCaseStmtAtN(0) };
    EXPECT_EQ(caseStmt->getSwitchBlockStmt()->getStmts().size(), expectedStmtCount);
}

TEST_F(ExpressionIntermediateRepresentationFixture, ExprConstantFolding){
    input = {"5 + 3 - 1 * 2"};
    initIR();

    ASSERT_TRUE(irExpr->getNodeType() == IR::defs::IRNodeType::LITERAL);
    EXPECT_EQ(static_cast<IR::node::IRLiteralExpr*>(irExpr.get())->getValue(), "6");
}

TEST_F(ExpressionIntermediateRepresentationFixture, ConditionConstantFolding){
    input = {"5 > 3 && 2 < 3"};
    initIR();

    ASSERT_TRUE(irExpr->getNodeType() == IR::defs::IRNodeType::LITERAL);
    EXPECT_EQ(static_cast<IR::node::IRLiteralExpr*>(irExpr.get())->getValue(), "1");
}