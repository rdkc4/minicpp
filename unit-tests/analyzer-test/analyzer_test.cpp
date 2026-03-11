#include <gtest/gtest.h>

#include "analyzer_test.hpp"
#include "../lexer-test/lexer_test.hpp"
#include "../parser-test/parser_test.hpp"

TEST(AnalyzerTest, SemanticCheck){
    std::vector<std::string> input{"int fun(){ return 1; } int main(){ return fun();}"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    auto ast = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};

    analyzer.semanticCheck(ast.get());
    ASSERT_FALSE(analyzer.hasSemanticError(ast.get()));
}

TEST(AnalyzerTest, SemanticCheckInvalidFunctionCallThrows){
    std::vector<std::string> input{"int fun(){ return 1; } int main(){ return fun(1);}"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    auto ast = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};
    
    analyzer.semanticCheck(ast.get());
    ASSERT_TRUE(analyzer.hasSemanticError(ast.get()));
    ASSERT_TRUE(analyzer.getErrors("main")[0].contains("invalid function call"));
}

TEST(AnalyzerTest, SemanticCheckTypeMismatchThrows){
    std::vector<std::string> input{"int fun(){ return 1u; } int main(){ return fun();}"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    auto ast = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};

    analyzer.semanticCheck(ast.get());
    ASSERT_TRUE(analyzer.hasSemanticError(ast.get()));
}

