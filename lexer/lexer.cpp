#include "lexer.hpp"

#include <cctype>
#include <iostream>
#include <format>
#include <string_view>

#include "../common/defs/defs.hpp"
#include "defs/lexer_defs.hpp"

Lexer::Lexer(std::string_view input) : input{ input }, position{ 0 }, lineNumber{ 1 }, prevLineLen{ 0 }, nextTokenIdx{ 1 } {}

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
            pushID();
        }
        else if(std::isdigit(input[position])){
            pushLiteral();
        }
        else if(isSignedLiteral()){
            pushLiteral(true);
        }
        else if(input[position] == '/' && position < inputSize - 1 && input[position + 1] == '/'){
            singleLineComment();
        }
        else if(input[position] == '/' && position < inputSize - 1 && input[position + 1] == '*'){
            multiLineComment();
        }
        else if(isBitwiseOperator()){
            pushBitwiseOperator();
        }
        else if(isAritOperator()){
            pushAritOperator();
        }
        else if(isAssignOperator()){
            pushAssignOperator();
        }
        else if(isRelOperator()){
            pushRelOperator();
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
    //printTokens();
}

bool Lexer::completedTokenization() const noexcept {
    return tokens.size() > 0 && tokens.back().type == TokenType::_EOF;
}

bool Lexer::hasLexicalErrors() const noexcept{
    return !exceptions.empty();
}

void Lexer::showLexicalErrors() const{
    std::string err{ std::format("\nLexical check: failed\nInvalid token count: {}\n", exceptions.size()) };
    for(const auto& exception : exceptions){
        err += std::format("{}\n", exception);
    }
    std::cerr << err;
}

void Lexer::next() noexcept {
    ++nextTokenIdx;
}

const Token& Lexer::peek() const noexcept {
    if(nextTokenIdx >= tokens.size()){
        return tokens.back();
    }
    return tokens[nextTokenIdx];
}

const Token& Lexer::current() const noexcept {
    if(nextTokenIdx >= tokens.size()){
        return tokens.back();
    }
    return tokens[nextTokenIdx - 1];
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

/// ID ([A-Za-z][A-Za-z0-9_]*)
void Lexer::pushID(){
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

/// literal ([+-]?[0-9][0-9]*[u]?)
void Lexer::pushLiteral(bool sign){
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

/// assign [=]
void Lexer::pushAssignOperator(){
    tokens.push_back(Token{std::string_view{&input[position], 1}, lineNumber, position + 1 - prevLineLen, TokenType::_ASSIGN});
    updatePosition();
}

/// bitwise operators (&, |, ^, <<, >>)
void Lexer::pushBitwiseOperator(){
    const size_t start{ position };
    if(input[position] == '<' || input[position] == '>'){
        updatePosition();
    }
    updatePosition();
    tokens.push_back(Token{std::string_view{&input[start], position - start}, lineNumber, position - prevLineLen, TokenType::_BITWISE, GeneralTokenType::OPERATOR});
}

/// arithmetic operators (+, -, *, /)
void Lexer::pushAritOperator(){
    tokens.push_back(Token{std::string_view{&input[position], 1}, lineNumber, position + 1 - prevLineLen, TokenType::_AROP, GeneralTokenType::OPERATOR});
    updatePosition();
}

/// relational operators (<, >, <=, >=, ==, !=)
void Lexer::pushRelOperator(){
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