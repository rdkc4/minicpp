#include "defs.hpp"

// supported operators
const std::unordered_set<std::string> relationalOperators {"<", ">", "<=", ">=", "==", "!=" };

const std::unordered_set<std::string> arithmeticOperators {"+", "-", "*", "/"};

const std::unordered_set<std::string> bitwiseOperators {"&", "|", "^", "<<", ">>"};

// conversions
const std::unordered_map<Types, std::string> typeToString {
    {Types::INT, "int"},
    {Types::UNSIGNED, "unsigned"},
    {Types::VOID, "void"},
    {Types::AUTO, "auto"},
    {Types::NO_TYPE, "no_type"}
};

const std::unordered_map<TokenType, Types> tokenTypeToType {
    {TokenType::_INT, Types::INT},
    {TokenType::_UNSIGNED, Types::UNSIGNED},
    {TokenType::_VOID, Types::VOID},
    {TokenType::_AUTO, Types::AUTO}
};

const std::unordered_map<Kinds, std::string> kindToString {
    {Kinds::NO_KIND, "NO_KIND"},
    {Kinds::REG, "REG"},
    {Kinds::LIT, "LIT"},
    {Kinds::FUN, "FUN"},
    {Kinds::VAR, "VAR"},
    {Kinds::PAR, "PAR"}
};
