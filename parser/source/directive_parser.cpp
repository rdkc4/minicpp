#include "../directive_parser.hpp"

#include <stdexcept>
#include <format>

DirectiveParser::DirectiveParser(TokenConsumer& tokenConsumer) : tokenConsumer{ tokenConsumer } {}

std::unique_ptr<ASTDir> DirectiveParser::parseDir() {
    tokenConsumer.consume(TokenType::_HASH);
    auto token = tokenConsumer.getToken();

    if(token.type == TokenType::_INCLUDE){
        return parseIncludeDir();
    }

    throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> near '{}'",
        token.line, token.column, token.value));
}

std::unique_ptr<ASTIncludeDir> DirectiveParser::parseIncludeDir() {
    std::unique_ptr<ASTIncludeDir> _include = std::make_unique<ASTIncludeDir>(Token{ tokenConsumer.getToken() });

    tokenConsumer.consume(TokenType::_INCLUDE);
    tokenConsumer.consume(TokenType::_COLON);

    const std::string libName = tokenConsumer.getToken().value;
    tokenConsumer.consume(TokenType::_ID);

    _include->setLibName(libName);

    return _include;
}