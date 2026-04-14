#include "defs.hpp"

const std::unordered_map<TokenType, Operator> tokenTypeToOperator {
    /// arithmetic
    {TokenType::PLUS, Operator::ADD},
    {TokenType::MINUS, Operator::SUB},
    {TokenType::STAR, Operator::MUL},
    {TokenType::SLASH, Operator::DIV},

    /// bitwise
    {TokenType::AMPERSEND, Operator::ANDB},
    {TokenType::PIPE, Operator::ORB},
    {TokenType::CARET, Operator::XOR},
    {TokenType::LSHIFT, Operator::LSHIFT},
    {TokenType::RSHIFT, Operator::RSHIFT},

    /// logical
    {TokenType::LOGICAL_AND, Operator::ANDL},
    {TokenType::LOGICAL_OR, Operator::ORL},

    /// comparison
    {TokenType::LESS, Operator::LESS},
    {TokenType::GREATER, Operator::GREATER},
    {TokenType::LESS_EQ, Operator::LEQUAL},
    {TokenType::GREATER_EQ, Operator::GEQUAL},
    {TokenType::EQUAL, Operator::EQUAL},
    {TokenType::NOT_EQ, Operator::NEQUAL}
};

const std::unordered_map<Type, std::string> typeToString {
    {Type::INT, "int"},
    {Type::UNSIGNED, "unsigned"},
    {Type::VOID, "void"},
    {Type::AUTO, "auto"},
    {Type::NO_TYPE, "no_type"}
};

const std::unordered_map<TokenType, Type> tokenTypeToType {
    {TokenType::INT, Type::INT},
    {TokenType::UNSIGNED, Type::UNSIGNED},
    {TokenType::VOID, Type::VOID},
    {TokenType::AUTO, Type::AUTO}
};

const std::unordered_map<Kind, std::string> kindToString {
    {Kind::NO_KIND, "NO_KIND"},
    {Kind::LIT, "LIT"},
    {Kind::FUN, "FUN"},
    {Kind::VAR, "VAR"},
    {Kind::PAR, "PAR"}
};

const std::unordered_map<Operator, std::string> operatorToString {
    /// arithmetic
    {Operator::ADD, "+"},
    {Operator::SUB, "-"},
    {Operator::MUL, "*"},
    {Operator::DIV, "/"},

    /// bitwise
    {Operator::ANDB, "&"},
    {Operator::ORB, "|"},
    {Operator::XOR, "^"},
    {Operator::LSHIFT, "<<"},
    {Operator::RSHIFT, ">>"},

    {Operator::ANDL, "&&"},
    {Operator::ORL, "||"},

    /// comparison
    {Operator::LESS, "<"},
    {Operator::GREATER, ">"},
    {Operator::LEQUAL, "<="},
    {Operator::GEQUAL, ">="},
    {Operator::EQUAL, "=="},
    {Operator::NEQUAL, "!="}
};
