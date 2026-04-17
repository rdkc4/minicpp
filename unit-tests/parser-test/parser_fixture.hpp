#ifndef PARSER_FIXTURE_HPP
#define PARSER_FIXTURE_HPP

#include <memory>

#include "../lexer-test/lexer_fixture.hpp"
#include "parser_test.hpp"

class ParserFixture : public LexerFixture {
protected:
    std::unique_ptr<syntax::ast::ASTProgram> program;

    void initParser(){
        initLexer();
        TokenConsumer tokenConsumer{ *lexer };
        ParserTest parser{ tokenConsumer };
        program = parser.parseProgram();
    }
};

class FunctionParserFixture : public LexerFixture {
protected:
    std::unique_ptr<syntax::ast::ASTFunction> function;

    void initParser(){
        initLexer();
        TokenConsumer tokenConsumer{ *lexer };
        FunctionParserTest parser{ tokenConsumer };
        function = parser.parseFunction();
    }
};

class StatementParserFixture : public LexerFixture {
protected:
    std::unique_ptr<syntax::ast::ASTStmt> stmt;
    
    void initParser() {
        initLexer();
        TokenConsumer tokenConsumer{ *lexer };
        StatementParserTest stmtParser{ tokenConsumer };
        stmt = stmtParser.parseStmt();
    }
};

class ExpressionParserFixture : public LexerFixture {
protected:
    std::unique_ptr<syntax::ast::ASTExpr> expr;
    
    void initParser() {
        initLexer();
        TokenConsumer tokenConsumer{ *lexer };
        ExpressionParserTest exprParser{ tokenConsumer };
        expr = exprParser.parseExpr();
    }
};

#endif