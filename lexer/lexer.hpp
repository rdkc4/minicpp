#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

#include "../common/token/token.hpp"

/**
 * @class Lexer
 * @brief Lexical analysis of the input
*/
class Lexer{
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
    bool hasLexicalErrors() const noexcept;

    /**
        * @brief getter for the lexical errors
        * @returns lexical errors merged into a string
    */
    std::string getLexicalErrors() const noexcept;

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
    size_t lineNumber;

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
        * @brief increases position for 1
    */
    void updatePosition() noexcept;

    /**
        * @brief increases the current position by a specified number of characters.
        * @param n - the number of characters by which to increment the position.
    */
    void updatePosition(size_t n) noexcept;

    /**
        * @brief increases position for 1
        *
        * increases lineNumber for 1
        *
        * sets prevLineLen to position
    */
    void updateLine() noexcept;

    /** 
        * @brief pushes keyword or id to tokens
    */
    void pushID();

    /** 
        * @brief pushes literal to tokens
        * @param sign - flag that tells if literal is signed or not, default false
    */
    void pushLiteral(bool sign = false);

    /** 
        * @brief pushes assignment to tokens
    */
    void pushAssignOperator();

    /** 
        * @brief pushes bitwise operator to tokens
    */
    void pushBitwiseOperator();

    /** 
        * @brief pushes arithmetic operator to tokens
    */
    void pushAritOperator();

    /** 
        * @brief pushes relational operator to tokens
    */
    void pushRelOperator();

    /** 
        * @brief checks if current sequence of characters is an assignment operator
        * @returns true if sequence matches assign operator, false otherwise
    */
    bool isAssignOperator() const noexcept;

    /** 
        * @brief checks if the current sequence of characters is a keyword
        * @param value - const string representing id or keyword
        * @returns true if sequence matches keyword, false otherwise
    */
    bool isKeyword(const std::string& value) const noexcept;

    /** 
        * @brief checks if the current sequence of characters is a signed literal
        * @returns true if sequence matches signed literal, false otherwise
    */
    bool isSignedLiteral() const noexcept;

    /** 
        * @brief checks if the current sequence of characters is an arithmetic operator
        * @returns true if sequence matches arithmetic operator, false otherwise
    */
    bool isAritOperator() const;

    /** 
        * @brief checks if the current sequence of characters is a bitwise operator
        * @returns true if sequence matches bitwise operator, false otherwise
    */
    bool isBitwiseOperator() const;

    /** 
        * @brief checks if the current sequence of characters is a relational operator
        * @returns true if sequence matches relational operator, false otherwise
    */
    bool isRelOperator() const;

    /** 
        * @brief skips single line comment
    */
    void singleLineComment();

    /** 
        * @brief skips multi-line comment
    */
    void multiLineComment();

};

#endif
