#ifndef TOKEN_DEFS_HPP
#define TOKEN_DEFS_HPP

#include <string>
#include <unordered_map>

/** 
 * @enum TokenType
 * @brief types of the tokens defined by the compiler
*/
enum class TokenType{
    /// identifiers and literals
    ID,           //< identifier (variables, function names etc.) 
    LITERAL,      //< numeric literals

    /// operators
    ARITHMETIC,   //< +, -, *, /
    BITWISE,      //< &, |, ^, <<, >>
    RELATIONAL,   //< <, >, <=, >=, ==, !=
    ASSIGN,       //< =

    /// delimiters and punctuations
    LPAREN,       //< (
    RPAREN,       //< )
    LBRACKET,     //< {
    RBRACKET,     //< }
    SEMICOLON,    //< ;
    COLON,        //< :
    HASH,         //< #
    COMMA,        //< ,

    /// control flow keywords
    IF, 
    ELSE, 
    WHILE, 
    DO, 
    FOR, 
    RETURN,
    SWITCH, 
    CASE, 
    DEFAULT, 
    BREAK,

    /// type keywords
    INT, 
    UNSIGNED, 
    VOID, 
    AUTO,

    /// preprocessor
    INCLUDE,

    ///special tokens
    INVALID, 
    NODE, 
    _EOF
};

/** 
 * @enum GeneralTokenType
 * @brief general types of the tokens
*/
enum class GeneralTokenType { 
    VALUE,      //< identifiers and literals
    OPERATOR,   //< all operators
    TYPE,       //< type keywords
    OTHER       //< everything else (keywords, punctuations, etc.)
};

/// maps token type to a string
extern const std::unordered_map<TokenType, std::string> tokenTypeToString;

/// maps general token type to a string
extern const std::unordered_map<GeneralTokenType, std::string> generalTokenTypeToString;

#endif