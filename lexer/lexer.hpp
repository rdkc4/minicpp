#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <string_view>
#include <optional>

#include "../common/token/token.hpp"
#include "defs/lexeme.hpp"

/**
 * @class Lexer
 * @brief Lexical analysis of the input
*/
class Lexer {
public:
    /** 
     * @brief Creates the instance of the lexer
     * @param input - source code that needs to be tokenized
    */
    Lexer(const std::vector<std::string>& input);

    /**
     * @brief translating input into a sequence of tokens
    */
    void tokenize();

    /**
     * @brief increases nextTokenIdx
    */
    inline void next() noexcept {
        ++nextTokenIdx;
    }

    /**
     * @brief gets the token that is next in line for parsing
     * @returns const reference of the next token
    */
    inline const syntax::Token& peek() const noexcept {
        return nextTokenIdx >= tokens.size() ? tokens.back() : tokens[nextTokenIdx];
    }

    /**
     * @brief gets the token that should be parsed at the moment
     * @returns const reference of the current token
    */
    inline const syntax::Token& current() const noexcept {
        return nextTokenIdx >= tokens.size() ? tokens.back() : tokens[nextTokenIdx - 1];
    }

    /** 
     * @brief asserts that the tokenization has been completed
     * @returns true if input has been tokenized, false otherwise
    */
    bool completedTokenization() const noexcept;

    /** 
     * @brief checks if any lexical error has been caught
     * @returns false if there are no lexical errors, true otherwise
    */
    bool hasErrors() const noexcept;

    /**
     * @brief getter for the lexical errors
     * @returns lexical errors merged into a string
    */
    std::string getErrors() const noexcept;

private:
    /// code that needs to be tokenized
    const std::vector<std::string>& input;
    
    /// index of the current file
    size_t fileIndex;

    // length of the current file
    size_t fileLength;

    /// current position in the input
    size_t position;

    /// current line in the input
    size_t line;

    /// position at which previous line ended, used to determine the column of a token: column = position - prevLineLen
    size_t prevLineLen;

    /// index of a token that should be processed by the parser next
    size_t nextTokenIdx;
    
    /// vector of lexical errors
    std::vector<std::string> lexicalErrors;

protected:
    /// vector of tokens generated from the input
    std::vector<syntax::Token> tokens;

private:

    /**
     * @brief resets the lexer state for currently processed file
    */
    inline void resetState() noexcept {
        position = 0;
        line = 1;
        prevLineLen = 0;
        fileLength = input[fileIndex].length();
    }

    /**
     * @brief getter for the char relative to the current position
     * @param offset - offset from current position, defaults to 0
     * @returns char relative to current position
    */
    inline char getRelativeChar(size_t offset = 0) const noexcept {
        return input[fileIndex][position + offset];
    }

    /**
     * @brief getter for the char at the absolute position
     * @param index - index of the character in a file
     * @returns char at the absolute position
    */
    inline char getChar(size_t index) const noexcept {
        return input[fileIndex][index];
    }

    /**
     * @brief getter for the sequence of characters
     * @param len - length of the sequence
     * @param offset - offset from the current position, defaults to 0
     * @returns sequence of characters at current position, with provided length
    */
    inline std::string_view getRelativeSequence(size_t len, size_t offset = 0) const noexcept {
        return std::string_view{&input[fileIndex][position + offset], len};
    }

    /**
     * @brief getter for the sequence of characters
     * @param startIndex - start index of the sequence
     * @param len - length of the sequence
     * @returns sequence of characters at given index, with provided length
    */
    inline std::string_view getSequence(size_t startIndex, size_t len) const noexcept {
        return std::string_view{&input[fileIndex][startIndex], len};
    }

    /**
     * @brief getter for the length of the sequence
     * @param start - position where sequence starts
     * @returns length of the sequence
    */
    inline size_t sequenceLength(size_t start) const noexcept {
        return position - start;
    }

    /**
     * @brief getter for the current column
     * @returns number of the current column
    */
    inline size_t column() const noexcept {
        return position - prevLineLen;
    }

    /**
     * @brief increases the current position by a specified number of characters
     * @param n - the number of characters by which to increment the position, defaults to 1
    */
    inline void advance(size_t n = 1) noexcept {
        position += n;
    }

