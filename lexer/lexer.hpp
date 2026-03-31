#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <string_view>
#include <cctype>

#include "../common/token/token.hpp"
#include "defs/lexer_defs.hpp"

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
    void next() noexcept;

    /**
     * @brief gets the token that is next in line for parsing
     * @returns const reference of the next token
    */
    const Token& peek() const noexcept;

    /**
     * @brief gets the token that should be parsed at the moment
     * @returns const reference of the current token
    */
    const Token& current() const noexcept;

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
    std::vector<Token> tokens;

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
     * @brief getter for the char at the current position
     * @param index - index of the character in a file
     * @returns char at the current position
    */
    inline char getChar(size_t index) const noexcept {
        return input[fileIndex][index];
    }

    /**
     * @brief getter for the sequence of characters
     * @param startIndex - start index of the sequence
     * @param len - length of the sequence
     * @returns sequence of characters at current index, with provided length
    */
    inline std::string_view getSequence(size_t startIndex, size_t len) const noexcept {
        return std::string_view{&input[fileIndex][startIndex], len};
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
    void addToken(std::string_view val, size_t lineNumber, size_t col, TokenType type, GeneralTokenType gtype = GeneralTokenType::OTHER);

    /**
     * @brief creates a new error message
     * @param msg - error message
     * @param lineNumber - line where error occured
     * @param col - column where error occured
    */
    void handleError(std::string_view msg, size_t lineNumber, size_t col);

    /**
     * @brief handles new lines, updates positions
     * @param c - current character
     * @returns true if character is a new line, false otherwise
    */
    bool handleNewline(char c) noexcept;

    /**
     * @brief handles whitespaces, updates position
     * @param c - current character
     * @returns true if character is a whitespace, false otherwise
    */
    bool handleWhitespace(char c) noexcept;

    /**
     * @brief handles identifiers, updates position
     * @param c - current character
     * @returns true if sequence is an identifier, false otherwise
    */
    bool handleIdentifier(char c);

    /**
     * @brief handles numbers, updates position
     * @param c - current character
     * @returns true if sequence is a number, false otherwise
    */
    bool handleNumber(char c);

    /**
     * @brief handles comments, updates position
     * @returns true if sequence is a comment, false otherwise
    */
    bool handleComment();

    /**
     * @brief handles operators, updates position
     * @returns true if sequence is an operator, false otherwise
    */
    bool handleOperator();

    /**
     * @brief handles delimiters, updates position
     * @param c - current character
     * @returns true if character is a delimiter, false otherwise
    */
    bool handleDelimiter(char c);

    /**
     * @brief handles invalid characters, updates position
    */
    void handleInvalid();

    /** 
     * @brief checks if the current sequence of characters is a keyword
     * @param value - const string representing id or keyword
     * @returns true if sequence matches keyword, false otherwise
    */
    inline bool isKeyword(const std::string& value) const noexcept {
        return keywords.find(value) != keywords.end();
    }

    inline bool canPrecedeSignedLiteral(TokenType type) const noexcept {
        switch(type){
            case TokenType::_LITERAL:
            case TokenType::_ID:
            case TokenType::_RPAREN:
                return false;
            
            default:
                return true;
        }
    }

    /** 
     * @brief checks if the current sequence of characters is a signed literal
     * @returns true if sequence matches signed literal, false otherwise
    */
    inline bool isSignedLiteral() const noexcept {
        char c{ getChar(position) };

        if(c != '+' && c != '-') return false;
        if(!isValidIndex(1)) return false;
        if(!isDigit(getChar(position + 1))) return false;

        return !tokens.empty() && canPrecedeSignedLiteral(tokens.back().type);
    }

    /** 
     * @brief checks if current sequence of characters is an assignment operator
     * @returns true if sequence matches assign operator, false otherwise
    */
    bool isAssignOperator() const noexcept {
        return getChar(position) == '=' && (!isValidIndex(1) || getChar(position + 1) != '=');
    }

    /** 
     * @brief checks if the current sequence of characters is an arithmetic operator
     * @details arithmetic operators: +, -, *, /
     * @returns true if sequence matches arithmetic operator, false otherwise
    */
    inline bool isAritOperator() const noexcept {
        switch(getChar(position)){
            case '+':
            case '-':
            case '*':
            case '/':
                return true;

            default:
                return false;
        }
    }

    /** 
     * @brief checks if the current sequence of characters is a bitwise operator
     * @details bitwise operators: <<, >>, &, |, ^
     * @returns length of the bitwise operator, 0 when it is not a bitwise operator
    */
    inline size_t isBitwiseOperator() const noexcept {
        char c1{ getChar(position) };
        char c2{ isValidIndex(1) ? getChar(position + 1) : '\0'};

        switch(c1){
            case '<':
                if(c2 == '<'){
                    return 2;
                }
                return 0;

            case '>':
                if(c2 == '>'){
                    return 2;
                }
                return 0;

            case '&':
            case '|':
            case '^':
                return 1;
            
            default:
                return 0;
        }
    }

    /** 
     * @brief checks if the current sequence of characters is a relational operator
     * @details relational operators: <, >, <=, >=, ==, !=
     * @returns length of the relational operator, 0 when it is not the relational operator
    */
    inline size_t isRelOperator() const noexcept {
        char c1{ getChar(position) };
        char c2{ isValidIndex(1) ? getChar(position + 1) : '\0'};
        
        switch(c1){
            case '<':
            case '>':
                if(c2 == '='){
                    return 2;
                }
                else if(c2 != '<' && c2 != '>'){
                    return 1;
                }
                return 0;
            
            case '=':
            case '!':
                if(c2 == '='){
                    return 2;
                }
                return 0;

            default:
                return 0;
        }
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
    inline bool isIdentifierSequence(char c) const noexcept {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
    }

    /**
     * @brief checks if character is a digit
     * @param c - current character
     * @returns true if character is a digit, false otherwise
    */
    inline bool isDigit(char c) const noexcept {
        return std::isdigit(static_cast<unsigned char>(c));
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
