#include "../directive_parser.hpp"

#include <stdexcept>
#include <format>

DirectiveParser::DirectiveParser(TokenConsumer& tokenConsumer) : tokenConsumer{ tokenConsumer } {}

std::unique_ptr<ASTDir> DirectiveParser::parseDir() {
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

std::unique_ptr<ASTIncludeDir> DirectiveParser::parseIncludeDir() {
    std::unique_ptr<ASTIncludeDir> includeDir{ 
        std::make_unique<ASTIncludeDir>(Token{ tokenConsumer.getToken() }) 
    };

    tokenConsumer.consume(TokenType::INCLUDE);
    tokenConsumer.consume(TokenType::COLON);

    const std::string libName{ tokenConsumer.getToken().value };
    tokenConsumer.consume(TokenType::ID);

    includeDir->setLibName(libName);

    return includeDir;
}