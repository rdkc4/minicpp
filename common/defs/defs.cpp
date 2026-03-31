#include "defs.hpp"

const std::unordered_map<std::string, Operator> stringToOperator {
    {"+", Operator::ADD},
    {"-", Operator::SUB},
    {"*", Operator::MUL},
    {"/", Operator::DIV},
    {"&", Operator::ANDB},
    {"|", Operator::ORB},
    {"^", Operator::XOR},
    {"<<", Operator::LSHIFT},
    {">>", Operator::RSHIFT},
    {"<", Operator::LESS},
    {">", Operator::GREATER},
    {"<=", Operator::LEQUAL},
    {">=", Operator::GEQUAL},
    {"==", Operator::EQUAL},
    {"!=", Operator::NEQUAL}
};

const std::unordered_map<Type, std::string> typeToString {
    {Type::INT, "int"},
    {Type::UNSIGNED, "unsigned"},
    {Type::VOID, "void"},
    {Type::AUTO, "auto"},
    {Type::NO_TYPE, "no_type"}
};

const std::unordered_map<TokenType, Type> tokenTypeToType {
    {TokenType::_INT, Type::INT},
    {TokenType::_UNSIGNED, Type::UNSIGNED},
    {TokenType::_VOID, Type::VOID},
    {TokenType::_AUTO, Type::AUTO}
};

const std::unordered_map<Kind, std::string> kindToString {
    {Kind::NO_KIND, "NO_KIND"},
    {Kind::LIT, "LIT"},
    {Kind::FUN, "FUN"},
    {Kind::VAR, "VAR"},
    {Kind::PAR, "PAR"}
};

const std::unordered_map<Operator, std::string> operatorToString {
    {Operator::ADD, "+"},
    {Operator::SUB, "-"},
    {Operator::MUL, "*"},
    {Operator::DIV, "/"},
    {Operator::ANDB, "&"},
    {Operator::ORB, "|"},
    {Operator::XOR, "^"},
    {Operator::LSHIFT, "<<"},
    {Operator::RSHIFT, ">>"},
    {Operator::LESS, "<"},
    {Operator::GREATER, ">"},
    {Operator::LEQUAL, "<="},
    {Operator::GEQUAL, ">="},
    {Operator::EQUAL, "=="},
    {Operator::NEQUAL, "!="}
};
