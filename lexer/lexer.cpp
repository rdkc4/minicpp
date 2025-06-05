#include "lexer.hpp"

#include <cctype>
#include <iostream>
#include <stdexcept>
#include <format>
#include <string_view>

#include "../common/defs/defs.hpp"
#include "defs/lexer_defs.hpp"

Lexer::Lexer(std::string_view input) : input{ input }, position{ 0 }, lineNumber{ 1 }, prevLineLen{ 0 }, nextTokenIdx{ 0 } {}

void Lexer::tokenize(){
    while(position < input.size()){
        char curr{ input[position] };
        
        if(curr == '\n'){
            updateLine();
            updatePosition();
        }
        else if(std::isspace(curr)){
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
            tokens.push_back(Token{std::string_view{&curr,1}, lineNumber, position - prevLineLen, TokenType::_LPAREN});
            updatePosition();
        }
        else if(curr == ')'){
            tokens.push_back(Token{std::string_view{&curr,1}, lineNumber, position - prevLineLen, TokenType::_RPAREN});
            updatePosition();
        }
        else if(curr == '{'){
            tokens.push_back(Token{std::string_view{&curr,1}, lineNumber, position - prevLineLen, TokenType::_LBRACKET});
            updatePosition();
        }
        else if(curr == '}'){
            tokens.push_back(Token{std::string_view{&curr,1}, lineNumber, position - prevLineLen, TokenType::_RBRACKET});
            updatePosition();
        }
        else if(curr == ','){
            tokens.push_back(Token{std::string_view{&curr,1}, lineNumber, position - prevLineLen, TokenType::_COMMA});
            updatePosition();
        }
        else if(curr == ';'){
            tokens.push_back(Token{std::string_view{&curr,1}, lineNumber, position - prevLineLen, TokenType::_SEMICOLON});
            updatePosition();
        }
        else if(curr == ':'){
            tokens.push_back(Token{std::string_view{&curr,1}, lineNumber, position - prevLineLen, TokenType::_COLON});
            updatePosition();
        }
        else{
            tokens.push_back(Token{std::string(1,curr), lineNumber, position - prevLineLen, TokenType::_INVALID});
            throw std::runtime_error(std::format("Line {}, Column {}: LEXICAL ERROR -> unknown symbol '{}'", 
                lineNumber, position - prevLineLen, std::string(1, curr)));
        }
    }
    tokens.push_back(Token{"", lineNumber, position - prevLineLen, TokenType::_EOF});
    //printTokens();
}

// returns current token and moves onto the next
const Token& Lexer::nextToken() noexcept {
    if(nextTokenIdx >= tokens.size()){
        return tokens.back();
    }
    return tokens[nextTokenIdx++];
}

// returns next token
const Token& Lexer::peekAtNext() const noexcept {
    if(nextTokenIdx >= tokens.size()){
        return tokens.back();
    }
    return tokens[nextTokenIdx];
}

void Lexer::printTokens() const {
    for(const auto& token: tokens){
        std::cout << std::format("Token: type - {}\t value - {}\n", tokenTypeToString.at(token.type), token.value);
    }
}

void Lexer::updatePosition() noexcept {
    ++position;
}

void Lexer::updatePosition(size_t n) noexcept {
    position += n;
}

void Lexer::updateLine() noexcept {
    ++lineNumber;
    prevLineLen = position;
}

// ID ([A-Za-z][A-Za-z0-9_]*)
void Lexer::getID(){
    const size_t start{ position };
    while(position < input.size() && (std::isalnum(input[position]) || input[position] == '_')){
        updatePosition(); 
    }
    std::string id{ input.substr(start, position - start) };
    tokens.push_back(Token{ id, lineNumber, position - prevLineLen, isKeyword(id) ? keywords.at(id) : TokenType::_ID});
}

