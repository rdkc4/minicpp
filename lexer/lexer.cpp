#include "lexer.hpp"

#include <cctype>
#include <format>
#include <sstream>
#include <string_view>

#include "../common/defs/defs.hpp"
#include "defs/lexer_defs.hpp"

Lexer::Lexer(const std::vector<std::string>& input) : input{ input }, nextTokenIdx{ 1 } {}

void Lexer::tokenize(){
    const size_t fileCount{ input.size() };

    for(fileIndex = 0; fileIndex < fileCount; ++fileIndex){
        position = 0;
        lineNumber = 1;
        prevLineLen = 0;
        fileLength = input[fileIndex].length();

        while(position < fileLength){
            if(input[fileIndex][position] == '\n'){
                updateLine();
            }
            else if(std::isspace(input[fileIndex][position])){
                updatePosition();
            }
            else if(std::isalpha(input[fileIndex][position])){
                pushID();
            }
            else if(std::isdigit(input[fileIndex][position])){
                pushLiteral();
            }
            else if(isSignedLiteral()){
                pushLiteral(true);
            }
            else if(input[fileIndex][position] == '/' && position < fileLength - 1 && input[fileIndex][position + 1] == '/'){
                singleLineComment();
            }
            else if(input[fileIndex][position] == '/' && position < fileLength - 1 && input[fileIndex][position + 1] == '*'){
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
            else if(input[fileIndex][position] == '('){
                tokens.push_back(Token{
                    std::string_view{&input[fileIndex][position],1},
                    lineNumber, 
                    position + 1 - prevLineLen, 
                    TokenType::_LPAREN
                });
                updatePosition();
            }
            else if(input[fileIndex][position] == ')'){
                tokens.push_back(Token{
                    std::string_view{&input[fileIndex][position],1}, 
                    lineNumber, 
                    position + 1 - prevLineLen, 
                    TokenType::_RPAREN
                });
                updatePosition();
            }
            else if(input[fileIndex][position] == '{'){
                tokens.push_back(Token{
                    std::string_view{&input[fileIndex][position], 1}, 
                    lineNumber, 
                    position + 1 - prevLineLen, 
                    TokenType::_LBRACKET
                });
                updatePosition();
            }
            else if(input[fileIndex][position] == '}'){
                tokens.push_back(Token{
                    std::string_view{&input[fileIndex][position],1}, 
                    lineNumber, 
                    position + 1 - prevLineLen, 
                    TokenType::_RBRACKET
                });
                updatePosition();
            }
            else if(input[fileIndex][position] == ','){
                tokens.push_back(Token{
                    std::string_view{&input[fileIndex][position],1}, 
                    lineNumber, 
                    position + 1 - prevLineLen, 
                    TokenType::_COMMA
                });
                updatePosition();
            }
            else if(input[fileIndex][position] == ';'){
                tokens.push_back(Token{
                    std::string_view{&input[fileIndex][position],1}, 
                    lineNumber, 
                    position + 1 - prevLineLen, 
                    TokenType::_SEMICOLON
                });
                updatePosition();
            }
            else if(input[fileIndex][position] == ':'){
                tokens.push_back(Token{
                    std::string_view{&input[fileIndex][position], 1}, 
                    lineNumber, 
                    position + 1 - prevLineLen, 
                    TokenType::_COLON
                });
                updatePosition();
            }
            else if(input[fileIndex][position] == '#'){
                tokens.push_back(Token{ 
                    std::string_view{&input[fileIndex][position], 1}, 
                    lineNumber, 
                    position + 1 - prevLineLen, 
                    TokenType::_HASH
                });
                updatePosition();
            }
            else{
                tokens.push_back(Token{
                    std::string_view{&input[fileIndex][position], 1}, 
                    lineNumber, 
                    position + 1 - prevLineLen, 
                    TokenType::_INVALID
                });
                lexicalErrors.push_back(
                    std::format("Line {}, Column {}: LEXICAL ERROR -> unknown symbol '{}'\n", 
                        lineNumber, position - prevLineLen, std::string_view{&input[fileIndex][position], 1})
                );
                updatePosition();
            }
        }
    }
    tokens.push_back(Token{"", lineNumber, position - prevLineLen, TokenType::_EOF});
}

bool Lexer::completedTokenization() const noexcept {
    return tokens.size() > 0 && tokens.back().type == TokenType::_EOF;
}

bool Lexer::hasLexicalErrors() const noexcept{
    return !lexicalErrors.empty();
}

std::string Lexer::getLexicalErrors() const noexcept {
    if(lexicalErrors.empty()){
        return "";
    }

    std::stringstream errors{"Lexical check failed:\n"};
    for(const auto& error : lexicalErrors){
        errors << error << "\n";
    }

    return errors.str();
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
    while(position < fileLength && (std::isalnum(input[fileIndex][position]) || input[fileIndex][position] == '_')){
        updatePosition(); 
    }
    std::string id{ input[fileIndex].substr(start, position - start) };

    if(isKeyword(id)){
        GeneralTokenType gtype{ 
            types.find(keywords.at(id)) != types.end() 
                ? GeneralTokenType::TYPE 
                : GeneralTokenType::OTHER 
        };
        tokens.push_back(Token{
            id, 
            lineNumber, 
            position - prevLineLen, 
            keywords.at(id), 
            gtype
        });
    }
    else{
        tokens.push_back(Token{ 
            id, 
            lineNumber, 
            position - prevLineLen, 
            TokenType::_ID, 
            GeneralTokenType::VALUE
        });
    }
}

/// literal ([+-]?[0-9][0-9]*[u]?)
void Lexer::pushLiteral(bool sign){
    size_t start{ sign ? ++position : position };
    while(position < fileLength && std::isdigit(input[fileIndex][position])){
        updatePosition();
    }
    if(position < fileLength && input[fileIndex][position] == 'u'){
        updatePosition();
    }
    if(sign && input[fileIndex][start-1] == '-'){
        --start;
    }
    tokens.push_back(Token{
        std::string_view{&input[fileIndex][start], position - start}, 
        lineNumber, 
        position - prevLineLen, 
        TokenType::_LITERAL, 
        GeneralTokenType::VALUE
    });
}

/// assign [=]
void Lexer::pushAssignOperator(){
    tokens.push_back(Token{
        std::string_view{&input[fileIndex][position], 1}, 
        lineNumber, 
        position + 1 - prevLineLen, 
        TokenType::_ASSIGN
    });
    updatePosition();
}

/// bitwise operators (&, |, ^, <<, >>)
void Lexer::pushBitwiseOperator(){
    const size_t start{ position };
    if(input[fileIndex][position] == '<' || input[fileIndex][position] == '>'){
        updatePosition();
    }
    updatePosition();
    tokens.push_back(Token{
        std::string_view{&input[fileIndex][start], position - start}, 
        lineNumber, 
        position - prevLineLen, 
        TokenType::_BITWISE, 
        GeneralTokenType::OPERATOR
    });
}

/// arithmetic operators (+, -, *, /)
void Lexer::pushAritOperator(){
    tokens.push_back(Token{
        std::string_view{&input[fileIndex][position], 1}, 
        lineNumber, 
        position + 1 - prevLineLen, 
        TokenType::_AROP, 
        GeneralTokenType::OPERATOR
    });
    updatePosition();
}

/// relational operators (<, >, <=, >=, ==, !=)
void Lexer::pushRelOperator(){
    const size_t start{ position };
    if(position < fileLength - 1 && 
        relationalOperators.find(std::string{&input[fileIndex][position], 2}) != relationalOperators.end()
    ){
        updatePosition(2);
    }
    else{
        updatePosition();
    }
    // general token type is OTHER, not yet included in numerical expressions 
    tokens.push_back(Token{
        std::string_view{&input[fileIndex][start], position - start}, 
        lineNumber, 
        position - prevLineLen, 
        TokenType::_RELOP
    });
}

bool Lexer::isKeyword(const std::string& value) const noexcept {
    return keywords.find(value) != keywords.end();
}

bool Lexer::isSignedLiteral() const noexcept {
    return (
        (input[fileIndex][position] == '-' || input[fileIndex][position] == '+') && 
        position < fileLength - 1 && std::isdigit(input[fileIndex][position + 1]) &&
        (!tokens.empty() && tokens.back().type != TokenType::_LITERAL &&
            tokens.back().type != TokenType::_ID && tokens.back().type != TokenType::_RPAREN
        )
    );
}

bool Lexer::isAssignOperator() const noexcept {
    return input[fileIndex][position] == '=' && (position + 1 >= fileLength || input[fileIndex][position + 1] != '=');
}

bool Lexer::isAritOperator() const {
    return arithmeticOperators.find(std::string{&input[fileIndex][position], 1}) != arithmeticOperators.end();
}

bool Lexer::isBitwiseOperator() const {
    return (bitwiseOperators.find(std::string{&input[fileIndex][position], 1}) != bitwiseOperators.end() || 
        (position + 1 < fileLength && 
            bitwiseOperators.find(std::string{&input[fileIndex][position], 2}) != bitwiseOperators.end()
        )
    );
}

bool Lexer::isRelOperator() const {
    return (relationalOperators.find(std::string{&input[fileIndex][position], 1}) != relationalOperators.end() || 
        (position + 1 < fileLength && 
            relationalOperators.find(std::string{&input[fileIndex][position], 2}) != relationalOperators.end()
        )
    );
}

void Lexer::singleLineComment(){
    updatePosition(2);
    while(position < fileLength && input[fileIndex][position] != '\n'){
        updatePosition();
    }
}

void Lexer::multiLineComment(){
    const size_t startLine{ lineNumber };
    const size_t startColumn{ position - prevLineLen };

    updatePosition(2);
    while(position < fileLength - 1){
        if(input[fileIndex][position] == '*' && input[fileIndex][position + 1] == '/'){
            updatePosition(2);
            return;
        }
        if(input[fileIndex][position] == '\n'){
            updateLine();
        }
        updatePosition();
    }
    lexicalErrors.push_back(
        std::format("Line {}, Column {}: SYNTAX ERROR -> multi-line comment starting at line {}, column {}: not closed\n",
            lineNumber, position - prevLineLen, startLine, startColumn
        )
    );
}