    /**
     * @brief increments position, increments lineNumber and sets prevLineLen to position
    */
    inline void advanceLine() noexcept {
        ++position;
        ++line;
        prevLineLen = position;
    }

    /**
     * @brief creates a new token
     * @param val - value of the token
     * @param lineNumber - line where token starts
     * @param column - column where token starts
     * @param type - type of the token
     * @param gtype - general type of the token, defaults to GeneralTokenType::OTHER
    */
    void addToken(
        std::string_view val, 
        size_t lineNumber, 
        size_t col, 
        syntax::TokenType type, 
        syntax::GeneralTokenType gtype = syntax::GeneralTokenType::OTHER
    );

    /**
     * @brief creates a new error message
     * @param msg - error message
     * @param lineNumber - line where error occured
     * @param col - column where error occured
    */
    void handleError(std::string_view msg, size_t lineNumber, size_t col);

    /**
     * @brief handles new lines, updates positions
     * @returns true if character is a new line, false otherwise
    */
    bool handleNewline() noexcept;

    /**
     * @brief handles whitespaces, updates position
     * @returns true if character is a whitespace, false otherwise
    */
    bool handleWhitespace() noexcept;

    /**
     * @brief handles identifiers, updates position
     * @returns true if sequence is an identifier, false otherwise
    */
    bool handleIdentifier();

    /**
     * @brief handles keywords
     * @param keyword - identifier of the keyword
     * @param lineNumber - line where keyword appears
     * @param col - column where keyword starts
     * @returns true if sequence is a keyword, false otherwise
    */
    bool handleKeyword(std::string_view keyword, size_t lineNumber, size_t col);

    /**
     * @brief handles numbers, updates position
     * @returns true if sequence is a number, false otherwise
    */
    bool handleNumber();

    /**
     * @brief handles comments, updates position
     * @returns true if sequence is a comment, false otherwise
    */
    bool handleComment();

    /**
     * @brief handles single line comment
     * @returns true if sequence is a single line comment, false otherwise
    */
    bool handleSingleLineComment() noexcept;

    /**
     * @brief handles multi line comment
     * @returns true if sequence is a multi line comment, false otherwise
    */
    bool handleMultiLineComment();

    /**
     * @brief handles operators, updates position
     * @returns true if sequence is an operator, false otherwise
    */
    bool handleOperator();

    /**
     * @brief handles relational operator
     * @returns true if sequence is a relational operator, false otherwise
    */
    bool handleRelationalOperator();

    /**
     * @brief handles logical operator
     * @returns true if sequence is a logical operator, false otherwise
    */    
    bool handleLogicalOperator();

    /**
     * @brief handles assign operator
     * @returns true if sequence is an assign operator, false otherwise
    */
    bool handleAssignOperator();

    /**
     * @brief handles bitwise operator
     * @returns true if a sequence is a bitwise operator, false otherwise
    */
    bool handleBitwiseOperator();

    /**
     * @brief handles arithmetic operator
     * @returns true if a sequence is an arithmetic operator, false otherwise
    */
    bool handleArithmeticOperator();

    /**
     * @brief generates the operator token
     * @param operatorLexeme - type and length of the operator
     * @param gtype - general token type of the token
    */
    void emitOperator(
        Lexeme operatorLexeme, 
        syntax::GeneralTokenType gtype = syntax::GeneralTokenType::OPERATOR
    );

    /**
     * @brief handles punctuations, updates position
     * @param c - current character
     * @returns true if character is a punctuation, false otherwise
    */
    bool handlePunctuation();

    /**
     * @brief handles invalid characters, updates position
    */
    void handleInvalid();

