#include "lexer.hpp"

#include <iostream>
#include <stdexcept>
#include <format>
#include <string_view>

#include "../common/defs/defs.hpp"
#include "defs/lexer_defs.hpp"

Lexer::Lexer(std::string_view input) : input{input}, position{0}, lineNumber{1}, prevLineLen{0}, nextTokenIdx{0}{}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// LEXICAL CHECK
// tokenization of entire file
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::tokenize(){
    while(position < input.size()){
        char curr = input[position];
        
        if(std::isspace(curr)){
            updatePosition();
        }
        else if(std::isalpha(curr)){
            getID();
        }
        else if(std::isdigit(curr)){
            getLiteral();
        }
        else if(isSignedLiteral(curr)){
            getLiteral(true);
        }
        else if(curr == '/' && position < input.size() - 1 && input[position + 1] == '/'){
            singleLineComment();
        }
        else if(curr == '/' && position < input.size() - 1 && input[position + 1] == '*'){
            multiLineComment();
        }
        else if(isBitwiseOperator(curr)){
            getBitwiseOperator(curr);
        }
        else if(isAritOperator(curr)){
            getAritOperator(curr);
        }
        else if(isAssignOperator(curr)){
            getAssignOperator(curr);
        }
        else if(isRelOperator(curr)){
            getRelOperator();
        }
        else if(curr == '('){
            tokens.push_back(Token(TokenType::_LPAREN, std::string(1,curr), lineNumber, position - prevLineLen));
            ++position;
        }
        else if(curr == ')'){
            tokens.push_back(Token(TokenType::_RPAREN, std::string(1,curr), lineNumber, position - prevLineLen));
            ++position;
        }
        else if(curr == '{'){
            tokens.push_back(Token(TokenType::_LBRACKET, std::string(1,curr), lineNumber, position - prevLineLen));
            ++position;
        }
        else if(curr == '}'){
            tokens.push_back(Token(TokenType::_RBRACKET, std::string(1,curr), lineNumber, position - prevLineLen));
            ++position;
        }
        else if(curr == ','){
            tokens.push_back(Token(TokenType::_COMMA, std::string(1,curr), lineNumber, position - prevLineLen));
            ++position;
        }
        else if(curr == ';'){
            tokens.push_back(Token(TokenType::_SEMICOLON, std::string(1,curr), lineNumber, position - prevLineLen));
            ++position;
        }
        else if(curr == ':'){
            tokens.push_back(Token(TokenType::_COLON, std::string(1, curr), lineNumber, position - prevLineLen));
            ++position;
        }
        else{
            tokens.push_back(Token(TokenType::_INVALID, std::string(1,curr), lineNumber, position - prevLineLen));
            throw std::runtime_error(std::format("Line {}, Column {}: LEXICAL ERROR -> unknown symbol '{}'", 
                lineNumber, position - prevLineLen, std::string(1, curr)));
        }
    }
    tokens.push_back(Token(TokenType::_EOF, "", lineNumber, position - prevLineLen));
    //printTokens();
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------
// MOVING ON TO THE NEXT TOKEN
//-----------------------------------------------------------------------------------------------------------------------------------------------------
const Token& Lexer::nextToken() {
    if(nextTokenIdx >= tokens.size()){
        return tokens.back();
    }
    return tokens[nextTokenIdx++];
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETRIEVAL OF NEXT TOKEN
//-----------------------------------------------------------------------------------------------------------------------------------------------------
const Token& Lexer::peekAtNext() const {
    if(nextTokenIdx >= tokens.size()){
        return tokens.back();
    }
    return tokens[nextTokenIdx];
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DISPLAYING ALL TOKENS (debbuging purpose)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::printTokens() const {
    for(const auto& token: tokens){
        std::cout << std::format("Token: type - {}\t value - {}\n", tokenTypeToString.at(token.type), token.value);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// line/column update
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::updatePosition(){
    ++position;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// retrieval of ID token ([A-Za-z][A-Za-z0-9_]*)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::getID(){
    size_t start = position;
    while(position < input.size() && (std::isalnum(input[position]) || input[position] == '_')){
        ++position;
    }
    std::string id = input.substr(start, position - start);
    if(isKeyword(id)){
        tokens.push_back(Token(keywords.at(id), id, lineNumber, position - prevLineLen));
    }
    else{
        tokens.push_back(Token(TokenType::_ID, id, lineNumber, position - prevLineLen));
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// retrieval of Literal token ([+-]?[0-9][0-9]*[u]?)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::getLiteral(bool sign){
    size_t start = sign ? ++position : position;
    while(position < input.size() && std::isdigit(input[position])){
        ++position;
    }
    if(position < input.size() && input[position] == 'u'){
        ++position;
    }
    std::string literal = input.substr(start, position - start);
    if(sign && input[start-1] == '-'){
        literal = input[start-1] + literal;
    }
    tokens.push_back(Token(TokenType::_LITERAL, literal, lineNumber, position - prevLineLen));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// retrieval of assign token 
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::getAssignOperator(char curr){
    tokens.push_back(Token(TokenType::_ASSIGN, std::string(1, curr), lineNumber, position - prevLineLen));
    ++position;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// retrieval of bitwise operator
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::getBitwiseOperator(char curr){
    std::string op = "";
    if(curr == '<' || curr == '>'){
        op += input[position++];
    }
    op += input[position++];
    tokens.push_back(Token(TokenType::_BITWISE, op, lineNumber, position - prevLineLen));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// retrieval of arithmetic operator
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::getAritOperator(char curr){
    tokens.push_back(Token(TokenType::_AROP, std::string(1, curr), lineNumber, position - prevLineLen));
    ++position;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// retrieval of relational operator
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::getRelOperator(){
    std::string op(1, input[position++]);
    if(position < input.size() && (op == "=" || op == "!" || op == "<" || op == ">")){
        char next = input[position];
        if((op == "=" || op == "!" || op == "<" || op == ">") && next == '='){
            op+= next;
            ++position;
        }
    }
    tokens.push_back(Token(TokenType::_RELOP, op, lineNumber, position - prevLineLen));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// check for predefined keywords
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool Lexer::isKeyword(const std::string& value) const{
    return keywords.find(value) != keywords.end();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// check for sign in ahead of literal
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool Lexer::isSignedLiteral(char curr) const{
    return ((curr == '-' || curr == '+') && position < input.size()-1 && std::isdigit(input[position+1]) 
            && !tokens.empty() && tokens[tokens.size()-1].type != TokenType::_LITERAL 
            && tokens[tokens.size()-1].type != TokenType::_ID && tokens[tokens.size()-1].type != TokenType::_RPAREN);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// check for assign operator
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool Lexer::isAssignOperator(char curr) const{
    return curr == '=' && (position+1 >= input.size() || input[position+1] != '=');
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// arithmetic operator checking (+, -, *, /)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool Lexer::isAritOperator(char curr) const{
    return arithmeticOperators.find(std::string(1, curr)) != arithmeticOperators.end();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// bitwise operator checking (&, |, ^, <<, >>)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool Lexer::isBitwiseOperator(char curr) const{
    return bitwiseOperators.find(std::string(1, curr)) != bitwiseOperators.end()
        || (position + 1 < input.size() && bitwiseOperators.find(std::string(1, curr) + input[position + 1]) != bitwiseOperators.end());
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// relational operator checking (<, >, <=, >=, ==, !=)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool Lexer::isRelOperator(char curr) const{
    return relationalOperators.find(std::string(1,curr)) != relationalOperators.end() 
        || (position + 1 < input.size() && relationalOperators.find(std::string(1, curr) + input[position + 1]) != relationalOperators.end());
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// COMMENTS - // single-line
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::singleLineComment(){
    position += 2;
    while(position < input.size() && input[position] != '\n'){
        ++position;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// COMMENTS - /* multi-line */
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::multiLineComment(){
    size_t startLine = lineNumber;
    size_t startPosition = position - prevLineLen;

    position += 2;
    while(position < input.size()-1 && input[position]!='*' && input[position+1]!='/'){
        if(input[position] == '\n'){
            ++lineNumber;
            prevLineLen = position;
        }
        ++position;
    }
    if(input[position] == '*' && input[position + 1] == '/'){
        position += 2;
    }
    else{
        std::cerr << std::format("Line {}, Column {}: SYNTAX ERROR -> multi-line comment starting at line {}, column {}: not closed\n",
            lineNumber, position - prevLineLen, startLine, startPosition);
    }
}