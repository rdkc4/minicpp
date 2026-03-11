#ifndef DIRECTIVE_PARSER_HPP
#define DIRECTIVE_PARSER_HPP

#include <memory>

#include "token_consumer.hpp"
#include "../common/abstract-syntax-tree/ast_dir.hpp"
#include "../common/abstract-syntax-tree/ast_include_dir.hpp"

/**
 * @class DirectiveParser
 * @brief used for parsing directives
*/
class DirectiveParser {
public:
    /** 
     * @brief Creates the instance of the parser specialized for directives
     * @param tokenConsumer - reference to a token handler wrapped around the lexer
    */
    DirectiveParser(TokenConsumer& tokenConsumer);

    /** 
     * @brief parses directives
     * @returns pointer to the directive
    */
    std::unique_ptr<ASTDir> parseDir();

    /** 
     * @brief parses the include directive
     * @returns pointer to the include directive
    */
    std::unique_ptr<ASTIncludeDir> parseIncludeDir();

private:
    /// reference to a token handler wrapped around the lexer
    TokenConsumer& tokenConsumer;

};

#endif