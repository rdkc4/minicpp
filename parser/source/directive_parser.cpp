#include "../directive_parser.hpp"

#include <stdexcept>
#include <format>

DirectiveParser::DirectiveParser(TokenConsumer& tokenConsumer) 
    : tokenConsumer{ tokenConsumer } {}

std::unique_ptr<syntax::ast::ASTDir> DirectiveParser::parseDir() {
    tokenConsumer.consume(syntax::TokenType::HASH);
    const auto& token{ tokenConsumer.getToken() };

    if(token.type == syntax::TokenType::INCLUDE){
        return parseIncludeDir();
    }

    throw std::runtime_error(
        std::format(
            "Line {}, Column {}: SYNTAX ERROR -> near '{}'",
            token.line, token.column, token.value
        )
    );
}

std::unique_ptr<syntax::ast::ASTIncludeDir> DirectiveParser::parseIncludeDir() {
    std::unique_ptr<syntax::ast::ASTIncludeDir> includeDir{ 
        std::make_unique<syntax::ast::ASTIncludeDir>(syntax::Token{ tokenConsumer.getToken() }) 
    };

    tokenConsumer.consume(syntax::TokenType::INCLUDE);
    tokenConsumer.consume(syntax::TokenType::COLON);

    const std::string libName{ tokenConsumer.getToken().value };
    tokenConsumer.consume(syntax::TokenType::ID);

    includeDir->setLibName(libName);

    return includeDir;
}