    /** 
     * @brief checks if the current sequence of characters is a keyword
     * @param value - string representing keyword
     * @returns token type of the keyword if sequence is keyword, nullopt otherwise
    */
    inline std::optional<syntax::TokenType> tryGetKeyword(std::string_view value) const noexcept {
        const size_t length{ value.size() };
        if(length < 2 || length > 8){
            return std::nullopt;
        }

        switch(length){
            case 2:
                if(value == "if") return syntax::TokenType::IF;
                if(value == "do") return syntax::TokenType::DO;
                break;

            case 3:
                if(value == "for") return syntax::TokenType::FOR;
                if(value == "int") return syntax::TokenType::INT;
                break;

            case 4:
                if(value == "else") return syntax::TokenType::ELSE;
                if(value == "case") return syntax::TokenType::CASE;
                if(value == "void") return syntax::TokenType::VOID;
                if(value == "auto") return syntax::TokenType::AUTO;
                break;

            case 5:
                if(value == "while") return syntax::TokenType::WHILE;
                if(value == "break") return syntax::TokenType::BREAK;
                break;

            case 6:
                if(value == "return") return syntax::TokenType::RETURN;
                if(value == "switch") return syntax::TokenType::SWITCH;
                break;

            case 7:
                if(value == "default") return syntax::TokenType::DEFAULT;
                if(value == "include") return syntax::TokenType::INCLUDE;
                break;

            case 8:
                if(value == "unsigned") return syntax::TokenType::UNSIGNED;
                break;
        }

        return std::nullopt;
    }

    /**
     * @brief checks if type can precede signed literal
     * @param type - type of the token
     * @returns true if type can precede signed literal, false otherwise
    */
    inline bool canPrecedeSignedLiteral(syntax::TokenType type) const noexcept {
        switch(type){
            case syntax::TokenType::LITERAL:
            case syntax::TokenType::ID:
            case syntax::TokenType::RPAREN:
                return false;
            
            default:
                return true;
        }
    }

    /** 
     * @brief checks if the current sequence of characters is a signed literal
     * @returns true if sequence matches signed literal, false otherwise
    */
    inline bool isSignedNumber() const noexcept {
        char c{ getChar(position) };

        if(c != '+' && c != '-') return false;
        if(!isValidIndex(1)) return false;
        if(!isDigit(getChar(position + 1))) return false;

        return !tokens.empty() && canPrecedeSignedLiteral(tokens.back().type);
    }

    /** 
     * @brief checks if current sequence of characters is an assignment operator
     * @returns lexeme of assign operator if sequence matches assign operator, nullopt otherwise
    */
    inline std::optional<Lexeme> tryGetAssignOperator() const noexcept {
        if(getChar(position) == '=' && (!isValidIndex(1) || getChar(position + 1) != '=')){
            return Lexeme{.type = syntax::TokenType::ASSIGN, .length = 1};
        }
        return std::nullopt;
    }

    /** 
     * @brief checks if the current sequence of characters is an arithmetic operator
     * @details arithmetic operators: +, -, *, /
     * @returns lexeme of arithmetic operator if sequence matches arithmetic operator, nullopt otherwise
    */
    inline std::optional<Lexeme> tryGetArithmeticOperator() const noexcept {
        switch(getChar(position)){
            case '+':
                return Lexeme{.type = syntax::TokenType::PLUS, .length = 1};
            case '-':
                return Lexeme{.type = syntax::TokenType::MINUS, .length = 1};
            case '*':
                return Lexeme{.type = syntax::TokenType::ASTERISK, .length = 1};
            case '/':
                return Lexeme{.type = syntax::TokenType::SLASH, .length = 1};

            default:
                return std::nullopt;
        }
    }

    /** 
     * @brief checks if the current sequence of characters is a bitwise operator
     * @details bitwise operators: <<, >>, &, |, ^
     * @returns lexeme of the bitwise operator when sequence is a bitwise operator, nullopt otherwise
    */
    inline std::optional<Lexeme> tryGetBitwiseOperator() const noexcept {
        char c1{ getChar(position) };
        char c2{ isValidIndex(1) ? getChar(position + 1) : '\0'};

        switch(c1){
            case '<':
                if(c2 == '<'){
                    return Lexeme{.type = syntax::TokenType::LSHIFT, .length = 2};
                }
                break;

            case '>':
                if(c2 == '>'){
                    return Lexeme{.type = syntax::TokenType::RSHIFT, .length = 2};
                }
                break;

            case '&':
                if(c2 != '&'){
                    return Lexeme{.type = syntax::TokenType::AMPERSEND, .length = 1};
                }
                break;

            case '|':
                if(c2 != '|'){
                    return Lexeme{.type = syntax::TokenType::PIPE, .length = 1};
                }
                break;

            case '^':
                return Lexeme{.type = syntax::TokenType::CARET, .length = 1};
        }

        return std::nullopt;
    }

