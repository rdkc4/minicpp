#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <string_view>

enum class ExitCode { NO_ERR, LEXICAL_ERR, SYNTAX_ERR, SEMANTIC_ERR, IR_ERR, CODEGEN_ERR };

class Compiler{
    public:
        ExitCode compile(std::string_view input, std::string_view output);

};

#endif