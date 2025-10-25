#include "../function_parser.hpp"

FunctionParser::FunctionParser(TokenConsumer& consumer) : stmtParser{ consumer }, tokenConsumer{ consumer } {}

// FUNCTION : TYPE ID LPAREN PARAMETERS RPAREN BODY
std::unique_ptr<ASTFunction> FunctionParser::function(){
    Types type{ tokenTypeToType.find(tokenConsumer.getToken().type) != tokenTypeToType.end() ? tokenTypeToType.at(tokenConsumer.getToken().type) : Types::NO_TYPE };
    tokenConsumer.consume(GeneralTokenType::TYPE);
    const Token token{ tokenConsumer.getToken() };
    tokenConsumer.consume(TokenType::_ID);
    std::unique_ptr<ASTFunction> _function = std::make_unique<ASTFunction>(token, ASTNodeType::FUNCTION, type);

    tokenConsumer.consume(TokenType::_LPAREN);
    parameter(_function.get());
    tokenConsumer.consume(TokenType::_RPAREN);

    body(_function.get());
    
    return _function;
}

// PARAMETERS : EMPTY 
//            | PARAMETER 
//            | PARAMETERS COMMA PARAMETER
void FunctionParser::parameter(ASTFunction* _function){
    while(tokenConsumer.getToken().gtype == GeneralTokenType::TYPE){
        Types type{ tokenTypeToType.at(tokenConsumer.getToken().type) };
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

// BODY : LBRACKET STATEMENT RBRACKET
void FunctionParser::body(ASTFunction* _function){
    tokenConsumer.consume(TokenType::_LBRACKET);
    while(tokenConsumer.getToken().type != TokenType::_RBRACKET){
        _function->addStatement(stmtParser.statement());
    }
    tokenConsumer.consume(TokenType::_RBRACKET);
}
