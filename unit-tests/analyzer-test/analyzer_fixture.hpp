#ifndef ANALYZER_FIXTURE_HPP
#define ANALYZER_FIXTURE_HPP

#include "../parser-test/parser_fixture.hpp"
#include "analyzer_test.hpp"

class AnalyzerFixture : public ParserFixture {
protected:
    ThreadPool tp{1};
    semantic::SymbolTable symtab;
    semantic::ScopeManager scopeManager{symtab};
    std::unique_ptr<AnalyzerTest> analyzer;

    void initAnalyzer() {
        initParser();
        analyzer = std::make_unique<AnalyzerTest>(scopeManager, tp);
        program->accept(*analyzer);
    }
};

class FunctionAnalyzerFixture : public FunctionParserFixture {
protected:
    ThreadPool tp{1};
    semantic::SymbolTable symtab;
    semantic::ScopeManager scopeManager{symtab};
    std::unique_ptr<AnalyzerTest> analyzer;

    void initAnalyzer() {
        initParser();
        analyzer = std::make_unique<AnalyzerTest>(scopeManager, tp);
        function->accept(*analyzer);
    }

    void SetUp() override {
        scopeManager.pushScope();
    }

    void TearDown() override {
        scopeManager.popScope();
    }

};

class StatementAnalyzerFixture : public StatementParserFixture {
protected:
    ThreadPool tp{1};
    semantic::SymbolTable symtab;
    semantic::ScopeManager scopeManager{symtab};
    std::unique_ptr<AnalyzerTest> analyzer;

    void initAnalyzer(){
        initParser();
        analyzer = std::make_unique<AnalyzerTest>(scopeManager, tp);
        analyzer->getContext().init("tmp", &scopeManager);
        stmt->accept(*analyzer);
    }

    void SetUp() override {
        scopeManager.pushScope();
    }

    void TearDown() override {
        if(analyzer){
            analyzer->getContext().reset();
        }
        scopeManager.popScope();
    }
};

class ExpressionAnalyzerFixture : public ExpressionParserFixture {
protected:
    ThreadPool tp{1};
    semantic::SymbolTable symtab;
    semantic::ScopeManager scopeManager{symtab};
    std::unique_ptr<AnalyzerTest> analyzer;

    void initAnalyzer(){
        initParser();
        analyzer = std::make_unique<AnalyzerTest>(scopeManager, tp);
        analyzer->getContext().init("tmp", &scopeManager);
        expr->accept(*analyzer);
    }

    void SetUp() override {
        scopeManager.pushScope();
    }

    void TearDown() override {
        if(analyzer){
            analyzer->getContext().reset();
        }
        scopeManager.popScope();
    }
};

#endif