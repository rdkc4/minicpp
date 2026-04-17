#ifndef TOKEN_DEFS_HPP
#define TOKEN_DEFS_HPP

#include <array>
#include <string_view>

namespace syntax {
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
        ASTERISK,     //< *
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

    /// number of general token types
    constexpr size_t GENERAL_TOKEN_TYPE_COUNT{ static_cast<size_t>(GeneralTokenType::OTHER) + 1 };

    /// maps token types to their string representations
    constexpr std::array<std::string_view, TOKEN_TYPE_COUNT> tokenTypeStringRepresentations {
        [] {
            /**
             * @brief converts token type enum to array index
             * @param type - element of the token type enum
             * @returns index in the array for provided token type
            */
            constexpr auto idx {
                [](TokenType type) noexcept -> size_t {
                    return static_cast<size_t>(type);
                }
            };

            std::array<std::string_view, TOKEN_TYPE_COUNT> str{};

            str[idx(TokenType::ID)]          = "ID";
            str[idx(TokenType::LITERAL)]     = "LITERAL";

            str[idx(TokenType::PLUS)]        = "PLUS";
            str[idx(TokenType::MINUS)]       = "MINUS";
            str[idx(TokenType::ASTERISK)]    = "ASTERISK";
            str[idx(TokenType::SLASH)]       = "SLASH";

            str[idx(TokenType::AMPERSEND)]   = "AMPERSEND";
            str[idx(TokenType::PIPE)]        = "PIPE";
            str[idx(TokenType::CARET)]       = "CARET";
            str[idx(TokenType::LSHIFT)]      = "LSHIFT";
            str[idx(TokenType::RSHIFT)]      = "RSHIFT";

            str[idx(TokenType::LESS)]        = "LESS";
            str[idx(TokenType::GREATER)]     = "GREATER";
            str[idx(TokenType::LESS_EQ)]     = "LESS_EQ";
            str[idx(TokenType::GREATER_EQ)]  = "GREATER_EQ";
            str[idx(TokenType::EQUAL)]       = "EQUAL";
            str[idx(TokenType::NOT_EQ)]      = "NOT_EQ";

            str[idx(TokenType::LOGICAL_AND)] = "LOGICAL_AND";
            str[idx(TokenType::LOGICAL_OR)]  = "LOGICAL_OR";

            str[idx(TokenType::ASSIGN)]      = "ASSIGN";

            str[idx(TokenType::LPAREN)]      = "LPAREN";
            str[idx(TokenType::RPAREN)]      = "RPAREN";
            str[idx(TokenType::LBRACE)]      = "LBRACE";
            str[idx(TokenType::RBRACE)]      = "RBRACE";
            str[idx(TokenType::SEMICOLON)]   = "SEMICOLON";
            str[idx(TokenType::COLON)]       = "COLON";
            str[idx(TokenType::HASH)]        = "HASH";
            str[idx(TokenType::COMMA)]       = "COMMA";

            str[idx(TokenType::IF)]          = "IF";
            str[idx(TokenType::ELSE)]        = "ELSE";
            str[idx(TokenType::WHILE)]       = "WHILE";
            str[idx(TokenType::DO)]          = "DO";
            str[idx(TokenType::FOR)]         = "FOR";
            str[idx(TokenType::RETURN)]      = "RETURN";
            str[idx(TokenType::SWITCH)]      = "SWITCH";
            str[idx(TokenType::CASE)]        = "CASE";
            str[idx(TokenType::DEFAULT)]     = "DEFAULT";
            str[idx(TokenType::BREAK)]       = "BREAK";

            str[idx(TokenType::INT)]         = "INT";
            str[idx(TokenType::UNSIGNED)]    = "UNSIGNED";
            str[idx(TokenType::VOID)]        = "VOID";
            str[idx(TokenType::AUTO)]        = "AUTO";

            str[idx(TokenType::INCLUDE)]     = "INCLUDE";

            str[idx(TokenType::INVALID)]     = "INVALID";
            str[idx(TokenType::NODE)]        = "NODE";
            str[idx(TokenType::_EOF)]        = "EOF";

            return str;
        }()
    };

    /**
     * @brief getter for the string representation of the token type
     * @param type - type of the token
     * @returns string representation of the token type
    */
    constexpr std::string_view tokenTypeToStr(TokenType type) {
        return tokenTypeStringRepresentations[static_cast<size_t>(type)];
    }

    /// maps general token types to their string representations
    constexpr std::array<std::string_view, GENERAL_TOKEN_TYPE_COUNT> generalTokenTypeStringRepresentations {
        [] {
            /**
             * @brief converts general token type enum to array index
             * @param type - element of the general token type enum
             * @returns index in the array for provided general token type
            */
            constexpr auto idx {
                [](GeneralTokenType type) noexcept -> size_t {
                    return static_cast<size_t>(type);
                }
            };

            std::array<std::string_view, GENERAL_TOKEN_TYPE_COUNT> str{};

            str[idx(GeneralTokenType::VALUE)]    = "VALUE";
            str[idx(GeneralTokenType::OPERATOR)] = "OPERATOR";
            str[idx(GeneralTokenType::TYPE)]     = "TYPE";
            str[idx(GeneralTokenType::OTHER)]    = "OTHER";

            return str;
        }()
    };

    /**
     * @brief getter for the string representation of the general token type
     * @param type - general type of the token
     * @returns string representation of the general token type
    */
    constexpr std::string_view generalTokenTypeToStr(GeneralTokenType type){
        return generalTokenTypeStringRepresentations[static_cast<size_t>(type)];
    }

}

#endif