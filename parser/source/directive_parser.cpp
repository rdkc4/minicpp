#include "../directive_parser.hpp"

#include <stdexcept>
#include <format>

DirectiveParser::DirectiveParser(TokenConsumer& tokenConsumer) 
    : tokenConsumer{ tokenConsumer } {}

std::unique_ptr<AST::node::ASTDir> DirectiveParser::parseDir() {
    tokenConsumer.consume(TokenType::HASH);
    const auto& token{ tokenConsumer.getToken() };

    if(token.type == TokenType::INCLUDE){
        return parseIncludeDir();
    }

    throw std::runtime_error(
        std::format(
            "Line {}, Column {}: SYNTAX ERROR -> near '{}'",
            token.line, token.column, token.value
        )
    );
}

std::unique_ptr<AST::node::ASTIncludeDir> DirectiveParser::parseIncludeDir() {
    std::unique_ptr<AST::node::ASTIncludeDir> includeDir{ 
        std::make_unique<AST::node::ASTIncludeDir>(Token{ tokenConsumer.getToken() }) 
    };

    tokenConsumer.consume(TokenType::INCLUDE);
    tokenConsumer.consume(TokenType::COLON);

    const std::string libName{ tokenConsumer.getToken().value };
    tokenConsumer.consume(TokenType::ID);

    includeDir->setLibName(libName);

    return includeDir;
}