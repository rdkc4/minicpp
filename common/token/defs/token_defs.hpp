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

    /// arithmetic operators
    PLUS,         //< +
    MINUS,        //< -
    STAR,         //< *
    SLASH,        //< /

    /// bitwise operators
    AMPERSEND,    //< &
    PIPE,         //< |
    CARET,        //< ^
    LSHIFT,       //< <<
    RSHIFT,       //< >>

    /// relational operators
    LESS,         //< <
    GREATER,      //< >
    LESS_EQ,      //< <=
    GREATER_EQ,   //< >=
    EQUAL,        //< ==
    NOT_EQ,       //< !=

    /// logical operators
    LOGICAL_AND,  //< &&
    LOGICAL_OR,   //< ||

    /// assign operators
    ASSIGN,       //< =

    /// punctuations
    LPAREN,       //< (
    RPAREN,       //< )
    LBRACE,       //< {
    RBRACE,       //< }
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

/// number of token types
constexpr size_t TOKEN_TYPE_COUNT{ static_cast<size_t>(TokenType::_EOF) + 1 };

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