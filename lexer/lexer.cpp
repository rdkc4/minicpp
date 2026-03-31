#include "lexer.hpp"

#include <format>
#include <sstream>

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

bool Lexer::hasErrors() const noexcept{
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

void Lexer::next() noexcept {
    ++nextTokenIdx;
}

const Token& Lexer::peek() const noexcept {
    return nextTokenIdx >= tokens.size()
        ? tokens.back()
        : tokens[nextTokenIdx];
}

const Token& Lexer::current() const noexcept {
    return nextTokenIdx >= tokens.size()
        ? tokens.back()
        : tokens[nextTokenIdx - 1];
}

void Lexer::addToken(std::string_view val, size_t lineNumber, size_t col, TokenType type, GeneralTokenType gtype){
    tokens.emplace_back(Token{ val, lineNumber, col, type, gtype });
}

void Lexer::handleError(std::string_view msg, size_t lineNumber, size_t col) {
    lexicalErrors.push_back(
        std::format(
            "Line {}, Column {}: {}\n", 
            lineNumber, col, msg
        )
    );
}

bool Lexer::handleNewline(char c) noexcept {
    if (isNewLine(c)) {
        advanceLine();
        return true;
    }
    return false;
}

bool Lexer::handleWhitespace(char c) noexcept {
    if (std::isspace(static_cast<unsigned char>(c))) {
        advance();
        return true;
    }
    return false;
}

bool Lexer::handleIdentifier(char c){
    if (!std::isalpha(static_cast<unsigned char>(c))) return false;

    size_t start{ position };
    size_t col{ column() };
    while (isValidIndex() && isIdentifierSequence(getChar(position))) {
        advance();
    }

    std::string value{ getSequence(start, position - start) };

    if (isKeyword(value)) {
        auto type{ keywords.at(value) };
        GeneralTokenType gtype {
            types.contains(type) 
                ? GeneralTokenType::TYPE 
                : GeneralTokenType::OTHER
        };

        tokens.emplace_back(Token{
            value, line, col, type, gtype
        });

    } else {
        tokens.emplace_back(Token{
            value, line, col, TokenType::_ID, GeneralTokenType::VALUE
        });
    }

    return true;
}

bool Lexer::handleNumber(char c) {
    bool sign{ isSignedLiteral() };
    if(!isDigit(c) && !sign){
        return false;
    }

    size_t col{ column() };
    size_t start{ sign ? position++ : position };

    while(isValidIndex() && isDigit(getChar(position))){
        advance();
    }

    if(isValidIndex() && getChar(position) == 'u') {
        advance();
    }

    if(sign && getChar(start - 1) == '-') {
        --start;
    }

    addToken(
        getSequence(start, position - start),
        line,
        col,
        TokenType::_LITERAL,
        GeneralTokenType::VALUE
    );

    return true;
}

bool Lexer::handleComment(){
    if (isSingleLineComment()){
        advance(2);
        while (isValidIndex() && !isNewLine(getChar(position))){
            advance();
        }
        return true;
    }

    if(isMultiLineCommentStart()){
        size_t startLine{ line };
        size_t startCol{ column() };

        advance(2);
        while(isValidIndex(1)) {
            if (isMultiLineCommentEnd()) {
                advance(2);
                return true;
            }

            if (isNewLine(getChar(position))){ 
                advanceLine();
            }
            else {
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

    return false;
}

bool Lexer::handleOperator(){
    size_t start{ position };
    size_t col{ column() };

    size_t relopLen{ isRelOperator() };
    if(relopLen > 0){
        advance(relopLen);

        addToken(
            getSequence(start, position - start),
            line, 
            col, 
            TokenType::_RELOP
        );
        return true;
    }

    if(isAssignOperator()){
        addToken(
            getSequence(start, 1), 
            line, 
            col, 
            TokenType::_ASSIGN
        );
        advance();
        return true;
    }

    size_t bitopLen{ isBitwiseOperator() };
    if(bitopLen > 0){
        advance(bitopLen);

        addToken(getSequence(start, position - start),
            line,
            col,
            TokenType::_BITWISE,
            GeneralTokenType::OPERATOR
        );
        return true;
    }

    if (isAritOperator()) {
        addToken(
            getSequence(start, 1), 
            line, 
            col, 
            TokenType::_AROP,
            GeneralTokenType::OPERATOR
        );
        advance();

        return true;
    }

    return false;
}

bool Lexer::handleDelimiter(char c){
    size_t col{ column() };
    switch(c){
        case '(': 
            addToken(getSequence(position, 1), line, col, TokenType::_LPAREN); 
            break;
        case ')': 
            addToken(getSequence(position, 1), line, col, TokenType::_RPAREN); 
            break;
        case '{': 
            addToken(getSequence(position, 1), line, col, TokenType::_LBRACKET); 
            break;
        case '}': 
            addToken(getSequence(position, 1), line, col, TokenType::_RBRACKET); 
            break;
        case ',': 
            addToken(getSequence(position, 1), line, col, TokenType::_COMMA); 
            break;
        case ';': 
            addToken(getSequence(position, 1), line, col, TokenType::_SEMICOLON); 
            break;
        case ':': 
            addToken(getSequence(position, 1), line, col, TokenType::_COLON); 
            break;
        case '#': 
            addToken(getSequence(position, 1), line, col, TokenType::_HASH); 
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
    addToken(getSequence(position, 1), line, col, TokenType::_INVALID);
    advance();
}
