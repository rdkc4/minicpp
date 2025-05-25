#include "lexer.hpp"

#include <iostream>
#include <regex>
#include <stdexcept>
#include <format>
#include <string_view>

#include "../common/defs/defs.hpp"
#include "defs/lexer_defs.hpp"

Lexer::Lexer(std::string_view input) : input{ input }, position{ 0 }, lineNumber{ 1 }, prevLineLen{ 0 }, nextTokenIdx{ 0 } {}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// LEXICAL CHECK
// tokenization of entire file
//-----------------------------------------------------------------------------------------------------------------------------------------------------
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
            getBitwiseOperator();
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
            tokens.push_back(Token{std::string(1,curr), lineNumber, position - prevLineLen, TokenType::_LPAREN});
            updatePosition();
        }
        else if(curr == ')'){
            tokens.push_back(Token{std::string(1,curr), lineNumber, position - prevLineLen, TokenType::_RPAREN});
            updatePosition();
        }
        else if(curr == '{'){
            tokens.push_back(Token{std::string(1,curr), lineNumber, position - prevLineLen, TokenType::_LBRACKET});
            updatePosition();
        }
        else if(curr == '}'){
            tokens.push_back(Token{std::string(1,curr), lineNumber, position - prevLineLen, TokenType::_RBRACKET});
            updatePosition();
        }
        else if(curr == ','){
            tokens.push_back(Token{std::string(1,curr), lineNumber, position - prevLineLen, TokenType::_COMMA});
            updatePosition();
        }
        else if(curr == ';'){
            tokens.push_back(Token{std::string(1,curr), lineNumber, position - prevLineLen, TokenType::_SEMICOLON});
            updatePosition();
        }
        else if(curr == ':'){
            tokens.push_back(Token{std::string(1, curr), lineNumber, position - prevLineLen, TokenType::_COLON});
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


//-----------------------------------------------------------------------------------------------------------------------------------------------------
// MOVING ON TO THE NEXT TOKEN
//-----------------------------------------------------------------------------------------------------------------------------------------------------
const Token& Lexer::nextToken() noexcept {
    if(nextTokenIdx >= tokens.size()){
        return tokens.back();
    }
    return tokens[nextTokenIdx++];
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETRIEVAL OF NEXT TOKEN
//-----------------------------------------------------------------------------------------------------------------------------------------------------
const Token& Lexer::peekAtNext() const noexcept {
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

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// retrieval of ID token ([A-Za-z][A-Za-z0-9_]*)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::getID(){
    const static std::regex id_pattern{ "[a-zA-Z][a-zA-Z0-9_]*" };
    static std::match_results<std::string::iterator> match;
    std::regex_search(input.begin() + position, input.end(), match, id_pattern);
    
    updatePosition(match.length());
    tokens.push_back(Token{ match[0].str(), lineNumber, position - prevLineLen, 
        isKeyword(match[0]) ? keywords.at(match[0]) : TokenType::_ID});
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// retrieval of Literal token ([+-]?[0-9][0-9]*[u]?)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::getLiteral(bool sign){
    const static std::regex literal_pattern{ "[+-]?(0|[1-9][0-9]*)[u]?" };
    static std::match_results<std::string::iterator> match;
    std::regex_search(input.begin() + position, input.end(), match, literal_pattern);

    std::string_view literal{ sign && match[0].str()[0] == '+' ? match[0].str().substr(1, match[0].length() - 1) : match[0] };
    updatePosition(match[0].length());
    tokens.push_back(Token{literal, lineNumber, position - prevLineLen, TokenType::_LITERAL});
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// retrieval of assign token 
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::getAssignOperator(char curr){
    tokens.push_back(Token{std::string(1, curr), lineNumber, position - prevLineLen, TokenType::_ASSIGN});
    updatePosition();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// retrieval of bitwise operator
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::getBitwiseOperator(){
    const static std::regex bitwise_pattern{ "^(>>|<<|&|\\||\\^)" };
    static std::match_results<std::string::iterator> match;
    std::regex_search(input.begin() + position, input.end(), match, bitwise_pattern);

    updatePosition(match[0].length());
    tokens.push_back(Token(match[0].str(), lineNumber, position - prevLineLen, TokenType::_BITWISE));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// retrieval of arithmetic operator
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::getAritOperator(char curr){
    tokens.push_back(Token{std::string(1, curr), lineNumber, position - prevLineLen, TokenType::_AROP});
    updatePosition();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// retrieval of relational operator
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::getRelOperator(){
    const static std::regex relop_pattern{ "<=|>=|==|!=|>|<" };
    static std::match_results<std::string::iterator> match;
    std::regex_search(input.begin() + position, input.end(), match, relop_pattern);

    updatePosition(match[0].length());
    tokens.push_back(Token{match[0].str(), lineNumber, position - prevLineLen, TokenType::_RELOP});
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// check for predefined keywords
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool Lexer::isKeyword(const std::string& value) const noexcept {
    return keywords.find(value) != keywords.end();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// check for sign in ahead of literal
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool Lexer::isSignedLiteral(char curr) const noexcept {
    return ((curr == '-' || curr == '+') && position < input.size()-1 && std::isdigit(input[position+1]) 
            && (!tokens.empty() && tokens[tokens.size()-1].type != TokenType::_LITERAL 
            && tokens[tokens.size()-1].type != TokenType::_ID && tokens[tokens.size()-1].type != TokenType::_RPAREN));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// check for assign operator
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool Lexer::isAssignOperator(char curr) const noexcept {
    return curr == '=' && (position + 1 >= input.size() || input[position+1] != '=');
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// arithmetic operator checking (+, -, *, /)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool Lexer::isAritOperator(char curr) const {
    return arithmeticOperators.find(std::string(1, curr)) != arithmeticOperators.end();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// bitwise operator checking (&, |, ^, <<, >>)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool Lexer::isBitwiseOperator(char curr) const {
    return bitwiseOperators.find(std::string(1, curr)) != bitwiseOperators.end()
        || (position + 1 < input.size() && bitwiseOperators.find(std::string(1, curr) + input[position + 1]) != bitwiseOperators.end());
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// relational operator checking (<, >, <=, >=, ==, !=)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool Lexer::isRelOperator(char curr) const {
    return relationalOperators.find(std::string(1,curr)) != relationalOperators.end() 
        || (position + 1 < input.size() && relationalOperators.find(std::string(1, curr) + input[position + 1]) != relationalOperators.end());
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// COMMENTS - // single-line
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::singleLineComment(){
    const static std::regex comment_pattern{ "//(.*?)(\n|$)" };
    static std::match_results<std::string::iterator> match;
    std::regex_search(input.begin() + position, input.end(), match, comment_pattern);
    
    updatePosition(match[0].length());
    updateLine();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// COMMENTS - /* multi-line */
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Lexer::multiLineComment(){
    size_t startLine{ lineNumber };
    size_t startPosition{ position - prevLineLen };

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
            lineNumber, position - prevLineLen, startLine, startPosition));
    }
}