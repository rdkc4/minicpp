#include <gtest/gtest.h>

#include "analyzer_test.hpp"
#include "../lexer-test/lexer_test.hpp"
#include "../parser-test/parser_test.hpp"

TEST(AnalyzerTest, SemanticCheck){
    LexerTest lexer{"int fun(){ return 1; } int main(){ return fun();}"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    auto ast = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};

    ASSERT_NO_THROW(analyzer.semanticCheck(ast.get()));
}

TEST(AnalyzerTest, SemanticCheckInvalidFunctionCallThrows){
    LexerTest lexer{"int fun(){ return 1; } int main(){ return fun(1);}"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
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

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    auto ast = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};

    ASSERT_THROW(analyzer.semanticCheck(ast.get()), std::runtime_error);
}

TEST(AnalyzerTest, SemanticCheckNoMainThrows){
    LexerTest lexer{"int fun(){ return 1; } int abc(){ return fun();}"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    auto ast = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};

    ASSERT_THROW(analyzer.semanticCheck(ast.get()), std::runtime_error);
}

TEST(AnalyzerTest, CheckFunctionSignatures){
    LexerTest lexer{"int rectArea(int x, int y){ return x * y; } int sq(int x){ return x * x; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    auto _program = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.testCheckFunctionSignatures(_program.get());
    ASSERT_TRUE(analyzer.getErrors("rectArea").empty());
    ASSERT_TRUE(analyzer.getErrors("sq").empty());

    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunctionSignaturesFunctionRedefError){
    LexerTest lexer{"int rectArea(int x, int y){ return x * y; } int rectArea(int x){ return x * x; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    auto _program = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.testCheckFunctionSignatures(_program.get());
    ASSERT_FALSE(analyzer.getErrors(analyzer.getGlobalErrLabel()).empty());
    ASSERT_TRUE(analyzer.getErrors(analyzer.getGlobalErrLabel())[0].contains("redefined"));

    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunctionSignaturesParameterRedefError){
    LexerTest lexer{"int rectArea(int x, int x){ return x * x; } int main(){ return x * x; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    auto _program = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.testCheckFunctionSignatures(_program.get());
    ASSERT_FALSE(analyzer.getErrors("rectArea").empty());
    ASSERT_TRUE(analyzer.getErrors("rectArea")[0].contains("redefined"));

    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunctionSignaturesMainParamsError){
    LexerTest lexer{"int main(int x){ return 0; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    auto _program = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.testCheckFunctionSignatures(_program.get());
    ASSERT_FALSE(analyzer.getErrors("main").empty());
    ASSERT_TRUE(analyzer.getErrors("main")[0].contains("parameter"));

    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunction){
    LexerTest lexer{"int fun(){ return 1; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    auto _function = parser.testFunction();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    scopeManager.pushSymbol(Symbol{"fun", Kinds::FUN, Types::INT});
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {{"fun", {}}};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.testCheckFunction(_function.get());
    ASSERT_TRUE(analyzer.getErrors("fun").empty());
    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunctionNotAllIfPathsReturnError){
    LexerTest lexer{"int fun(int x){ if(x > 0) return 1; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    auto _function = parser.testFunction();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    scopeManager.pushSymbol(Symbol{"fun", Kinds::FUN, Types::INT});
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {{"fun", {}}};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.testCheckFunction(_function.get());
    ASSERT_FALSE(analyzer.getErrors("fun").empty());
    ASSERT_TRUE(analyzer.getErrors("fun")[0].contains("not all paths return"));
    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunctionNotAllSwitchPathsReturnError){
    LexerTest lexer{"int fun(int x){ switch(x){ case 0: return 0; case 1: return 1; } }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    auto _function = parser.testFunction();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    scopeManager.pushSymbol(Symbol{"fun", Kinds::FUN, Types::INT});
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {{"fun", {}}};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.testCheckFunction(_function.get());
    ASSERT_FALSE(analyzer.getErrors("fun").empty());
    ASSERT_TRUE(analyzer.getErrors("fun")[0].contains("not all paths return"));
    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunctionParameterRedefError){
    LexerTest lexer{"int fun(int x){ int x = 1; return 0; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    auto _function = parser.testFunction();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    scopeManager.pushSymbol(Symbol{"fun", Kinds::FUN, Types::INT});
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {{"fun", {}}};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.testCheckFunction(_function.get());
    ASSERT_FALSE(analyzer.getErrors("fun").empty());
    ASSERT_TRUE(analyzer.getErrors("fun")[0].contains("redefined"));
    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunctionVoidReturnsTypeError){
    LexerTest lexer{"void fun(){ return 1; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    auto _function = parser.testFunction();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    scopeManager.pushSymbol(Symbol{"fun", Kinds::FUN, Types::VOID});
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {{"fun", {}}};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.testCheckFunction(_function.get());
    ASSERT_FALSE(analyzer.getErrors("fun").empty());
    ASSERT_TRUE(analyzer.getErrors("fun")[0].contains("type mismatch"));
    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunctionVoidTypeMismatchError){
    LexerTest lexer{"int fun(){ return 1u; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    auto _function = parser.testFunction();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    scopeManager.pushSymbol(Symbol{"fun", Kinds::FUN, Types::INT});
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {{"fun", {}}};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.testCheckFunction(_function.get());
    ASSERT_FALSE(analyzer.getErrors("fun").empty());
    ASSERT_TRUE(analyzer.getErrors("fun")[0].contains("type mismatch"));
    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckVariable){
    LexerTest lexer{"int x = 3;"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _variable = parser.testVariable();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer(scopeManager);
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();

    analyzer.testCheckVariable(_variable.get());
    ASSERT_TRUE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(scopeManager.lookupSymbol("x", {Kinds::VAR}));
    
    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, CheckVariableExitedScope){
    LexerTest lexer{"{ { int x = 3; } int x = 5; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _compound = parser.testCompoundStatement();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{ scopeManager };
    
    analyzer.getContext().init("tmp", &scopeManager);

    analyzer.testCheckCompoundStatement(_compound.get());
    ASSERT_TRUE(analyzer.getContext().semanticErrors.empty());
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, CheckVariableRedefError){
    LexerTest lexer{"int x = 5;"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _variable = parser.testVariable();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();
    analyzer.getContext().scopeManager->pushSymbol(Symbol{"x", Kinds::VAR, Types::INT});

    analyzer.testCheckVariable(_variable.get());
    ASSERT_FALSE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().semanticErrors[0].contains("redefined"));
    
    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, CheckVariableTypeMismatchError){
    LexerTest lexer{"int x = 5u;"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _variable = parser.testVariable();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();

    analyzer.testCheckVariable(_variable.get());
    ASSERT_FALSE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().semanticErrors[0].contains("type mismatch"));
    
    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, CheckVariableAutoType){
    LexerTest lexer{"auto x = 5u;"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _variable = parser.testVariable();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();

    analyzer.testCheckVariable(_variable.get());
    ASSERT_TRUE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().scopeManager->lookupSymbol("x", {Kinds::VAR}));
    ASSERT_EQ(analyzer.getContext().scopeManager->getSymbol("x").getType(), Types::UNSIGNED);
    
    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, CheckVariableAutoError){
    LexerTest lexer{"auto x;"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _variable = parser.testVariable();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();

    analyzer.testCheckVariable(_variable.get());
    ASSERT_FALSE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().semanticErrors[0].contains("deduction"));
    
    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, CheckVariableUndefinedVariableError){
    LexerTest lexer{"int x = y;"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _variable = parser.testVariable();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};

    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();

    analyzer.testCheckVariable(_variable.get());
    ASSERT_FALSE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().semanticErrors[0].contains("undefined"));
    
    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, ForStatement){
    LexerTest lexer{"for(i = 0; i < 2; i = i + 1) printf(i);"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _for = parser.testForStatement();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};

    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();
    analyzer.getContext().scopeManager->pushSymbol(Symbol{"i", Kinds::VAR, Types::INT}); // initialize i;

    analyzer.testCheckForStatement(_for.get());
    ASSERT_TRUE(analyzer.getContext().semanticErrors.empty());

    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, ForStatementUndefVarError){
    LexerTest lexer{"for(i = 0; i < 10; i = i + 1) printf(i);"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _for = parser.testForStatement();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();

    analyzer.testCheckForStatement(_for.get());
    ASSERT_FALSE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().semanticErrors[0].contains("undefined"));
    
    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, ForStatementTypeMismatchError){
    LexerTest lexer{"for(i = 0u; i < 10; i = i + 1) printf(i);"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _for = parser.testForStatement();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();
    analyzer.getContext().scopeManager->pushSymbol(Symbol{"i", Kinds::VAR, Types::INT}); // initialize i;

    analyzer.testCheckForStatement(_for.get());
    ASSERT_FALSE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().semanticErrors[0].contains("type mismatch"));
    
    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, SwitchStatement){
    LexerTest lexer{"switch(x){ case 1: break; default: break; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _switch = parser.testSwitchStatement();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();
    analyzer.getContext().scopeManager->pushSymbol(Symbol{"x", Kinds::VAR, Types::INT}); // initialize x;

    analyzer.testCheckSwitchStatement(_switch.get());
    ASSERT_TRUE(analyzer.getContext().semanticErrors.empty());
    
    analyzer.getContext().scopeManager->popScope();   
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, SwitchStatementTypeMismatchError){
    LexerTest lexer{"switch(x){ case 1u: break; default: break; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _switch = parser.testSwitchStatement();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();
    analyzer.getContext().scopeManager->pushSymbol(Symbol{"x", Kinds::VAR, Types::INT}); // initialize x;

    analyzer.testCheckSwitchStatement(_switch.get());
    ASSERT_FALSE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().semanticErrors[0].contains("type mismatch"));
    
    analyzer.getContext().scopeManager->popScope();   
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, SwitchStatementCaseDuplicateError){
    LexerTest lexer{"switch(x){ case 1: break; case 1: break; }"};
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _switch = parser.testSwitchStatement();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();
    analyzer.getContext().scopeManager->pushSymbol(Symbol{"x", Kinds::VAR, Types::INT}); // initialize x;

    analyzer.testCheckSwitchStatement(_switch.get());
    ASSERT_FALSE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().semanticErrors[0].contains("duplicate case"));
    
    analyzer.getContext().scopeManager->popScope();   
    analyzer.getContext().reset();
}
