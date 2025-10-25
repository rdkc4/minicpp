#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>

#include "../common/abstract-syntax-tree/ASTProgram.hpp"

#include "function_parser.hpp"
#include "token_consumer.hpp"

/*
    Syntax analysis of the tokenized input
    Abstract syntax tree generation
*/
class Parser {
    public:
        Parser(TokenConsumer& lexer);
        
        // parsing of the tokenized input
        // returns the root of the abstract syntax tree
        [[nodiscard]] std::unique_ptr<ASTProgram> parseProgram();

    private:
        FunctionParser funcParser;
        TokenConsumer& tokenConsumer;

};

#endif
