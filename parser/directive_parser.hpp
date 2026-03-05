#ifndef DIRECTIVE_PARSER_HPP
#define DIRECTIVE_PARSER_HPP

#include <memory>

#include "token_consumer.hpp"
#include "../common/abstract-syntax-tree/ASTDirective.hpp"
#include "../common/abstract-syntax-tree/ASTInclude.hpp"

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
    std::unique_ptr<ASTDirective> directive();

private:
    /// reference to a token handler wrapped around the lexer
    TokenConsumer& tokenConsumer;

    /** 
     * @brief parses the include directive
     * @returns pointer to the include directive
    */
    std::unique_ptr<ASTInclude> include();
};

#endif