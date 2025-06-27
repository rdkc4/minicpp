#include <gtest/gtest.h>

#include "analyzer_test.hpp"
#include "../lexer-test/lexer_test.hpp"
#include "../parser-test/parser_test.hpp"

TEST(AnalyzerTest, SemanticCheck){
    LexerTest lexer{"int fun(){ return 1; } int main(){ return fun();}"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};

    ASSERT_NO_THROW(analyzer.semanticCheck(ast.get()));
}

TEST(AnalyzerTest, SemanticCheckInvalidFunctionCallThrows){
    LexerTest lexer{"int fun(){ return 1; } int main(){ return fun(1);}"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    
    ASSERT_THROW(analyzer.semanticCheck(ast.get()), std::runtime_error);
    ASSERT_TRUE(analyzer.getErrors("main")[0].contains("invalid function call"));
}

TEST(AnalyzerTest, SemanticCheckTypeMismatchThrows){
    LexerTest lexer{"int fun(){ return 1u; } int main(){ return fun();}"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};

    ASSERT_THROW(analyzer.semanticCheck(ast.get()), std::runtime_error);
}

TEST(AnalyzerTest, SemanticCheckNoMainThrows){
    LexerTest lexer{"int fun(){ return 1; } int abc(){ return fun();}"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};

    ASSERT_THROW(analyzer.semanticCheck(ast.get()), std::runtime_error);
}

TEST(AnalyzerTest, CheckFunctionSignatures){
    LexerTest lexer{"int rectArea(int x, int y){ return x * y; } int sq(int x){ return x * x; }"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    analyzer.getScopeManager().pushScope();

    analyzer.testCheckFunctionSignatures(ast->getChild(0));
    ASSERT_TRUE(analyzer.getErrors("rectArea").empty());
    ASSERT_TRUE(analyzer.getErrors("sq").empty());

    analyzer.getScopeManager().popScope();
}

TEST(AnalyzerTest, CheckFunctionSignaturesFunctionRedefError){
    LexerTest lexer{"int rectArea(int x, int y){ return x * y; } int rectArea(int x){ return x * x; }"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    analyzer.getScopeManager().pushScope();

    analyzer.testCheckFunctionSignatures(ast->getChild(0));
    ASSERT_TRUE(!analyzer.getErrors(analyzer.getGlobalErrLabel()).empty());
    ASSERT_TRUE(analyzer.getErrors(analyzer.getGlobalErrLabel())[0].contains("redefined"));

    analyzer.getScopeManager().popScope();
}

TEST(AnalyzerTest, CheckFunctionSignaturesParameterRedefError){
    LexerTest lexer{"int rectArea(int x, int x){ return x * x; } int main(){ return x * x; }"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    analyzer.getScopeManager().pushScope();

    analyzer.testCheckFunctionSignatures(ast->getChild(0));
    ASSERT_TRUE(!analyzer.getErrors("rectArea").empty());
    ASSERT_TRUE(analyzer.getErrors("rectArea")[0].contains("redefined"));

    analyzer.getScopeManager().popScope();
}

TEST(AnalyzerTest, CheckFunctionSignaturesMainParamsError){
    LexerTest lexer{"int main(int x){ return 0; }"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    analyzer.getScopeManager().pushScope();

    analyzer.testCheckFunctionSignatures(ast->getChild(0));
    ASSERT_TRUE(!analyzer.getErrors("main").empty());
    ASSERT_TRUE(analyzer.getErrors("main")[0].contains("parameter"));

    analyzer.getScopeManager().popScope();
}

TEST(AnalyzerTest, CheckFunction){
    LexerTest lexer{"int fun(){ return 1; }"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testFunction();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    analyzer.getScopeManager().pushScope();
    analyzer.getScopeManager().pushSymbol(Symbol{"fun", Kinds::FUN, Types::INT});
    analyzer.initErrorsEmpty("fun");

    analyzer.testCheckFunction(ast.get());
    ASSERT_TRUE(analyzer.getErrors("fun").empty());
    analyzer.getScopeManager().popScope();
}

TEST(AnalyzerTest, CheckFunctionParameterRedefError){
    LexerTest lexer{"int fun(int x){ int x = 1; return 0; }"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testFunction();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    analyzer.getScopeManager().pushScope();
    analyzer.getScopeManager().pushSymbol(Symbol{"fun", Kinds::FUN, Types::INT});
    analyzer.initErrorsEmpty("fun");

    analyzer.testCheckFunction(ast.get());
    ASSERT_TRUE(!analyzer.getErrors("fun").empty());
    ASSERT_TRUE(analyzer.getErrors("fun")[0].contains("redefined"));
    analyzer.getScopeManager().popScope();
}

TEST(AnalyzerTest, CheckFunctionVoidReturnsTypeError){
    LexerTest lexer{"void fun(){ return 1; }"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testFunction();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    analyzer.getScopeManager().pushScope();
    analyzer.getScopeManager().pushSymbol(Symbol{"fun", Kinds::FUN, Types::VOID});
    analyzer.initErrorsEmpty("fun");

    analyzer.testCheckFunction(ast.get());
    ASSERT_TRUE(!analyzer.getErrors("fun").empty());
    ASSERT_TRUE(analyzer.getErrors("fun")[0].contains("type mismatch"));
    analyzer.getScopeManager().popScope();
}

TEST(AnalyzerTest, CheckFunctionVoidTypeMismatchError){
    LexerTest lexer{"int fun(){ return 1u; }"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testFunction();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    analyzer.getScopeManager().pushScope();
    analyzer.getScopeManager().pushSymbol(Symbol{"fun", Kinds::FUN, Types::INT});
    analyzer.initErrorsEmpty("fun");

    analyzer.testCheckFunction(ast.get());
    ASSERT_TRUE(!analyzer.getErrors("fun").empty());
    ASSERT_TRUE(analyzer.getErrors("fun")[0].contains("type mismatch"));
    analyzer.getScopeManager().popScope();
}

TEST(AnalyzerTest, CheckVariable){
    LexerTest lexer{"int x = 3;"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testVariable();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    
    analyzer.getAnalyzerContext().reset();
    analyzer.getAnalyzerContext().init("tmp", &scopeManager); // test is single-threaded, it doesn't matter that analyzer and thread share scopeManager
    analyzer.getAnalyzerContext().scopeManager->pushScope();

    analyzer.testCheckVariable(ast.get());
    ASSERT_TRUE(analyzer.getAnalyzerContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getScopeManager().lookupSymbol("x", {Kinds::VAR}));
    analyzer.getAnalyzerContext().scopeManager->popScope();
}

TEST(AnalyzerTest, CheckVariableExitedScope){
    LexerTest lexer{"{ { int x = 3; } int x = 5; }"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testCompoundStatement();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    
    analyzer.getAnalyzerContext().reset();
    analyzer.getAnalyzerContext().init("tmp", &scopeManager);

    analyzer.testCheckCompoundStatement(ast.get());
    ASSERT_TRUE(analyzer.getAnalyzerContext().semanticErrors.empty());
}

TEST(AnalyzerTest, CheckVariableRedefError){
    LexerTest lexer{"int x = 5;"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testVariable();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    
    analyzer.getAnalyzerContext().reset();
    analyzer.getAnalyzerContext().init("tmp", &scopeManager);
    analyzer.getAnalyzerContext().scopeManager->pushScope();
    analyzer.getAnalyzerContext().scopeManager->pushSymbol(Symbol{"x", Kinds::VAR, Types::INT});

    analyzer.testCheckVariable(ast.get());
    ASSERT_TRUE(!analyzer.getAnalyzerContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getAnalyzerContext().semanticErrors[0].contains("redefined"));
    analyzer.getAnalyzerContext().scopeManager->popScope();
}

TEST(AnalyzerTest, CheckVariableTypeMismatchError){
    LexerTest lexer{"int x = 5u;"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testVariable();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    
    analyzer.getAnalyzerContext().reset();
    analyzer.getAnalyzerContext().init("tmp", &scopeManager);
    analyzer.getAnalyzerContext().scopeManager->pushScope();

    analyzer.testCheckVariable(ast.get());
    ASSERT_TRUE(!analyzer.getAnalyzerContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getAnalyzerContext().semanticErrors[0].contains("type mismatch"));
    analyzer.getAnalyzerContext().scopeManager->popScope();
}

TEST(AnalyzerTest, CheckVariableAutoType){
    LexerTest lexer{"auto x = 5u;"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testVariable();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    
    analyzer.getAnalyzerContext().reset();
    analyzer.getAnalyzerContext().init("tmp", &scopeManager);
    analyzer.getAnalyzerContext().scopeManager->pushScope();

    analyzer.testCheckVariable(ast.get());
    ASSERT_TRUE(analyzer.getAnalyzerContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getAnalyzerContext().scopeManager->lookupSymbol("x", {Kinds::VAR}));
    ASSERT_EQ(analyzer.getAnalyzerContext().scopeManager->getSymbol("x").getType(), Types::UNSIGNED);
    analyzer.getAnalyzerContext().scopeManager->popScope();
}

TEST(AnalyzerTest, CheckVariableAutoError){
    LexerTest lexer{"auto x;"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testVariable();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    
    analyzer.getAnalyzerContext().reset();
    analyzer.getAnalyzerContext().init("tmp", &scopeManager);
    analyzer.getAnalyzerContext().scopeManager->pushScope();

    analyzer.testCheckVariable(ast.get());
    ASSERT_TRUE(!analyzer.getAnalyzerContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getAnalyzerContext().semanticErrors[0].contains("deduction"));
    analyzer.getAnalyzerContext().scopeManager->popScope();
}

TEST(AnalyzerTest, CheckVariableUndefinedVariableError){
    LexerTest lexer{"int x = y;"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testVariable();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};

    analyzer.getAnalyzerContext().reset();
    analyzer.getAnalyzerContext().init("tmp", &scopeManager);
    analyzer.getAnalyzerContext().scopeManager->pushScope();

    analyzer.testCheckVariable(ast.get());
    ASSERT_TRUE(!analyzer.getAnalyzerContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getAnalyzerContext().semanticErrors[0].contains("undefined"));
    analyzer.getAnalyzerContext().scopeManager->popScope();
}

TEST(AnalyzerTest, ForStatement){
    LexerTest lexer{"for(i = 0; i < 2; i = i + 1) printf(i);"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testForStatement();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};

    analyzer.getAnalyzerContext().reset();
    analyzer.getAnalyzerContext().init("tmp", &scopeManager);
    analyzer.getAnalyzerContext().scopeManager->pushScope();
    analyzer.getAnalyzerContext().scopeManager->pushSymbol(Symbol{"i", Kinds::VAR, Types::INT}); // initialize i;

    analyzer.testCheckForStatement(ast.get());
    ASSERT_TRUE(analyzer.getAnalyzerContext().semanticErrors.empty());
}

TEST(AnalyzerTest, ForStatementUndefVarError){
    LexerTest lexer{"for(i = 0; i < 10; i = i + 1) printf(i);"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testForStatement();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    
    analyzer.getAnalyzerContext().reset();
    analyzer.getAnalyzerContext().init("tmp", &scopeManager);
    analyzer.getAnalyzerContext().scopeManager->pushScope();

    analyzer.testCheckForStatement(ast.get());
    ASSERT_TRUE(!analyzer.getAnalyzerContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getAnalyzerContext().semanticErrors[0].contains("undefined"));
    analyzer.getAnalyzerContext().scopeManager->popScope();
}

TEST(AnalyzerTest, ForStatementTypeMismatchError){
    LexerTest lexer{"for(i = 0u; i < 10; i = i + 1) printf(i);"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testForStatement();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    
    analyzer.getAnalyzerContext().reset();
    analyzer.getAnalyzerContext().init("tmp", &scopeManager);
    analyzer.getAnalyzerContext().scopeManager->pushScope();
    analyzer.getAnalyzerContext().scopeManager->pushSymbol(Symbol{"i", Kinds::VAR, Types::INT}); // initialize i;

    analyzer.testCheckForStatement(ast.get());
    ASSERT_TRUE(!analyzer.getAnalyzerContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getAnalyzerContext().semanticErrors[0].contains("type mismatch"));
    analyzer.getAnalyzerContext().scopeManager->popScope();
}

TEST(AnalyzerTest, SwitchStatement){
    LexerTest lexer{"switch(x){ case 1: break; default: break; }"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testSwitchStatement();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    
    analyzer.getAnalyzerContext().reset();
    analyzer.getAnalyzerContext().init("tmp", &scopeManager);
    analyzer.getAnalyzerContext().scopeManager->pushScope();
    analyzer.getAnalyzerContext().scopeManager->pushSymbol(Symbol{"x", Kinds::VAR, Types::INT}); // initialize x;

    analyzer.testCheckSwitchStatement(ast.get());
    ASSERT_TRUE(analyzer.getAnalyzerContext().semanticErrors.empty());
    analyzer.getAnalyzerContext().scopeManager->popScope();   
}

TEST(AnalyzerTest, SwitchStatementTypeMismatchError){
    LexerTest lexer{"switch(x){ case 1u: break; default: break; }"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testSwitchStatement();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    
    analyzer.getAnalyzerContext().reset();
    analyzer.getAnalyzerContext().init("tmp", &scopeManager);
    analyzer.getAnalyzerContext().scopeManager->pushScope();
    analyzer.getAnalyzerContext().scopeManager->pushSymbol(Symbol{"x", Kinds::VAR, Types::INT}); // initialize x;

    analyzer.testCheckSwitchStatement(ast.get());
    ASSERT_TRUE(!analyzer.getAnalyzerContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getAnalyzerContext().semanticErrors[0].contains("type mismatch"));
    analyzer.getAnalyzerContext().scopeManager->popScope();   
}

TEST(AnalyzerTest, SwitchStatementCaseDuplicateError){
    LexerTest lexer{"switch(x){ case 1: break; case 1: break; }"};
    lexer.tokenize();

    ParserTest parser{ lexer };
    auto ast = parser.testSwitchStatement();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    
    analyzer.getAnalyzerContext().reset();
    analyzer.getAnalyzerContext().init("tmp", &scopeManager);
    analyzer.getAnalyzerContext().scopeManager->pushScope();
    analyzer.getAnalyzerContext().scopeManager->pushSymbol(Symbol{"x", Kinds::VAR, Types::INT}); // initialize x;

    analyzer.testCheckSwitchStatement(ast.get());
    ASSERT_TRUE(!analyzer.getAnalyzerContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getAnalyzerContext().semanticErrors[0].contains("duplicate case"));
    analyzer.getAnalyzerContext().scopeManager->popScope();   
}
