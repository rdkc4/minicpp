#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <memory>
#include <ostream>
#include <string_view>
#include <vector>
#include <string>
#include <iostream>

#include "../lexer/lexer.hpp"
#include "../common/abstract-syntax-tree/ast_program.hpp"
#include "../common/intermediate-representation-tree/ir_program.hpp"
#include "../thread-pool/thread_pool.hpp"

/** 
 * @namespace Compiler
 * @brief Module that orchestrates the process of compiling source code
*/
namespace Compiler {
    /** 
     * @enum ExitCode
     * @brief exit codes of the compiler
    */
    enum class ExitCode { 
        NO_ERR,             //< successful compilation

        PREPROCESS_ERR,     //< error during preprocessing stage
        LEXICAL_ERR,        //< error during lexical analysis (tokenization)
        SYNTAX_ERR,         //< error during syntax analysis (parsing)
        SEMANTIC_ERR,       //< error during semantic analysis

        IR_ERR,             //< error during intermediate representation generation 
        CODEGEN_ERR,        //< error during code generation
        ASM_LINK_ERR        //< error during assembly or linking stage
    };

    /**
     * @struct CompileOptions
     * @brief structure containing the compile options
    */
    struct CompileOptions {
        /// flag if compiler should dump ast structure
        bool dumpAST{false};

        /// flag if compiler should dump ir structure
        bool dumpIR{false};

        /// flag if .s file should be compiled into executable
        bool stopAfterAssembly{false};

        /// relative path to input file, .mcpp extension
        std::string input;

        /// relative path for the output (optional), no extension
        std::string output{"output"};
    };

    /**
     * @brief parses arguments into compiler options
     * @param argc - number of args
     * @param argv - pointer to the first argument
     * @returns compile options
     * @details
     * 
     * CLI: ./minicpp <input> [--dump-ast --dump-ir -s] [-o <output>]
     *
     * <input> - path to input file, mandatory .mcpp extension
     * 
     * Options:
     *
     * --dump-ast - dumps the structure of the ast
     *
     * --dump-ir - dumps the structure of the ir
     *
     * -s - stops after generating .s file
     *
     * -o <output> - path to output file
    */
    CompileOptions parseOptions(int argc, char** argv);

    /**
     * @struct PreprocessResult
     * @brief result of the preprocessing
    */
    struct PreprocessResult {
        /// exit code after preprocessing, NO_ERR or PREPROCESS_ERR
        ExitCode exitCode;

        /// preprocessed source code
        std::vector<std::string> source;
    };

    /**
     * @brief reads the source code from the input file
     * @param input - path to input file
     * @returns source code
    */
    std::string readSourceCode(const std::string& input);

    /** 
     * @brief preprocesses the source code
     * @param source - source code
     * @return pair (ExitCode, vector of preprocessed files), ExitCode is NO_ERR if preprocessing succeeded, PREPROCESS_ERR if it failed
    */
    const PreprocessResult preprocess(const std::string& source);

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
    ExitCode syntaxAnalysis(Lexer& lexer, std::unique_ptr<AST::node::ASTProgram>& astProgram);

    /** 
     * @brief performs semantic analysis of the code
     * @param astProgram - reference to the pointer of the AST program
     * @param threadPool - reference to a thread pool
     * @returns SEMANTIC_ERR if it captures any semantic errors, NO_ERR otherwise
    */
    ExitCode semanticAnalysis(std::unique_ptr<AST::node::ASTProgram>& astProgram, ThreadPool& threadPool);

    /** 
     * @brief performs transformation of the code from AST to IRT
     * @param astProgram - reference to the pointer of the AST program
     * @param irProgram - reference to the pointer of the IRT program
     * @param threadPool - reference to a thread pool
     * @returns IR_ERR if it captures any errors, NO_ERR otherwise
    */
    ExitCode transformASTToIRT(
        std::unique_ptr<AST::node::ASTProgram>& astProgram, 
        std::unique_ptr<IR::node::IRProgram>& irProgram, 
        ThreadPool& threadPool
    );

    /** 
     * @brief performs code generation
     * @param irProgram - const pointer to the IRT program
     * @param output - path of the output file
     * @param threadPool - reference to a thread pool
     * @returns CODEGEN_ERR if it fails to generate code, NO_ERR otherwise
    */
    ExitCode generateProgram(
        const IR::node::IRProgram* irProgram, 
        std::string_view output, 
        ThreadPool& threadPool
    );

    /**
     * @brief translates assembly files into executable
     * @param irProgram - const pointer to the irt program
     * @param output - path of the output file
     * @returns ASM_LINK_ERR if it fails to generate executable, NO_ERR otherwise
    */
    ExitCode assembleAndLink(const IR::node::IRProgram* irProgram, std::string_view output);

    /** 
     * @brief performs compilation of the code
     *
     * preprocessing -> lexer -> parser -> analyzer -> intermediate-representation -> code-generation -> asm + link
     * @param input - source code
     * @param output - path of the output file
     * @returns exit code depending on the result of the compilation
    */
    ExitCode compile(CompileOptions options);

    /**
     * @brief dumps the structure of the ast program
     * @param program - const pointer to the ast program
     * @param out - output stream, defaults to std::cout
    */
    void dumpAST(AST::node::ASTProgram* program, std::ostream& out = std::cout);

    /**
     * @brief dumps the structure of the ir
     * @param program - const pointer to the ir program
     * @param out - output stream, defaults to std::cout
    */
    void dumpIR(IR::node::IRProgram* program, std::ostream& out = std::cout);
};

#endif