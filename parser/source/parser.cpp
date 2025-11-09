#include "../parser.hpp"

#include <memory>

#include "../token_consumer.hpp"

Parser::Parser(TokenConsumer& consumer) : funcParser{ consumer }, tokenConsumer{ consumer } {}

std::unique_ptr<ASTProgram> Parser::parseProgram(){
    std::unique_ptr<ASTProgram> _program = std::make_unique<ASTProgram>(Token{"program", 0, 0}, ASTNodeType::PROGRAM);
    
    do{
        _program->addFunction(funcParser.function());
    }while(tokenConsumer.getToken().gtype == GeneralTokenType::TYPE);

    // check if input ends correctly
    tokenConsumer.consume(TokenType::_EOF);

    return _program;
}