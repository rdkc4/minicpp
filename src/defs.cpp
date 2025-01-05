#include "../include/defs.hpp"

const std::unordered_set<std::string> relationalOperators = {"<", ">", "<=", ">=", "==", "!=" };

const std::unordered_set<std::string> arithmeticOperators = {"+", "-", "*", "/"};

const std::unordered_set<std::string> bitwiseOperators = {"&", "|", "^"};

const std::unordered_map<Types, std::string> typeToString = {
    {Types::INT, "int"},
    {Types::UNSIGNED, "unsigned"},
    {Types::VOID, "void"},
    {Types::NO_TYPE, "no_type"}
};

const std::unordered_map<Kinds, std::string> kindToString = {
    {Kinds::NO_KIND, "NO_KIND"},
    {Kinds::REG, "REG"},
    {Kinds::LIT, "LIT"},
    {Kinds::FUN, "FUN"},
    {Kinds::VAR, "VAR"},
    {Kinds::PAR, "PAR"}
};

const std::unordered_map<std::string, std::vector<std::string>> stringToJMP = {
    {">", {"jg", "ja"}},
    {">=", {"jge", "jae"}},
    {"<", {"jl", "jb"}},
    {"<=", {"jle", "jbe"}},
    {"==", {"je", "je"}},
    {"!=", {"jne", "jne"}}
};

const std::unordered_map<std::string, std::vector<std::string>> stringToOppJMP = {
    {">", {"jle", "jbe"}},
    {">=", {"jl", "jb"}},
    {"<", {"jge", "jae"}},
    {"<=", {"jg", "ja"}},
    {"==", {"jne", "jne"}},
    {"!=", {"je", "je"}}
};
