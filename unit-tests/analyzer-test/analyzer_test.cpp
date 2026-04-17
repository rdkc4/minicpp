#include <gtest/gtest.h>

#include "analyzer_fixture.hpp"

TEST_F(AnalyzerFixture, SemanticCheck){
    input = {"int fun(){ return 1; } int main(){ return fun();}"};
    initAnalyzer();

    ASSERT_FALSE(analyzer->hasSemanticErrors(program.get()));
}

TEST_F(AnalyzerFixture, SemanticCheckInvalidFunctionCallThrows){
    input = {"int fun(){ return 1; } int main(){ return fun(1);}"};
    initAnalyzer();

    EXPECT_TRUE(analyzer->hasSemanticErrors(program.get()));
    EXPECT_TRUE(analyzer->getErrors("main")[0].contains("invalid function call"));
}

TEST_F(AnalyzerFixture, SemanticCheckTypeMismatchThrows){
    input = {"int fun(){ return 1u; } int main(){ return fun();}"};
    initAnalyzer();

    EXPECT_TRUE(analyzer->hasSemanticErrors(program.get()));
}

TEST_F(AnalyzerFixture, SemanticCheckNoMainThrows){
    input = {"int fun(){ return 1; } int abc(){ return fun();}"};
    initAnalyzer();

    EXPECT_TRUE(analyzer->hasSemanticErrors(program.get()));
}

TEST_F(AnalyzerFixture, CheckFunctionSignatures){
    input = {"int rectArea(int x, int y){ return x * y; } int sq(int x){ return x * x; }"};
    initAnalyzer();

    EXPECT_TRUE(analyzer->getErrors("rectArea").empty());
    EXPECT_TRUE(analyzer->getErrors("sq").empty());
}

TEST_F(AnalyzerFixture, CheckFunctionSignaturesFunctionRedefError){
    input = {"int rectArea(int x, int y){ return x * y; } int rectArea(int x){ return x * x; }"};
    initAnalyzer();

    EXPECT_FALSE(analyzer->getErrors(analyzer->getGlobalErrLabel()).empty());
    EXPECT_TRUE(analyzer->getErrors(analyzer->getGlobalErrLabel())[0].contains("redefined"));
}

TEST_F(AnalyzerFixture, CheckFunctionSignaturesParameterRedefError){
    input = {"int rectArea(int x, int x){ return x * x; } int main(){ return x * x; }"};
    initAnalyzer();

    EXPECT_FALSE(analyzer->getErrors("rectArea").empty());
    EXPECT_TRUE(analyzer->getErrors("rectArea")[0].contains("redefined"));
}

TEST_F(AnalyzerFixture, CheckFunctionSignaturesMainParamsError){
    input = {"int main(int x){ return 0; }"};
    initAnalyzer();

    EXPECT_FALSE(analyzer->getErrors("main").empty());
    EXPECT_TRUE(analyzer->getErrors("main")[0].contains("parameter"));
}

TEST_F(FunctionAnalyzerFixture, CheckFunction){
    input = {"int fun(){ return 1; }"};
    scopeManager.pushSymbol(semantics::Symbol{"fun", Kind::FUN, Type::INT});
    initAnalyzer();

    EXPECT_TRUE(analyzer->getErrors("fun").empty());
}

TEST_F(FunctionAnalyzerFixture, CheckFunctionNotAllIfPathsReturnError){
    input = {"int fun(int x){ if(x > 0) return 1; }"};
    scopeManager.pushSymbol(semantics::Symbol{"fun", Kind::FUN, Type::INT});
    initAnalyzer();

    EXPECT_FALSE(analyzer->getErrors("fun").empty());
    EXPECT_TRUE(analyzer->getErrors("fun")[0].contains("not all paths return"));
}

TEST_F(FunctionAnalyzerFixture, CheckFunctionNotAllSwitchPathsReturnError){
    input = {"int fun(int x){ switch(x){ case 0: return 0; case 1: return 1; } }"};
    scopeManager.pushSymbol(semantics::Symbol{"fun", Kind::FUN, Type::INT});
    initAnalyzer();

    EXPECT_FALSE(analyzer->getErrors("fun").empty());
    EXPECT_TRUE(analyzer->getErrors("fun")[0].contains("not all paths return"));
}

TEST_F(FunctionAnalyzerFixture, CheckFunctionParameterRedefError){
    input = {"int fun(int x){ int x = 1; return 0; }"};
    scopeManager.pushSymbol(semantics::Symbol{"fun", Kind::FUN, Type::INT});
    initAnalyzer();

    EXPECT_FALSE(analyzer->getErrors("fun").empty());
    EXPECT_TRUE(analyzer->getErrors("fun")[0].contains("redefined"));
}

TEST_F(FunctionAnalyzerFixture, CheckFunctionVoidReturnsTypeError){
    input = {"void fun(){ return 1; }"};
    scopeManager.pushSymbol(semantics::Symbol{"fun", Kind::FUN, Type::VOID});
    initAnalyzer();

    EXPECT_FALSE(analyzer->getErrors("fun").empty());
    EXPECT_TRUE(analyzer->getErrors("fun")[0].contains("type mismatch"));
}