    /** 
     * @brief checks if the current sequence of characters is a logical operator
     * @details logical operators: &&, ||
     * @returns lexeme of the logical operator when sequence is a logical operator, nullopt otherwise
    */
    inline std::optional<Lexeme> tryGetLogicalOperator() const noexcept {
        char c1{ getChar(position) };
        char c2{ isValidIndex(1) ? getChar(position + 1) : '\0' };

        switch(c1){
            case '&':
                if(c2 == '&'){
                    return Lexeme{.type = syntax::TokenType::LOGICAL_AND, .length = 2};
                }
                break;

            case '|':
                if(c2 == '|'){
                    return Lexeme{.type = syntax::TokenType::LOGICAL_OR, .length = 2};
                }
                break;
        }

        return std::nullopt;
    }

    /** 
     * @brief checks if the current sequence of characters is a relational operator
     * @details relational operators: <, >, <=, >=, ==, !=
     * @returns lexeme of the relational operator if sequence matches relational operatr, nullopt otherwise
    */
    inline std::optional<Lexeme> tryGetRelationalOperator() const noexcept {
        char c1{ getChar(position) };
        char c2{ isValidIndex(1) ? getChar(position + 1) : '\0'};
        
        switch(c1){
            case '<':
                if(c2 == '='){
                    return Lexeme{.type = syntax::TokenType::LESS_EQ, .length = 2};
                }
                else if(c2 != '<'){
                    return Lexeme{.type = syntax::TokenType::LESS, .length = 1};
                }
                break;

            case '>':
                if(c2 == '='){
                    return Lexeme{.type = syntax::TokenType::GREATER_EQ, .length = 2};
                }
                else if(c2 != '>'){
                    return Lexeme{.type = syntax::TokenType::GREATER, .length = 1};
                }
                break;
            
            case '=':
                if(c2 == '='){
                    return Lexeme{.type = syntax::TokenType::EQUAL, .length = 2};
                }
                break;

            case '!':
                if(c2 == '='){
                    return Lexeme{.type = syntax::TokenType::NOT_EQ, .length = 2};
                }
                break;
        }

        return std::nullopt;
    }

    /**
     * @brief checks if the position is valid
     * @param offset - number of characters after current position, defaults to 0
     * @returns true of position is valid, false otherwise
    */
    inline bool isValidIndex(size_t offset = 0) const noexcept {
        return position + offset < fileLength;
    }

    /**
     * @brief checks if the char is adequate for an identifier
     * @param c - current character
     * @returns true if character is a part of id sequence, false otherwise
    */
    inline constexpr bool isIdentifierSequence(char c) const noexcept {
        return isAlpha(c) || isDigit(c) || c == '_';
    }

    /**
     * @brief checks if character is a letter [a-z][A-Z]
     * @param c - current character
     * @returns true if character is a letter, false otherwise
    */
    inline constexpr bool isAlpha(char c) const noexcept {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    /**
     * @brief checks if character is a digit
     * @param c - current character
     * @returns true if character is a digit, false otherwise
    */
    inline constexpr bool isDigit(char c) const noexcept {
        return c >= '0' && c <= '9';
    }

    /**
     * @brief checks if the sequence is a single line comment
     * @returns true if sequence is a single line comment, false otherwise
    */
    inline bool isSingleLineComment() const noexcept {
        return isValidIndex(1) && getChar(position) == '/' && getChar(position + 1) == '/';
    }

    /**
     * @brief checks if the sequence is the opening of the multi line comment
     * @returns true if the sequence is the opening of the multi line comment, false otherwise
    */
    inline bool isMultiLineCommentStart() const noexcept {
        return isValidIndex(1) && getChar(position) == '/' && getChar(position + 1) == '*';
    } 

    /**
     * @brief checks if the sequence is the closing of the multi line comment
     * @returns true if the sequence is the closing of the multi line comment, false otherwise
    */
    inline bool isMultiLineCommentEnd() const noexcept {
        return isValidIndex(1) && getChar(position) == '*' && getChar(position + 1) == '/';
    }

    /**
     * @brief checks if the character is a new line
     * @param c - current character
     * @returns true if character is a new line, false otherwise
    */
    inline bool isNewLine(char c) const noexcept {
        return c == '\n';
    };

};

#endif
