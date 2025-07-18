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
    const size_t inputSize = input.size();

    while(position < inputSize){
        if(input[position] == '\n'){
            updateLine();
        }
        else if(std::isspace(input[position])){
            updatePosition();
        }
        else if(std::isalpha(input[position])){
            getID();
        }
        else if(std::isdigit(input[position])){
            getLiteral();
        }
        else if(isSignedLiteral()){
            getLiteral(true);
        }
        else if(input[position] == '/' && position < inputSize - 1 && input[position + 1] == '/'){
            singleLineComment();
        }
        else if(input[position] == '/' && position < inputSize - 1 && input[position + 1] == '*'){
            multiLineComment();
        }
        else if(isBitwiseOperator()){
            getBitwiseOperator();
        }
        else if(isAritOperator()){
            getAritOperator();
        }
        else if(isAssignOperator()){
            getAssignOperator();
        }
        else if(isRelOperator()){
            getRelOperator();
        }
        else if(input[position] == '('){
            tokens.push_back(Token{std::string_view{&input[position],1}, lineNumber, position + 1 - prevLineLen, TokenType::_LPAREN});
            updatePosition();
        }
        else if(input[position] == ')'){
            tokens.push_back(Token{std::string_view{&input[position],1}, lineNumber, position + 1 - prevLineLen, TokenType::_RPAREN});
            updatePosition();
        }
        else if(input[position] == '{'){
            tokens.push_back(Token{std::string_view{&input[position], 1}, lineNumber, position + 1 - prevLineLen, TokenType::_LBRACKET});
            updatePosition();
        }
        else if(input[position] == '}'){
            tokens.push_back(Token{std::string_view{&input[position],1}, lineNumber, position + 1 - prevLineLen, TokenType::_RBRACKET});
            updatePosition();
        }
        else if(input[position] == ','){
            tokens.push_back(Token{std::string_view{&input[position],1}, lineNumber, position + 1 - prevLineLen, TokenType::_COMMA});
            updatePosition();
        }
        else if(input[position] == ';'){
            tokens.push_back(Token{std::string_view{&input[position],1}, lineNumber, position + 1 - prevLineLen, TokenType::_SEMICOLON});
            updatePosition();
        }
        else if(input[position] == ':'){
            tokens.push_back(Token{std::string_view{&input[position], 1}, lineNumber, position + 1 - prevLineLen, TokenType::_COLON});
            updatePosition();
        }
        else{
            tokens.push_back(Token{std::string_view{&input[position], 1}, lineNumber, position + 1 - prevLineLen, TokenType::_INVALID});
            exceptions.push_back(
                std::format("Line {}, Column {}: LEXICAL ERROR -> unknown symbol '{}'\n", 
                    lineNumber, position - prevLineLen, std::string_view{&input[position], 1})
            );
            updatePosition();
        }
    }
    tokens.push_back(Token{"", lineNumber, position - prevLineLen, TokenType::_EOF});
    checkLexicalErrors();
    //printTokens();
}

void Lexer::checkLexicalErrors() const {
    if(!exceptions.empty()) {
        std::string err{ std::format("Invalid token count: {}\n", exceptions.size()) };
        for(const auto& ex : exceptions){
            err += ex;
        }
        throw std::runtime_error(err);
    }
}

