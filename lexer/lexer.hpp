#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <string_view>
#include <vector>

#include "../common/token/token.hpp"

/**
 * @class Lexer
 * @brief Lexical analysis of the input
*/
class Lexer{
    public:

        Lexer(std::string_view input);

        /**
         * @brief translating input into a sequence of tokens
         * @returns void 
        */
        void tokenize();

        /**
         * @brief increases nextTokenIdx
         * @returns void
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
         * @returns boolean, confirmation that entire input has been tokenized
        */
        bool completedTokenization() const noexcept;

        /** 
         * @brief checks if any lexical error has been caught
         * @returns boolean, whether or not there are any lexical errors
        */
        bool hasLexicalErrors() const noexcept;

        /**
         * @brief prints all lexical errors
         * @returns void
        */
        void showLexicalErrors() const; 
        
        /** 
         * @brief prints all tokens
         * @note debugging purposes
         * @returns void
        */
        void printTokens() const noexcept;

    private:
        /// raw text that needs to be tokenized
        std::string input;
        
        /// current position in the input
        size_t position;
        /// current line in the input
        size_t lineNumber;
        /// position at which previous line ended, used to determine the column of a token: column = position - prevLineLen
        size_t prevLineLen;

        /// index of a token that should be processed by the parser next
        size_t nextTokenIdx;
        /// vector of lexical errors
        std::vector<std::string> exceptions;
    
    protected:
        /// vector of tokens generated from the input
        std::vector<Token> tokens;
    
    private:
        /**
         * @brief increases position for 1
         * @returns void
        */
        void updatePosition() noexcept;

        /**
         * @brief increases the current position by a specified number of characters.
         * @param n - the number of characters by which to increment the position.
         * @returns void
        */
        void updatePosition(size_t n) noexcept;

        /**
         * @brief increases position for 1
         *
         * increases lineNumber for 1
         *
         * sets prevLineLen to position
         * @returns void
        */
        void updateLine() noexcept;

        /** 
         * @brief pushes keyword or id to tokens
         * @returns void
        */
        void pushID();

        /** 
         * @brief pushes literal to tokens
         * @param sign - boolean, tells if literal is signed or not, false by default
         * @returns void
        */
        void pushLiteral(bool sign = false);

        /** 
         * @brief pushes assignment to tokens
         * @returns void
        */
        void pushAssignOperator();

        /** 
         * @brief pushes bitwise operator to tokens
         * @returns void
        */
        void pushBitwiseOperator();

        /** 
         * @brief pushes arithmetic operator to tokens
         * @returns void
        */
        void pushAritOperator();

        /** 
         * @brief pushes relational operator to tokens
         * @returns void
        */
        void pushRelOperator();

        /** 
         * @brief checks if current sequence of characters is an assignment operator
         * @returns boolean
        */
        bool isAssignOperator() const noexcept;

        /** 
         * @brief checks if the current sequence of characters is a keyword
         * @param value - const string representing id or keyword
         * @returns boolean
        */
        bool isKeyword(const std::string& value) const noexcept;

        /** 
         * @brief checks if the current sequence of characters is a signed literal
         * @returns boolean
        */
        bool isSignedLiteral() const noexcept;

        /** 
         * @brief checks if the current sequence of characters is an arithmetic operator
         * @returns boolean
        */
        bool isAritOperator() const;

        /** 
         * @brief checks if the current sequence of characters is a bitwise operator
         * @returns boolean
        */
        bool isBitwiseOperator() const;

        /** 
         * @brief checks if the current sequence of characters is a relational operator
         * @returns boolean
        */
        bool isRelOperator() const;

        /** 
         * @brief skips single line comment
         * @returns void
        */
        void singleLineComment();

        /** 
         * @brief skips multi-line comment
         * @returns void
        */
        void multiLineComment();

};

#endif
