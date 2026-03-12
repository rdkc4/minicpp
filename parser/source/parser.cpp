#include "../parser.hpp"

#include <memory>

#include "../token_consumer.hpp"

Parser::Parser(TokenConsumer& consumer) : funcParser{ consumer }, dirParser{ consumer }, tokenConsumer{ consumer } {}

std::unique_ptr<ASTProgram> Parser::parseProgram(){
    std::unique_ptr<ASTProgram> program = std::make_unique<ASTProgram>(Token{"program", 0, 0});
    
    do{
        auto token = tokenConsumer.getToken();
        if(token.gtype == GeneralTokenType::TYPE){
            program->addFunction(funcParser.parseFunction());
        }
        else if(token.type == TokenType::_HASH){
            program->addDir(dirParser.parseDir());
        }
    }while(tokenConsumer.getToken().gtype == GeneralTokenType::TYPE || tokenConsumer.getToken().type == TokenType::_HASH);

    // check if input ends correctly
    tokenConsumer.consume(TokenType::_EOF);

    return program;
}