TEST(AnalyzerTest, SemanticCheckNoMainThrows){
    std::vector<std::string> input{"int fun(){ return 1; } int abc(){ return fun();}"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    auto ast = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    AnalyzerTest analyzer{scopeManager};

    analyzer.semanticCheck(ast.get());
    ASSERT_TRUE(analyzer.hasSemanticError(ast.get()));
}

TEST(AnalyzerTest, CheckFunctionSignatures){
    std::vector<std::string> input{"int rectArea(int x, int y){ return x * y; } int sq(int x){ return x * x; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    auto _program = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.checkFunctionSignatures(_program.get());
    ASSERT_TRUE(analyzer.getErrors("rectArea").empty());
    ASSERT_TRUE(analyzer.getErrors("sq").empty());

    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunctionSignaturesFunctionRedefError){
    std::vector<std::string> input{"int rectArea(int x, int y){ return x * y; } int rectArea(int x){ return x * x; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    auto _program = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.checkFunctionSignatures(_program.get());
    ASSERT_FALSE(analyzer.getErrors(analyzer.getGlobalErrLabel()).empty());
    ASSERT_TRUE(analyzer.getErrors(analyzer.getGlobalErrLabel())[0].contains("redefined"));

    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunctionSignaturesParameterRedefError){
    std::vector<std::string> input{"int rectArea(int x, int x){ return x * x; } int main(){ return x * x; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    auto _program = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.checkFunctionSignatures(_program.get());
    ASSERT_FALSE(analyzer.getErrors("rectArea").empty());
    ASSERT_TRUE(analyzer.getErrors("rectArea")[0].contains("redefined"));

    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunctionSignaturesMainParamsError){
    std::vector<std::string> input{"int main(int x){ return 0; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    ParserTest parser{ tokenConsumer };
    auto _program = parser.parseProgram();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.checkFunctionSignatures(_program.get());
    ASSERT_FALSE(analyzer.getErrors("main").empty());
    ASSERT_TRUE(analyzer.getErrors("main")[0].contains("parameter"));

    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunction){
    std::vector<std::string> input{"int fun(){ return 1; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    auto _function = parser.parseFunction();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    scopeManager.pushSymbol(Symbol{"fun", Kind::FUN, Type::INT});
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {{"fun", {}}};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.checkFunction(_function.get());
    ASSERT_TRUE(analyzer.getErrors("fun").empty());
    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunctionNotAllIfPathsReturnError){
    std::vector<std::string> input{"int fun(int x){ if(x > 0) return 1; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    auto _function = parser.parseFunction();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    scopeManager.pushSymbol(Symbol{"fun", Kind::FUN, Type::INT});
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {{"fun", {}}};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.checkFunction(_function.get());
    ASSERT_FALSE(analyzer.getErrors("fun").empty());
    ASSERT_TRUE(analyzer.getErrors("fun")[0].contains("not all paths return"));
    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunctionNotAllSwitchPathsReturnError){
    std::vector<std::string> input{"int fun(int x){ switch(x){ case 0: return 0; case 1: return 1; } }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    auto _function = parser.parseFunction();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    scopeManager.pushSymbol(Symbol{"fun", Kind::FUN, Type::INT});
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {{"fun", {}}};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.checkFunction(_function.get());
    ASSERT_FALSE(analyzer.getErrors("fun").empty());
    ASSERT_TRUE(analyzer.getErrors("fun")[0].contains("not all paths return"));
    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunctionParameterRedefError){
    std::vector<std::string> input{"int fun(int x){ int x = 1; return 0; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    auto _function = parser.parseFunction();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    scopeManager.pushSymbol(Symbol{"fun", Kind::FUN, Type::INT});
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {{"fun", {}}};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.checkFunction(_function.get());
    ASSERT_FALSE(analyzer.getErrors("fun").empty());
    ASSERT_TRUE(analyzer.getErrors("fun")[0].contains("redefined"));
    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunctionVoidReturnsTypeError){
    std::vector<std::string> input{"void fun(){ return 1; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    auto _function = parser.parseFunction();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    scopeManager.pushSymbol(Symbol{"fun", Kind::FUN, Type::VOID});
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {{"fun", {}}};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.checkFunction(_function.get());
    ASSERT_FALSE(analyzer.getErrors("fun").empty());
    ASSERT_TRUE(analyzer.getErrors("fun")[0].contains("type mismatch"));
    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckFunctionVoidTypeMismatchError){
    std::vector<std::string> input{"int fun(){ return 1u; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    FunctionParserTest parser{ tokenConsumer };
    auto _function = parser.parseFunction();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    scopeManager.pushScope();
    scopeManager.pushSymbol(Symbol{"fun", Kind::FUN, Type::INT});
    std::unordered_map<std::string, std::vector<std::string>> semErrors = {{"fun", {}}};
    FunctionAnalyzerTest analyzer(scopeManager, semErrors, "__global");

    analyzer.checkFunction(_function.get());
    ASSERT_FALSE(analyzer.getErrors("fun").empty());
    ASSERT_TRUE(analyzer.getErrors("fun")[0].contains("type mismatch"));
    scopeManager.popScope();
}

TEST(AnalyzerTest, CheckVariable){
    std::vector<std::string> input{"int x = 3;"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _variable = parser.parseVariableDeclStmt();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer(scopeManager);
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();

    analyzer.checkVariable(_variable.get());
    ASSERT_TRUE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(scopeManager.lookupSymbol("x", {Kind::VAR}));
    
    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, CheckVariableExitedScope){
    std::vector<std::string> input{"{ { int x = 3; } int x = 5; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _compound = parser.parseCompoundStmt();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{ scopeManager };
    
    analyzer.getContext().init("tmp", &scopeManager);

    analyzer.checkCompoundStatement(_compound.get());
    ASSERT_TRUE(analyzer.getContext().semanticErrors.empty());
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, CheckVariableRedefError){
    std::vector<std::string> input{"int x = 5;"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _variable = parser.parseVariableDeclStmt();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();
    analyzer.getContext().scopeManager->pushSymbol(Symbol{"x", Kind::VAR, Type::INT});

    analyzer.checkVariable(_variable.get());
    ASSERT_FALSE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().semanticErrors[0].contains("redefined"));
    
    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, CheckVariableTypeMismatchError){
    std::vector<std::string> input{"int x = 5u;"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _variable = parser.parseVariableDeclStmt();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();

    analyzer.checkVariable(_variable.get());
    ASSERT_FALSE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().semanticErrors[0].contains("type mismatch"));
    
    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, CheckVariableAutoType){
    std::vector<std::string> input{"auto x = 5u;"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _variable = parser.parseVariableDeclStmt();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();

    analyzer.checkVariable(_variable.get());
    ASSERT_TRUE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().scopeManager->lookupSymbol("x", {Kind::VAR}));
    ASSERT_EQ(analyzer.getContext().scopeManager->getSymbol("x").getType(), Type::UNSIGNED);
    
    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, CheckVariableAutoError){
    std::vector<std::string> input{"auto x;"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _variable = parser.parseVariableDeclStmt();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();

    analyzer.checkVariable(_variable.get());
    ASSERT_FALSE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().semanticErrors[0].contains("deduction"));
    
    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, CheckVariableUndefinedVariableError){
    std::vector<std::string> input{"int x = y;"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _variable = parser.parseVariableDeclStmt();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};

    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();

    analyzer.checkVariable(_variable.get());
    ASSERT_FALSE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().semanticErrors[0].contains("undefined"));
    
    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, ForStatement){
    std::vector<std::string> input{"for(i = 0; i < 2; i = i + 1) i = i + 1;"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _for = parser.parseForStmt();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};

    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();
    analyzer.getContext().scopeManager->pushSymbol(Symbol{"i", Kind::VAR, Type::INT}); // initialize i;

    analyzer.checkForStatement(_for.get());
    ASSERT_TRUE(analyzer.getContext().semanticErrors.empty());

    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, ForStatementUndefVarError){
    std::vector<std::string> input{"for(i = 0; i < 10; i = i + 1) i = i + 1;"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _for = parser.parseForStmt();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();

    analyzer.checkForStatement(_for.get());
    ASSERT_FALSE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().semanticErrors[0].contains("undefined"));
    
    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, ForStatementTypeMismatchError){
    std::vector<std::string> input{"for(i = 0u; i < 10; i = i + 1) i = i + 1;"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _for = parser.parseForStmt();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();
    analyzer.getContext().scopeManager->pushSymbol(Symbol{"i", Kind::VAR, Type::INT}); // initialize i;

    analyzer.checkForStatement(_for.get());
    ASSERT_FALSE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().semanticErrors[0].contains("type mismatch"));
    
    analyzer.getContext().scopeManager->popScope();
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, SwitchStatement){
    std::vector<std::string> input{"switch(x){ case 1: break; default: break; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _switch = parser.parseSwitchStmt();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();
    analyzer.getContext().scopeManager->pushSymbol(Symbol{"x", Kind::VAR, Type::INT}); // initialize x;

    analyzer.checkSwitchStatement(_switch.get());
    ASSERT_TRUE(analyzer.getContext().semanticErrors.empty());
    
    analyzer.getContext().scopeManager->popScope();   
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, SwitchStatementTypeMismatchError){
    std::vector<std::string> input{"switch(x){ case 1u: break; default: break; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _switch = parser.parseSwitchStmt();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();
    analyzer.getContext().scopeManager->pushSymbol(Symbol{"x", Kind::VAR, Type::INT}); // initialize x;

    analyzer.checkSwitchStatement(_switch.get());
    ASSERT_FALSE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().semanticErrors[0].contains("type mismatch"));
    
    analyzer.getContext().scopeManager->popScope();   
    analyzer.getContext().reset();
}

TEST(AnalyzerTest, SwitchStatementCaseDuplicateError){
    std::vector<std::string> input{"switch(x){ case 1: break; case 1: break; }"};
    LexerTest lexer{ input };
    lexer.tokenize();

    TokenConsumer tokenConsumer { lexer };
    StatementParserTest parser{ tokenConsumer };
    auto _switch = parser.parseSwitchStmt();

    SymbolTable symtab;
    ScopeManager scopeManager{ symtab };
    StatementAnalyzerTest analyzer{scopeManager};
    
    analyzer.getContext().init("tmp", &scopeManager);
    analyzer.getContext().scopeManager->pushScope();
    analyzer.getContext().scopeManager->pushSymbol(Symbol{"x", Kind::VAR, Type::INT}); // initialize x;

    analyzer.checkSwitchStatement(_switch.get());
    ASSERT_FALSE(analyzer.getContext().semanticErrors.empty());
    ASSERT_TRUE(analyzer.getContext().semanticErrors[0].contains("duplicate case"));
    
    analyzer.getContext().scopeManager->popScope();   
    analyzer.getContext().reset();
}
