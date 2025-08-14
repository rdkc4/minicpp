#include "../function_parser.hpp"

FunctionParser::FunctionParser(Lexer& lexer) : TokenConsumer{ lexer }, stmtParser{ lexer } {}

// FUNCTION : TYPE ID LPAREN PARAMETERS RPAREN BODY
std::unique_ptr<ASTFunction> FunctionParser::function(){
    Types type{ tokenTypeToType.find(getToken().type) != tokenTypeToType.end() ? tokenTypeToType.at(getToken().type) : Types::NO_TYPE };
    consume(GeneralTokenType::TYPE);
    const Token token{ getToken() };
    consume(TokenType::_ID);
    std::unique_ptr<ASTFunction> _function = std::make_unique<ASTFunction>(token, ASTNodeType::FUNCTION, type);

    consume(TokenType::_LPAREN);
    parameter(_function.get());
    consume(TokenType::_RPAREN);

    body(_function.get());
    
    return _function;
}

// PARAMETERS : EMPTY 
//            | PARAMETER 
//            | PARAMETERS COMMA PARAMETER
void FunctionParser::parameter(ASTFunction* _function){
    while(getToken().gtype == GeneralTokenType::TYPE){
        Types type{ tokenTypeToType.at(getToken().type) };
        consume(GeneralTokenType::TYPE);
        const Token token{ getToken() };
        consume(TokenType::_ID);
        
        _function->addParameter(std::make_unique<ASTParameter>(token, ASTNodeType::PARAMETER, type));

        if(getToken().type == TokenType::_COMMA && peek().gtype == GeneralTokenType::TYPE){
            consume(TokenType::_COMMA);
        }
        else{
            break;
        }
    }
}

// BODY : LBRACKET STATEMENT RBRACKET
void FunctionParser::body(ASTFunction* _function){
    consume(TokenType::_LBRACKET);
    while(getToken().type != TokenType::_RBRACKET){
        _function->addStatement(stmtParser.statement());
    }
    consume(TokenType::_RBRACKET);
}
