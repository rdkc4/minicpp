#include "../function_parser.hpp"

syntax::FunctionParser::FunctionParser(TokenConsumer& consumer) 
    : stmtParser{ consumer }, 
      tokenConsumer{ consumer } {}

std::unique_ptr<syntax::ast::ASTFunction> syntax::FunctionParser::parseFunction(){
    Type type{ tokenTypeToType(tokenConsumer.getToken().type) };
    tokenConsumer.consume(syntax::GeneralTokenType::TYPE);

    const syntax::Token& token{ tokenConsumer.getToken() };
    tokenConsumer.consume(syntax::TokenType::ID);
    
    std::unique_ptr<syntax::ast::ASTFunction> function{ 
        std::make_unique<syntax::ast::ASTFunction>(token, type) 
    };

    tokenConsumer.consume(syntax::TokenType::LPAREN);
    parseParameters(function.get());
    tokenConsumer.consume(syntax::TokenType::RPAREN);

    if(tokenConsumer.getToken().type == syntax::TokenType::SEMICOLON){
        tokenConsumer.consume(syntax::TokenType::SEMICOLON);
        function->setPredefined(true);
        return function;
    }

    parseBody(function.get());
    
    return function;
}

void syntax::FunctionParser::parseParameters(syntax::ast::ASTFunction* function){
    if(tokenConsumer.getToken().gtype != syntax::GeneralTokenType::TYPE){
        return;
    }

    auto parseParameter{ 
        [this, function]() -> void {
            Type type{ tokenTypeToType(tokenConsumer.getToken().type) };
            tokenConsumer.consume(syntax::GeneralTokenType::TYPE);

            const auto& token{ tokenConsumer.getToken() };
            tokenConsumer.consume(syntax::TokenType::ID);

            function->addParameter(std::make_unique<syntax::ast::ASTParameter>(token, type));
        }
    };

    parseParameter();
    
    while(tokenConsumer.getToken().type == syntax::TokenType::COMMA){
        tokenConsumer.consume(syntax::TokenType::COMMA);
        parseParameter();
    }
}

void syntax::FunctionParser::parseBody(syntax::ast::ASTFunction* function){
    tokenConsumer.consume(syntax::TokenType::LBRACE);
    while(tokenConsumer.getToken().type != syntax::TokenType::RBRACE){
        function->addStatement(stmtParser.parseStmt());
    }
    tokenConsumer.consume(syntax::TokenType::RBRACE);
}
