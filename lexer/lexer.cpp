#include "lexer.hpp"

#include <format>
#include <sstream>
#include <cctype>
#include <cassert>

#include "../common/defs/types.hpp"

lex::Lexer::Lexer(const std::vector<std::string>& input) 
    : input{ input }, nextTokenIdx{ 1 } {}

void lex::Lexer::tokenize(){
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

    addToken("", line, column(), syntax::TokenType::_EOF);
}

bool lex::Lexer::completedTokenization() const noexcept {
    return tokens.size() > 0 && tokens.back().type == syntax::TokenType::_EOF;
}

bool lex::Lexer::hasErrors() const noexcept {
    return !lexicalErrors.empty();
}

std::string lex::Lexer::getErrors() const noexcept {
    if(lexicalErrors.empty()){
        return "";
    }

    std::stringstream errors{"Lexical check failed:\n"};
    for(const auto& error : lexicalErrors){
        errors << error << "\n";
    }

    return errors.str();
}

void lex::Lexer::addToken(
    std::string_view val, 
    size_t lineNumber, 
    size_t col, 
    syntax::TokenType type, 
    syntax::GeneralTokenType gtype
){
    tokens.emplace_back(
        syntax::Token{ val, lineNumber, col, type, gtype }
    );
}

void lex::Lexer::handleError(std::string_view msg, size_t lineNumber, size_t col){
    lexicalErrors.push_back(
        std::format(
            "Line {}, Column {}: {}\n", 
            lineNumber, col, msg
        )
    );
}

bool lex::Lexer::handleNewline() noexcept {
    if(isNewLine(getRelativeChar())){
        advanceLine();
        return true;
    }
    return false;
}

bool lex::Lexer::handleWhitespace() noexcept {
    char c{ getRelativeChar() };
    assert(c != '\n');

    if(std::isspace(static_cast<unsigned char>(c))){
        advance();
        return true;
    }
    return false;
}

bool lex::Lexer::handleIdentifier(){
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
        tokens.emplace_back(syntax::Token{
            value, line, col, syntax::TokenType::ID, syntax::GeneralTokenType::VALUE
        });
    }

    return true;
}

bool lex::Lexer::handleKeyword(std::string_view keyword, size_t lineNumber, size_t col){
    if(auto kwdType{ tryGetKeyword(keyword) }){
        auto type{ *kwdType };
        auto gtype{
            tokenTypeToType(type) != Type::NO_TYPE
                ? syntax::GeneralTokenType::TYPE
                : syntax::GeneralTokenType::OTHER
        };

        tokens.emplace_back(syntax::Token{
            keyword, lineNumber, col, type, gtype
        });

        return true;
    }

    return false;
}

bool lex::Lexer::handleNumber(){
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
        syntax::TokenType::LITERAL,
        syntax::GeneralTokenType::VALUE
    );

    return true;
}

bool lex::Lexer::handleComment(){
    if(handleSingleLineComment()){
        return true;
    }

    if(handleMultiLineComment()){
        return true;
    }

    return false;
}

bool lex::Lexer::handleSingleLineComment() noexcept {
    if(!isSingleLineComment()){
        return false;
    }

    advance(2);
    while (isValidIndex() && !isNewLine(getRelativeChar())){
        advance();
    }

    return true;
}

bool lex::Lexer::handleMultiLineComment(){
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

bool lex::Lexer::handleOperator(){
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

bool lex::Lexer::handleRelationalOperator(){
    if(auto opLexeme{ tryGetRelationalOperator() }){
        emitOperator(*opLexeme);
        return true;
    }
    return false;
}

bool lex::Lexer::handleLogicalOperator(){
    if(auto opLexeme{ tryGetLogicalOperator() }){
        emitOperator(*opLexeme);
        return true;
    }
    return false;
}

bool lex::Lexer::handleAssignOperator(){
    if(auto opLexeme{ tryGetAssignOperator() }){
        emitOperator(*opLexeme);
        return true;
    }
    return false;
}

bool lex::Lexer::handleBitwiseOperator(){
    if(auto opLexeme{ tryGetBitwiseOperator() }){
        emitOperator(*opLexeme);
        return true;
    }
    return false;
}

bool lex::Lexer::handleArithmeticOperator(){
    if(auto opLexeme{ tryGetArithmeticOperator() }){
        emitOperator(*opLexeme);
        return true;
    }

    return false;
}

void lex::Lexer::emitOperator(lex::Lexeme operatorLexeme, syntax::GeneralTokenType gtype){
    addToken(getRelativeSequence(
        operatorLexeme.length), 
        line, 
        column(), 
        operatorLexeme.type, 
        gtype
    );
    advance(operatorLexeme.length);
}

bool lex::Lexer::handlePunctuation(){
    size_t col{ column() };
    switch(getRelativeChar()){
        case '(': 
            addToken(getRelativeSequence(1), line, col, syntax::TokenType::LPAREN); 
            break;

        case ')': 
            addToken(getRelativeSequence(1), line, col, syntax::TokenType::RPAREN); 
            break;

        case '{': 
            addToken(getRelativeSequence(1), line, col, syntax::TokenType::LBRACE); 
            break;

        case '}': 
            addToken(getRelativeSequence(1), line, col, syntax::TokenType::RBRACE); 
            break;

        case ',': 
            addToken(getRelativeSequence(1), line, col, syntax::TokenType::COMMA); 
            break;

        case ';': 
            addToken(getRelativeSequence(1), line, col, syntax::TokenType::SEMICOLON); 
            break;

        case ':': 
            addToken(getRelativeSequence(1), line, col, syntax::TokenType::COLON); 
            break;

        case '#': 
            addToken(getRelativeSequence(1), line, col, syntax::TokenType::HASH); 
            break;

        default: 
            return false;
    }

    advance();
    return true;
}

void lex::Lexer::handleInvalid(){
    size_t col{ column() };
    handleError(
        std::format("LEXICAL ERROR -> unknown symbol '{}'", 
            getRelativeChar()
        ), 
        line, 
        col
    );
    addToken(getRelativeSequence(1), line, col, syntax::TokenType::INVALID);
    advance();
}
