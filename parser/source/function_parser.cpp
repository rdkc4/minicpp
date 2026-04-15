#include "../function_parser.hpp"

FunctionParser::FunctionParser(TokenConsumer& consumer) : stmtParser{ consumer }, tokenConsumer{ consumer } {}

std::unique_ptr<AST::node::ASTFunction> FunctionParser::parseFunction(){
    Type type{ tokenTypeToType(tokenConsumer.getToken().type) };
    tokenConsumer.consume(GeneralTokenType::TYPE);

    const Token& token{ tokenConsumer.getToken() };
    tokenConsumer.consume(TokenType::ID);
    
    std::unique_ptr<AST::node::ASTFunction> function{ 
        std::make_unique<AST::node::ASTFunction>(token, type) 
    };

    tokenConsumer.consume(TokenType::LPAREN);
    parseParameters(function.get());
    tokenConsumer.consume(TokenType::RPAREN);

    if(tokenConsumer.getToken().type == TokenType::SEMICOLON){
        tokenConsumer.consume(TokenType::SEMICOLON);
        function->setPredefined(true);
        return function;
    }

    parseBody(function.get());
    
    return function;
}

void FunctionParser::parseParameters(AST::node::ASTFunction* function){
    if(tokenConsumer.getToken().gtype != GeneralTokenType::TYPE){
        return;
    }

    auto parseParameter{ 
        [this, function]() -> void {
            Type type{ tokenTypeToType(tokenConsumer.getToken().type) };
            tokenConsumer.consume(GeneralTokenType::TYPE);

            const auto& token{ tokenConsumer.getToken() };
            tokenConsumer.consume(TokenType::ID);

            function->addParameter(std::make_unique<AST::node::ASTParameter>(token, type));
        }
    };

    parseParameter();
    
    while(tokenConsumer.getToken().type == TokenType::COMMA){
        tokenConsumer.consume(TokenType::COMMA);
        parseParameter();
    }
}

void FunctionParser::parseBody(AST::node::ASTFunction* function){
    tokenConsumer.consume(TokenType::LBRACE);
    while(tokenConsumer.getToken().type != TokenType::RBRACE){
        function->addStatement(stmtParser.parseStmt());
    }
    tokenConsumer.consume(TokenType::RBRACE);
}
