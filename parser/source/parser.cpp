#include "../parser.hpp"

#include <memory>

#include "../token_consumer.hpp"

Parser::Parser(TokenConsumer& consumer) 
    : funcParser{ consumer }, 
      dirParser{ consumer }, 
      tokenConsumer{ consumer } {}

std::unique_ptr<AST::node::ASTProgram> Parser::parseProgram(){
    std::unique_ptr<AST::node::ASTProgram> program{ 
        std::make_unique<AST::node::ASTProgram>(Token{"program", 0, 0})
    };
    
    while(true) {
        const auto& token{ tokenConsumer.getToken() };

        if(token.gtype == GeneralTokenType::TYPE){
            program->addFunction(funcParser.parseFunction());
        }
        else if(token.type == TokenType::HASH){
            program->addDir(dirParser.parseDir());
        }
        else {
            break;
        }
    }

    // check if input ends correctly
    tokenConsumer.consume(TokenType::_EOF);

    return program;
}