bool Lexer::completedTokenization() const noexcept {
    return tokens.size() > 0 && tokens.back().type == TokenType::_EOF;
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

void Lexer::printTokens() const noexcept {
    for(const auto& token: tokens){
        std::cout << std::format("Token: type - {:<15} gtype - {:<10} line - {:<5}\t column - {:<5} value - {}\n", 
            tokenTypeToString.at(token.type), generalTokenTypeToString.at(token.gtype), token.line, token.column, token.value);
    }
}

void Lexer::updatePosition() noexcept {
    ++position;
}

void Lexer::updatePosition(size_t n) noexcept {
    position += n;
}

void Lexer::updateLine() noexcept {
    updatePosition();
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

    if(isKeyword(id)){
        GeneralTokenType gtype = types.find(keywords.at(id)) != types.end() ? GeneralTokenType::TYPE : GeneralTokenType::OTHER;
        tokens.push_back(Token{ id, lineNumber, position - prevLineLen, keywords.at(id), gtype});
    }
    else{
        tokens.push_back(Token{ id, lineNumber, position - prevLineLen, TokenType::_ID, GeneralTokenType::VALUE});
    }
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
    tokens.push_back(Token{std::string_view{&input[start], position - start}, lineNumber, position - prevLineLen, TokenType::_LITERAL, GeneralTokenType::VALUE});
}

// assign [=]
void Lexer::getAssignOperator(){
    tokens.push_back(Token{std::string_view{&input[position], 1}, lineNumber, position + 1 - prevLineLen, TokenType::_ASSIGN});
    updatePosition();
}

// bitwise operators (&, |, ^, <<, >>)
void Lexer::getBitwiseOperator(){
    const size_t start{ position };
    if(input[position] == '<' || input[position] == '>'){
        updatePosition();
    }
    updatePosition();
    tokens.push_back(Token{std::string_view{&input[start], position - start}, lineNumber, position - prevLineLen, TokenType::_BITWISE, GeneralTokenType::OPERATOR});
}

// arithmetic operators (+, -, *, /)
void Lexer::getAritOperator(){
    tokens.push_back(Token{std::string_view{&input[position], 1}, lineNumber, position + 1 - prevLineLen, TokenType::_AROP, GeneralTokenType::OPERATOR});
    updatePosition();
}

// relational operators (<, >, <=, >=, ==, !=)
void Lexer::getRelOperator(){
    const size_t start{ position };
    if(position < input.size() - 1 && relationalOperators.find(std::string{&input[position], 2}) != relationalOperators.end()){
        updatePosition(2);
    }
    else{
        updatePosition();
    }
    // general token type is OTHER, not yet included in numerical expressions 
    tokens.push_back(Token{std::string_view{&input[start], position - start}, lineNumber, position - prevLineLen, TokenType::_RELOP});
}

bool Lexer::isKeyword(const std::string& value) const noexcept {
    return keywords.find(value) != keywords.end();
}

bool Lexer::isSignedLiteral() const noexcept {
    return ((input[position] == '-' || input[position] == '+') && position < input.size() - 1 && std::isdigit(input[position + 1]) 
            && (!tokens.empty() && tokens.back().type != TokenType::_LITERAL 
            && tokens.back().type != TokenType::_ID && tokens.back().type != TokenType::_RPAREN));
}

bool Lexer::isAssignOperator() const noexcept {
    return input[position] == '=' && (position + 1 >= input.size() || input[position + 1] != '=');
}

bool Lexer::isAritOperator() const {
    return arithmeticOperators.find(std::string{&input[position], 1}) != arithmeticOperators.end();
}

bool Lexer::isBitwiseOperator() const {
    return bitwiseOperators.find(std::string{&input[position], 1}) != bitwiseOperators.end()
        || (position + 1 < input.size() && bitwiseOperators.find(std::string{&input[position], 2}) != bitwiseOperators.end());
}

bool Lexer::isRelOperator() const {
    return relationalOperators.find(std::string{&input[position], 1}) != relationalOperators.end() 
        || (position + 1 < input.size() && relationalOperators.find(std::string{&input[position], 2}) != relationalOperators.end());
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
    while(position < input.size() - 1){
        if(input[position] == '*' && input[position + 1] == '/'){
            updatePosition(2);
            return;
        }
        if(input[position] == '\n'){
            updateLine();
        }
        updatePosition();
    }
    exceptions.push_back(
        std::format("Line {}, Column {}: SYNTAX ERROR -> multi-line comment starting at line {}, column {}: not closed\n",
            lineNumber, position - prevLineLen, startLine, startColumn)
    );
}