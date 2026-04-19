#ifndef PARSER_TEST_HPP
#define PARSER_TEST_HPP

#include "../../parser/parser.hpp"

class ParserTest : public syntax::Parser {
    public:
        ParserTest(syntax::TokenConsumer& consumer) 
            : syntax::Parser{ consumer } {}
};

class FunctionParserTest : public syntax::FunctionParser {
    public:
        FunctionParserTest(syntax::TokenConsumer& consumer) 
            : syntax::FunctionParser{ consumer } {}
};

class StatementParserTest : public syntax::StatementParser {
public:
    StatementParserTest(syntax::TokenConsumer& consumer) 
        : syntax::StatementParser{ consumer } {}
};

class ExpressionParserTest : public syntax::ExpressionParser {
    public:
        ExpressionParserTest(syntax::TokenConsumer& consumer) 
            : syntax::ExpressionParser{ consumer } {}
};

#endif