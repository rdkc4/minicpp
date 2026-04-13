#include "lexer.hpp"

#include <format>
#include <sstream>
#include <cctype>
#include <cassert>

Lexer::Lexer(const std::vector<std::string>& input) 
    : input{ input }, nextTokenIdx{ 1 } {}

void Lexer::tokenize(){
    const size_t fileCount{ input.size() };

    for (fileIndex = 0; fileIndex < fileCount; ++fileIndex) {
        resetState();

        while (position < fileLength) {

            if(handleNewline()) continue;
            if(handleWhitespace()) continue;
            if(handleIdentifier()) continue;
            if(handleNumber()) continue;
            if(handleComment()) continue;
            if(handleOperator()) continue;
            if(handlePunctuation()) continue;

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

bool Lexer::handleNewline() noexcept {
    if(isNewLine(getRelativeChar())){
        advanceLine();
        return true;
    }
    return false;
}

bool Lexer::handleWhitespace() noexcept {
    char c{ getRelativeChar() };
    assert(c != '\n');

    if(std::isspace(static_cast<unsigned char>(c))){
        advance();
        return true;
    }
    return false;
}

bool Lexer::handleIdentifier(){
    if (!isAlpha(getRelativeChar())){
        return false;
    }

    size_t start{ position };
    size_t col{ column() };
    while(isValidIndex() && isIdentifierSequence(getRelativeChar())){
        advance();
    }

    std::string_view value{ getSequence(start, sequenceLength(start)) };

    if(!handleKeyword(value, line, col)){
        tokens.emplace_back(Token{
            value, line, col, TokenType::ID, GeneralTokenType::VALUE
        });
    }

    return true;
}

bool Lexer::handleKeyword(std::string_view keyword, size_t lineNumber, size_t col){
    if(!isKeyword(keyword)){
        return false;
    }

    auto type{ keywords.at(keyword) };
    GeneralTokenType gtype {
        types.find(type) != types.end() 
            ? GeneralTokenType::TYPE 
            : GeneralTokenType::OTHER
    };

    tokens.emplace_back(Token{
        keyword, lineNumber, col, type, gtype
    });

    return true;
}

bool Lexer::handleNumber(){
    bool sign{ isSignedNumber() };
    if(!isDigit(getRelativeChar()) && !sign){
        return false;
    }

    size_t col{ column() };
    size_t start{ position };

    if(sign){
        advance();
    }

    while(isValidIndex() && isDigit(getRelativeChar())){
        advance();
    }

    if(isValidIndex() && getRelativeChar() == 'u'){
        advance();
    }

    if(sign && getChar(start) == '+'){
        ++start;
    }

    addToken(
        getSequence(start, sequenceLength(start)),
        line,
        col,
        TokenType::LITERAL,
        GeneralTokenType::VALUE
    );

    return true;
}

bool Lexer::handleComment(){
    if(handleSingleLineComment()){
        return true;
    }

    if(handleMultiLineComment()){
        return true;
    }

    return false;
}

bool Lexer::handleSingleLineComment() noexcept {
    if(!isSingleLineComment()){
        return false;
    }

    advance(2);
    while (isValidIndex() && !isNewLine(getRelativeChar())){
        advance();
    }

    return true;
}

bool Lexer::handleMultiLineComment(){
    if(!isMultiLineCommentStart()){
        return false;
    }

    size_t startLine{ line };
    size_t startCol{ column() };

    advance(2);
    while(isValidIndex(1)){
        if(isMultiLineCommentEnd()){
            advance(2);
            return true;
        }

        if(!handleNewline()){
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

    return true;
}

bool Lexer::handleOperator(){
    if(handleRelationalOperator()){
        return true;
    }

    if(handleLogicalOperator()){
        return true;
    }

    if(handleAssignOperator()){
        return true;
    }

    if(handleBitwiseOperator()){
        return true;
    }

    if(handleArithmeticOperator()){
        return true;
    }

    return false;
}

bool Lexer::handleRelationalOperator(){
    Lexeme opLexeme{ isRelationalOperator() };
    if(opLexeme.length == 0){
        return false;
    }
    emitOperator(opLexeme.type, opLexeme.length);
    return true;
}

bool Lexer::handleLogicalOperator(){
    Lexeme opLexeme{ isLogicalOperator() };
    if(opLexeme.length == 0){
        return false;
    }
    emitOperator(opLexeme.type, opLexeme.length);
    return true;
}

bool Lexer::handleAssignOperator(){
    Lexeme opLexeme{ isAssignOperator() };
    if(opLexeme.length == 0){
        return false;
    }
    emitOperator(opLexeme.type, opLexeme.length);
    return true;
}

bool Lexer::handleBitwiseOperator(){
    Lexeme opLexeme{ isBitwiseOperator() };
    if(opLexeme.length == 0){
        return false;
    }
    emitOperator(opLexeme.type, opLexeme.length);
    return true;
}

bool Lexer::handleArithmeticOperator(){
    Lexeme opLexeme{ isArithmeticOperator() };
    if(opLexeme.length == 0){
        return false;
    }
    emitOperator(opLexeme.type, opLexeme.length);
    return true;
}

void Lexer::emitOperator(TokenType type, size_t length, GeneralTokenType gtype){
    addToken(getRelativeSequence(length), line, column(), type, gtype);
    advance(length);
}

bool Lexer::handlePunctuation(){
    size_t col{ column() };
    switch(getRelativeChar()){
        case '(': 
            addToken(getRelativeSequence(1), line, col, TokenType::LPAREN); 
            break;
        case ')': 
            addToken(getRelativeSequence(1), line, col, TokenType::RPAREN); 
            break;
        case '{': 
            addToken(getRelativeSequence(1), line, col, TokenType::LBRACE); 
            break;
        case '}': 
            addToken(getRelativeSequence(1), line, col, TokenType::RBRACE); 
            break;
        case ',': 
            addToken(getRelativeSequence(1), line, col, TokenType::COMMA); 
            break;
        case ';': 
            addToken(getRelativeSequence(1), line, col, TokenType::SEMICOLON); 
            break;
        case ':': 
            addToken(getRelativeSequence(1), line, col, TokenType::COLON); 
            break;
        case '#': 
            addToken(getRelativeSequence(1), line, col, TokenType::HASH); 
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
            getRelativeChar()
        ), 
        line, 
        col
    );
    addToken(getRelativeSequence(1), line, col, TokenType::INVALID);
    advance();
}
