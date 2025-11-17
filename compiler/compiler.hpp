#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <memory>
#include <string_view>

#include "../lexer/lexer.hpp"
#include "../common/abstract-syntax-tree/ASTProgram.hpp"
#include "../common/intermediate-representation-tree/IRProgram.hpp"

/** 
 * @namespace Compiler
 * @brief compilation module that orchestrates the process of compiling source code
*/
namespace Compiler {
    enum class ExitCode { NO_ERR, LEXICAL_ERR, SYNTAX_ERR, SEMANTIC_ERR, IR_ERR, CODEGEN_ERR };

    /** 
     * @brief performs lexical analysis of the code
     * @param lexer - reference to a lexer
     * @returns LEXICAL_ERR if it captures any lexical errors, NO_ERR otherwise
    */
    ExitCode lexicalAnalysis(Lexer& lexer);

    /** 
     * @brief performs syntax analysis of the code
     * @param lexer - reference to a lexer
     * @param astProgram - reference to the pointer of the AST program
     * @returns SYNTAX_ERR if it captures any syntax errors, NO_ERR otherwise
    */
    ExitCode syntaxAnalysis(Lexer& lexer, std::unique_ptr<ASTProgram>& astProgram);

    /** 
     * @brief performs semantic analysis of the code
     * @param astProgram - reference to the pointer of the AST program
     * @returns SEMANTIC_ERR if it captures any semantic errors, NO_ERR otherwise
    */
    ExitCode semanticAnalysis(std::unique_ptr<ASTProgram>& astProgram);

    /** 
     * @brief performs transformation of the code from AST to IRT
     * @param astProgram - reference to the pointer of the AST program
     * @param irProgram - reference to the pointer of the IRT program
     * @returns IR_ERR if it captures any errors, NO_ERR otherwise
    */
    ExitCode transformASTToIRT(std::unique_ptr<ASTProgram>& astProgram, std::unique_ptr<IRProgram>& irProgram);

    /** 
     * @brief performs code generation
     * @param irProgram - reference to the pointer of the IRT program
     * @param output - path of the output file
     * @returns CODEGEN_ERR if it fails to generate code, NO_ERR otherwise
    */
    ExitCode generateCode(std::unique_ptr<IRProgram>& irProgram, const std::string_view output);

    /** 
     * @brief performs compilation of the code
     *
     * lexer -> parser -> analyzer -> intermediate-representation -> code-generation
     * @param input - source code
     * @param output - path of the output file
     * @returns exit code depending on the result of the compilation
    */
    ExitCode compile(std::string_view input, std::string_view output);
};

#endif