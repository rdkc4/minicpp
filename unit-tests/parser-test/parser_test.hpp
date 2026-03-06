#ifndef PARSER_TEST_HPP
#define PARSER_TEST_HPP

#include "../../parser/parser.hpp"

class ParserTest : public Parser {
    public:
        ParserTest(TokenConsumer& consumer) : Parser{ consumer } {}
};

class FunctionParserTest : public FunctionParser {
    public:
        FunctionParserTest(TokenConsumer& consumer) : FunctionParser{ consumer } {}
};

class StatementParserTest : public StatementParser {
public:
    StatementParserTest(TokenConsumer& consumer) : StatementParser{ consumer } {}
};

class ExpressionParserTest : public ExpressionParser {
    public:
        ExpressionParserTest(TokenConsumer& consumer) : ExpressionParser{ consumer } {}
};

#endif