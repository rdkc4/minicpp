#include "../function_parser.hpp"

FunctionParser::FunctionParser(TokenConsumer& consumer) : stmtParser{ consumer }, tokenConsumer{ consumer } {}

std::unique_ptr<ASTFunction> FunctionParser::parseFunction(){
    Type type{ tokenTypeToType.find(
        tokenConsumer.getToken().type) != tokenTypeToType.end() 
            ? tokenTypeToType.at(tokenConsumer.getToken().type) 
            : Type::NO_TYPE 
    };
    tokenConsumer.consume(GeneralTokenType::TYPE);

    const Token& token{ tokenConsumer.getToken() };
    tokenConsumer.consume(TokenType::_ID);
    
    std::unique_ptr<ASTFunction> function{ 
        std::make_unique<ASTFunction>(token, type) 
    };

    tokenConsumer.consume(TokenType::_LPAREN);
    parseParameters(function.get());
    tokenConsumer.consume(TokenType::_RPAREN);

    if(tokenConsumer.getToken().type == TokenType::_SEMICOLON){
        tokenConsumer.consume(TokenType::_SEMICOLON);
        function->setPredefined(true);
        return function;
    }

    parseBody(function.get());
    
    return function;
}

void FunctionParser::parseParameters(ASTFunction* function){
    if(tokenConsumer.getToken().gtype != GeneralTokenType::TYPE){
        return;
    }

    auto parseParameter = [&]() -> void {
        Type type{ tokenTypeToType.at(tokenConsumer.getToken().type) };
        tokenConsumer.consume(GeneralTokenType::TYPE);

        const auto& token{ tokenConsumer.getToken() };
        tokenConsumer.consume(TokenType::_ID);

        function->addParameter(std::make_unique<ASTParameter>(token, type));
    };

    parseParameter();
    
    while(tokenConsumer.getToken().type == TokenType::_COMMA){
        tokenConsumer.consume(TokenType::_COMMA);
        parseParameter();
    }
}

void FunctionParser::parseBody(ASTFunction* function){
    tokenConsumer.consume(TokenType::_LBRACKET);
    while(tokenConsumer.getToken().type != TokenType::_RBRACKET){
        function->addStatement(stmtParser.parseStmt());
    }
    tokenConsumer.consume(TokenType::_RBRACKET);
}