TEST_F(FunctionAnalyzerFixture, CheckFunctionVoidTypeMismatchError){
    input = {"int fun(){ return 1u; }"};
    scopeManager.pushSymbol(semantics::Symbol{"fun", Kind::FUN, Type::INT});
    initAnalyzer();

    EXPECT_FALSE(analyzer->getErrors("fun").empty());
    EXPECT_TRUE(analyzer->getErrors("fun")[0].contains("type mismatch"));
}

TEST_F(StatementAnalyzerFixture, CheckVariable){
    input = {"int x = 3;"};
    initAnalyzer();
    
    ASSERT_TRUE(analyzer->getContext().semanticErrors.empty());
    EXPECT_TRUE(scopeManager.lookupSymbol("x", {Kind::VAR}));
}

TEST_F(StatementAnalyzerFixture, CheckVariableExitedScope){
    input = {"{ { int x = 3; } int x = 5; }"};
    initAnalyzer();

    ASSERT_TRUE(analyzer->getContext().semanticErrors.empty());
}

TEST_F(StatementAnalyzerFixture, CheckVariableRedefError){
    input = {"int x = 5;"};
    scopeManager.pushSymbol(semantics::Symbol{"x", Kind::VAR, Type::INT});
    initAnalyzer();

    ASSERT_FALSE(analyzer->getContext().semanticErrors.empty());
    EXPECT_TRUE(analyzer->getContext().semanticErrors[0].contains("redefined"));
}

TEST_F(StatementAnalyzerFixture, CheckVariableTypeMismatchError){
    input = {"int x = 5u;"};
    initAnalyzer();
    
    ASSERT_FALSE(analyzer->getContext().semanticErrors.empty());
    EXPECT_TRUE(analyzer->getContext().semanticErrors[0].contains("type mismatch"));
}

TEST_F(StatementAnalyzerFixture, CheckVariableAutoType){
    input = {"auto x = 5u;"};
    initAnalyzer();

    ASSERT_TRUE(analyzer->getContext().semanticErrors.empty());
    EXPECT_TRUE(analyzer->getContext().scopeManager->lookupSymbol("x", {Kind::VAR}));
    EXPECT_EQ(analyzer->getContext().scopeManager->getSymbol("x").getType(), Type::UNSIGNED);
}

TEST_F(StatementAnalyzerFixture, CheckVariableAutoError){
    input = {"auto x;"};
    initAnalyzer();

    ASSERT_FALSE(analyzer->getContext().semanticErrors.empty());
    EXPECT_TRUE(analyzer->getContext().semanticErrors[0].contains("deduction"));
}

TEST_F(StatementAnalyzerFixture, CheckVariableUndefinedVariableError){
    input = {"int x = y;"};
    initAnalyzer();
    
    ASSERT_FALSE(analyzer->getContext().semanticErrors.empty());
    EXPECT_TRUE(analyzer->getContext().semanticErrors[0].contains("undefined"));
}

TEST_F(StatementAnalyzerFixture, ForStatement){
    input = {"for(i = 0; i < 2; i = i + 1) i = i + 1;"};
    scopeManager.pushSymbol(semantics::Symbol{"i", Kind::VAR, Type::INT});
    initAnalyzer();

    EXPECT_TRUE(analyzer->getContext().semanticErrors.empty());
}

TEST_F(StatementAnalyzerFixture, ForStatementUndefVarError){
    input = {"for(i = 0; i < 10; i = i + 1) i = i + 1;"};
    initAnalyzer();

    ASSERT_FALSE(analyzer->getContext().semanticErrors.empty());
    EXPECT_TRUE(analyzer->getContext().semanticErrors[0].contains("undefined"));
}

TEST_F(StatementAnalyzerFixture, ForStatementTypeMismatchError){
    input = {"for(i = 0u; i < 10; i = i + 1) i = i + 1;"};
    scopeManager.pushSymbol(semantics::Symbol{"i", Kind::VAR, Type::INT});
    initAnalyzer();

    ASSERT_FALSE(analyzer->getContext().semanticErrors.empty());
    EXPECT_TRUE(analyzer->getContext().semanticErrors[0].contains("type mismatch"));
}

TEST_F(StatementAnalyzerFixture, SwitchStatement){
    input = {"switch(x){ case 1: break; default: break; }"};
    scopeManager.pushSymbol(semantics::Symbol{"x", Kind::VAR, Type::INT});
    initAnalyzer();

    EXPECT_TRUE(analyzer->getContext().semanticErrors.empty());
}

TEST_F(StatementAnalyzerFixture, SwitchStatementTypeMismatchError){
    input = {"switch(x){ case 1u: break; default: break; }"};
    scopeManager.pushSymbol(semantics::Symbol{"x", Kind::VAR, Type::INT});
    initAnalyzer();

    ASSERT_FALSE(analyzer->getContext().semanticErrors.empty());
    EXPECT_TRUE(analyzer->getContext().semanticErrors[0].contains("type mismatch"));
}

TEST_F(StatementAnalyzerFixture, SwitchStatementCaseDuplicateError){
    input = {"switch(x){ case 1: break; case 1: break; }"};
    scopeManager.pushSymbol(semantics::Symbol{"x", Kind::VAR, Type::INT});
    initAnalyzer();

    ASSERT_FALSE(analyzer->getContext().semanticErrors.empty());
    EXPECT_TRUE(analyzer->getContext().semanticErrors[0].contains("duplicate case"));
}
