#include "../include/lexer.hpp"

Lexer::Lexer(const std::string& input) : input(input), position(0), lineNumber(1){}

void Lexer::tokenize(){
    while(position < input.size()){
                char curr = input[position];

                if(std::isspace(curr)){
                    ++position;
                    if(curr == '\n'){
                        ++lineNumber;
                    }
                }
                else if(std::isalpha(curr)){
                    std::string id = getID();
                    if(isKeyword(id)){
                        tokens.push_back(Token(keywords.at(id), id));
                    }
                    else{
                        tokens.push_back(Token(TokenType::_ID, id));
                    }
                }
                else if(std::isdigit(curr)){
                    tokens.push_back(Token(TokenType::_LITERAL, getLiteral()));
                }
                else if(isAritOperator(curr)){
                    tokens.push_back(Token(TokenType::_AROP, std::string(1, curr)));
                    ++position;
                }
                else if(curr == '=' && (position+1 >= input.size() || input[position+1] != '=')){
                    tokens.push_back(Token(TokenType::_ASSIGN, std::string(1, curr)));
                    ++position;
                }
                else if(isRelOperator(curr)){
                    tokens.push_back(Token(TokenType::_RELOP, getRelOperator()));
                }
                else if(curr == '('){
                    tokens.push_back(Token(TokenType::_LPAREN, std::string(1,curr)));
                    ++position;
                }
                else if(curr == ')'){
                    tokens.push_back(Token(TokenType::_RPAREN, std::string(1,curr)));
                    ++position;
                }
                else if(curr == '{'){
                    tokens.push_back(Token(TokenType::_LBRACKET, std::string(1,curr)));
                    ++position;
                }
                else if(curr == '}'){
                    tokens.push_back(Token(TokenType::_RBRACKET, std::string(1,curr)));
                    ++position;
                }
                else if(curr == ','){
                    tokens.push_back(Token(TokenType::_COMMA, std::string(1,curr)));
                    ++position;
                }
                else if(curr == ';'){
                    tokens.push_back(Token(TokenType::_SEMICOLON, std::string(1,curr)));
                    ++position;
                }
                else{
                    tokens.push_back(Token(TokenType::_INVALID, std::string(1,curr)));
                    throw std::runtime_error("Line " + std::to_string(lineNumber) + ": LEXICAL ERROR at char '" + std::string(1, curr) + "'");
                }
            }
            tokens.push_back(Token(TokenType::_EOF, ""));
            printTokens(tokens);
}

Token Lexer::nextToken(){
    if(tokens.empty()){
        tokenize();
    }
    if(_nextToken >= tokens.size()){
            return Token(TokenType::_EOF, "");
    }
    return tokens[_nextToken++];
}

Token Lexer::peekAtNext(){
    if(tokens.empty()){
        tokenize();
    }
    if(_nextToken >= tokens.size()){
        return Token(TokenType::_EOF, "");
    }
    return tokens[_nextToken];
}

size_t Lexer::getNextTokenPosition() const{ 
    return _nextToken; 
}

void Lexer::setNextTokenPosition(const size_t nt){ 
    _nextToken = nt; 
}

void Lexer::printTokens(const std::vector<Token>& tokens) const{
    for(const auto& token: tokens){
        std::cout << "Token: type - " + tokenTypeToString.at(token.type);      
        if(token.type != TokenType::_EOF && token.type != TokenType::_INVALID){
            std::cout << "\t| value - " << token.value;
        }
        std::cout << "\n";
    }
}

std::string Lexer::getID(){
    size_t start = position;
    while(position < input.size() && (std::isalnum(input[position]) || input[position] == '_')){
        ++position;
    }
    return input.substr(start, position - start);
}

std::string Lexer::getLiteral(){
    size_t start = position;
    while(position < input.size() && std::isdigit(input[position])){
        ++position;
    }
    return input.substr(start, position - start);
}

bool Lexer::isKeyword(const std::string& value) const{
    return keywords.find(value) != keywords.end();
}

bool Lexer::isAritOperator(char curr) const{
    return aritOperators.find(std::string(1, curr)) != aritOperators.end();
}

bool Lexer::isRelOperator(char curr) const{
    return relOperators.find(std::string(1,curr)) != relOperators.end() || curr == '=';
}

std::string Lexer::getRelOperator(){
    std::string op(1, input[position++]);
    if(position < input.size() && (op == "=" || op == "!" || op == "<" || op == ">")){
        char next = input[position];
        if((op == "=" || op == "!" || op == "<" || op == ">") && next == '='){
            op+= next;
            ++position;
        }
    }
    return op;
}