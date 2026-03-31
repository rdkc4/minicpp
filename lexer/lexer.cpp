#include "lexer.hpp"

#include <format>
#include <sstream>
#include <cctype>

Lexer::Lexer(const std::vector<std::string>& input) 
    : input{ input }, nextTokenIdx{ 1 } {}

void Lexer::tokenize(){
    const size_t fileCount{ input.size() };

    for (fileIndex = 0; fileIndex < fileCount; ++fileIndex) {
        resetState();

        while (position < fileLength) {
            char c{ getChar(position) };

            if(handleNewline(c)) continue;
            if(handleWhitespace(c)) continue;
            if(handleIdentifier(c)) continue;
            if(handleNumber(c)) continue;
            if(handleComment()) continue;
            if(handleOperator()) continue;
            if(handleDelimiter(c)) continue;

            handleInvalid();
        }
    }

    addToken("", line, column(), TokenType::_EOF);
}

bool Lexer::completedTokenization() const noexcept {
    return tokens.size() > 0 && tokens.back().type == TokenType::_EOF;
}

bool Lexer::hasErrors() const noexcept {
    return !lexicalErrors.empty();
}

std::string Lexer::getErrors() const noexcept {
    if(lexicalErrors.empty()){
        return "";
    }

    std::stringstream errors{"Lexical check failed:\n"};
    for(const auto& error : lexicalErrors){
        errors << error << "\n";
    }

    return errors.str();
}

void Lexer::addToken(std::string_view val, size_t lineNumber, size_t col, TokenType type, GeneralTokenType gtype){
    tokens.emplace_back(Token{ val, lineNumber, col, type, gtype });
}

void Lexer::handleError(std::string_view msg, size_t lineNumber, size_t col){
    lexicalErrors.push_back(
        std::format(
            "Line {}, Column {}: {}\n", 
            lineNumber, col, msg
        )
    );
}

bool Lexer::handleNewline(char c) noexcept {
    if(isNewLine(c)){
        advanceLine();
        return true;
    }
    return false;
}

bool Lexer::handleWhitespace(char c) noexcept {
    if(std::isspace(static_cast<unsigned char>(c))){
        advance();
        return true;
    }
    return false;
}

bool Lexer::handleIdentifier(char c){
    if (!isAlpha(c)){
        return false;
    }

    size_t start{ position };
    size_t col{ column() };
    while(isValidIndex() && isIdentifierSequence(getChar(position))){
        advance();
    }

    std::string_view value{ getSequence(start, position - start) };

    if(isKeyword(value)){
        handleKeyword(value, line, col);
    } 
    else {
        tokens.emplace_back(Token{
            value, line, col, TokenType::ID, GeneralTokenType::VALUE
        });
    }

    return true;
}

void Lexer::handleKeyword(std::string_view keyword, size_t lineNumber, size_t col){
    auto type{ keywords.at(keyword) };
    GeneralTokenType gtype {
        types.find(type) != types.end() 
            ? GeneralTokenType::TYPE 
            : GeneralTokenType::OTHER
    };

    tokens.emplace_back(Token{
        keyword, lineNumber, col, type, gtype
    });
}

bool Lexer::handleNumber(char c){
    bool sign{ isSignedNumber() };
    if(!isDigit(c) && !sign){
        return false;
    }

    size_t col{ column() };
    size_t start{ sign ? position++ : position };

    while(isValidIndex() && isDigit(getChar(position))){
        advance();
    }

    if(isValidIndex() && getChar(position) == 'u'){
        advance();
    }

    if(sign && getChar(start - 1) == '-'){
        --start;
    }

    addToken(
        getSequence(start, position - start),
        line,
        col,
        TokenType::LITERAL,
        GeneralTokenType::VALUE
    );

    return true;
}

bool Lexer::handleComment(){
    if(isSingleLineComment()){
        handleSingleLineComment();
        return true;
    }

    if(isMultiLineCommentStart()){
        handleMultiLineComment();
        return true;
    }

    return false;
}

void Lexer::handleSingleLineComment() noexcept {
    advance(2);
    while (isValidIndex() && !isNewLine(getChar(position))){
        advance();
    }
}

void Lexer::handleMultiLineComment(){
    size_t startLine{ line };
    size_t startCol{ column() };

    advance(2);
    while(isValidIndex(1)){
        if(isMultiLineCommentEnd()){
            advance(2);
            return;
        }

        if(!handleNewline(getChar(position))){
            advance();
        }
    }

    handleError(
        std::format(
            "SYNTAX ERROR -> multi-line comment not closed (started at line {}, column {})",
            startLine, startCol
        ),
        line,
        column()
    );
}

bool Lexer::handleOperator(){
    size_t opLen{};

    if(opLen = isRelationalOperator(); opLen > 0){
        handleRelationalOperator(opLen);
        return true;
    }

    if(isAssignOperator()){
        handleAssignOperator();
        return true;
    }

    if(opLen = isBitwiseOperator(); opLen > 0){
        handleBitwiseOperator(opLen);
        return true;
    }

    if (isArithmeticOperator()) {
        handleArithmeticOperator();
        return true;
    }

    return false;
}

void Lexer::handleRelationalOperator(size_t opLen){
    addToken(
        getSequence(position, opLen),
        line, 
        column(), 
        TokenType::RELATIONAL
    );
    advance(opLen);
}

void Lexer::handleAssignOperator(){
    addToken(
        getSequence(position, 1), 
        line, 
        column(), 
        TokenType::ASSIGN
    );
    advance();
}

void Lexer::handleBitwiseOperator(size_t opLen){
    addToken(getSequence(position, opLen),
        line,
        column(),
        TokenType::BITWISE,
        GeneralTokenType::OPERATOR
    );
    advance(opLen);
}

void Lexer::handleArithmeticOperator(){
    addToken(
        getSequence(position, 1), 
        line, 
        column(), 
        TokenType::ARITHMETIC,
        GeneralTokenType::OPERATOR
    );
    advance();
}

bool Lexer::handleDelimiter(char c){
    size_t col{ column() };
    switch(c){
        case '(': 
            addToken(getSequence(position, 1), line, col, TokenType::LPAREN); 
            break;
        case ')': 
            addToken(getSequence(position, 1), line, col, TokenType::RPAREN); 
            break;
        case '{': 
            addToken(getSequence(position, 1), line, col, TokenType::LBRACKET); 
            break;
        case '}': 
            addToken(getSequence(position, 1), line, col, TokenType::RBRACKET); 
            break;
        case ',': 
            addToken(getSequence(position, 1), line, col, TokenType::COMMA); 
            break;
        case ';': 
            addToken(getSequence(position, 1), line, col, TokenType::SEMICOLON); 
            break;
        case ':': 
            addToken(getSequence(position, 1), line, col, TokenType::COLON); 
            break;
        case '#': 
            addToken(getSequence(position, 1), line, col, TokenType::HASH); 
            break;
        default: 
            return false;
    }

    advance();
    return true;
}

void Lexer::handleInvalid(){
    size_t col{ column() };
    handleError(
        std::format("LEXICAL ERROR -> unknown symbol '{}'", 
            getChar(position)
        ), 
        line, 
        col
    );
    addToken(getSequence(position, 1), line, col, TokenType::INVALID);
    advance();
}
