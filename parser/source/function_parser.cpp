#include "../function_parser.hpp"

FunctionParser::FunctionParser(TokenConsumer& consumer) : stmtParser{ consumer }, tokenConsumer{ consumer } {}

std::unique_ptr<ASTFunction> FunctionParser::function(){
    Type type{ tokenTypeToType.find(tokenConsumer.getToken().type) != tokenTypeToType.end() ? tokenTypeToType.at(tokenConsumer.getToken().type) : Type::NO_TYPE };
    tokenConsumer.consume(GeneralTokenType::TYPE);
    const Token token{ tokenConsumer.getToken() };
    tokenConsumer.consume(TokenType::_ID);
    std::unique_ptr<ASTFunction> _function = std::make_unique<ASTFunction>(token, ASTNodeType::FUNCTION, type);

    tokenConsumer.consume(TokenType::_LPAREN);
    parameter(_function.get());
    tokenConsumer.consume(TokenType::_RPAREN);

    if(tokenConsumer.getToken().type == TokenType::_SEMICOLON){
        tokenConsumer.consume(TokenType::_SEMICOLON);
        _function->setPredefined(true);
        return _function;
    }

    body(_function.get());
    
    return _function;
}

void FunctionParser::parameter(ASTFunction* _function){
    while(tokenConsumer.getToken().gtype == GeneralTokenType::TYPE){
        Type type{ tokenTypeToType.at(tokenConsumer.getToken().type) };
        tokenConsumer.consume(GeneralTokenType::TYPE);
        const Token token{ tokenConsumer.getToken() };
        tokenConsumer.consume(TokenType::_ID);
        
        _function->addParameter(std::make_unique<ASTParameter>(token, ASTNodeType::PARAMETER, type));

        if(tokenConsumer.getToken().type == TokenType::_COMMA && tokenConsumer.peek().gtype == GeneralTokenType::TYPE){
            tokenConsumer.consume(TokenType::_COMMA);
        }
        else{
            break;
        }
    }
}

void FunctionParser::body(ASTFunction* _function){
    tokenConsumer.consume(TokenType::_LBRACKET);
    while(tokenConsumer.getToken().type != TokenType::_RBRACKET){
        _function->addStatement(stmtParser.statement());
    }
    tokenConsumer.consume(TokenType::_RBRACKET);
}
