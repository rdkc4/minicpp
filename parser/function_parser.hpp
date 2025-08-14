#ifndef FUNCTION_PARSER_HPP
#define FUNCTION_PARSER_HPP

#include <memory>

#include "../common/abstract-syntax-tree/ASTFunction.hpp"
#include "statement_parser.hpp"
#include "token_consumer.hpp"

class FunctionParser : public TokenConsumer {
public:
    FunctionParser(Lexer& lexer);

    std::unique_ptr<ASTFunction> function();

private:
    StatementParser stmtParser;

    void parameter(ASTFunction* _function);
    void body(ASTFunction* _function);

};

#endif