// literal ([+-]?[0-9][0-9]*[u]?)
void Lexer::getLiteral(bool sign){
    size_t start{ sign ? ++position : position };
    while(position < input.size() && std::isdigit(input[position])){
        updatePosition();
    }
    if(position < input.size() && input[position] == 'u'){
        updatePosition();
    }
    if(sign && input[start-1] == '-'){
        --start;
    }
    tokens.push_back(Token{std::string_view{&input[start], position - start}, lineNumber, position - prevLineLen, TokenType::_LITERAL});
}

// assign [=]
void Lexer::getAssignOperator(char curr){
    tokens.push_back(Token{std::string_view{&curr, 1}, lineNumber, position - prevLineLen, TokenType::_ASSIGN});
    updatePosition();
}

// bitwise operators (&, |, ^, <<, >>)
void Lexer::getBitwiseOperator(char curr){
    const size_t start{ position };
    if(curr == '<' || curr == '>'){
        updatePosition();
    }
    updatePosition();
    tokens.push_back(Token(std::string_view{&input[start], position - start}, lineNumber, position - prevLineLen, TokenType::_BITWISE));
}

// arithmetic operators (+, -, *, /)
void Lexer::getAritOperator(char curr){
    tokens.push_back(Token{std::string_view{&curr, 1}, lineNumber, position - prevLineLen, TokenType::_AROP});
    updatePosition();
}

// relational operators (<, >, <=, >=, ==, !=)
void Lexer::getRelOperator(){
    const size_t start{ position };
    if(position < input.size() - 1 && (input[position + 1] == '=' || input[position + 1] == '!' || input[position + 1] == '<' || input[position + 1] == '>')){
        updatePosition(2);
    }
    else{
        updatePosition();
    }
    tokens.push_back(Token{std::string_view{&input[start], position - start}, lineNumber, position - prevLineLen, TokenType::_RELOP});
}

bool Lexer::isKeyword(const std::string& value) const noexcept {
    return keywords.find(value) != keywords.end();
}

bool Lexer::isSignedLiteral(char curr) const noexcept {
    return ((curr == '-' || curr == '+') && position < input.size()-1 && std::isdigit(input[position+1]) 
            && (!tokens.empty() && tokens[tokens.size()-1].type != TokenType::_LITERAL 
            && tokens[tokens.size()-1].type != TokenType::_ID && tokens[tokens.size()-1].type != TokenType::_RPAREN));
}

bool Lexer::isAssignOperator(char curr) const noexcept {
    return curr == '=' && (position + 1 >= input.size() || input[position+1] != '=');
}

bool Lexer::isAritOperator(char curr) const {
    return arithmeticOperators.find(std::string(1, curr)) != arithmeticOperators.end();
}

bool Lexer::isBitwiseOperator(char curr) const {
    return bitwiseOperators.find(std::string(1, curr)) != bitwiseOperators.end()
        || (position + 1 < input.size() && bitwiseOperators.find(std::string(1, curr) + input[position + 1]) != bitwiseOperators.end());
}

bool Lexer::isRelOperator(char curr) const {
    return relationalOperators.find(std::string(1,curr)) != relationalOperators.end() 
        || (position + 1 < input.size() && relationalOperators.find(std::string(1, curr) + input[position + 1]) != relationalOperators.end());
}

void Lexer::singleLineComment(){
    updatePosition(2);
    while(position < input.size() && input[position] != '\n'){
        updatePosition();
    }
}

void Lexer::multiLineComment(){
    const size_t startLine{ lineNumber };
    const size_t startColumn{ position - prevLineLen };

    updatePosition(2);
    while(position < input.size() - 1 && input[position] != '*' && input[position+1] != '/'){
        if(input[position] == '\n'){
            updateLine();
        }
        updatePosition();
    }
    if(input[position] == '*' && input[position + 1] == '/'){
        updatePosition(2);
    }
    else{
        throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> multi-line comment starting at line {}, column {}: not closed\n",
            lineNumber, position - prevLineLen, startLine, startColumn));
    }
}