#include "../parser.hpp"

#include <memory>

#include "../token_consumer.hpp"

syntax::Parser::Parser(TokenConsumer& consumer) 
    : funcParser{ consumer }, 
      dirParser{ consumer }, 
      tokenConsumer{ consumer } {}

std::unique_ptr<syntax::ast::ASTProgram> syntax::Parser::parseProgram(){
    std::unique_ptr<syntax::ast::ASTProgram> program{ 
        std::make_unique<syntax::ast::ASTProgram>(syntax::Token{"program", 0, 0})
    };
    
    while(true) {
        const auto& token{ tokenConsumer.getToken() };

        if(token.gtype == syntax::GeneralTokenType::TYPE){
            program->addFunction(funcParser.parseFunction());
        }
        else if(token.type == syntax::TokenType::HASH){
            program->addDir(dirParser.parseDir());
        }
        else {
            break;
        }
    }

    // check if input ends correctly
    tokenConsumer.consume(syntax::TokenType::_EOF);

    return program;
}