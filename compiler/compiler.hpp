#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <memory>
#include <string_view>

#include "../lexer/lexer.hpp"
#include "../common/abstract-syntax-tree/ASTProgram.hpp"
#include "../common/intermediate-representation-tree/IRProgram.hpp"

namespace Compiler {
    
    enum class ExitCode { NO_ERR, LEXICAL_ERR, SYNTAX_ERR, SEMANTIC_ERR, IR_ERR, CODEGEN_ERR };

    ExitCode lexicalAnalysis(Lexer& lexer);
    ExitCode syntaxAnalysis(Lexer& lexer, std::unique_ptr<ASTProgram>& astProgram);
    ExitCode semanticAnalysis(std::unique_ptr<ASTProgram>& astProgram);
    ExitCode transformASTToIRT(std::unique_ptr<ASTProgram>& astProgram, std::unique_ptr<IRProgram>& irProgram);
    ExitCode generateCode(std::unique_ptr<IRProgram>& irProgram, const std::string_view output);

    ExitCode compile(std::string_view input, std::string_view output);
};

#endif

/*

    ExitCode handleIntermediateRepresentation(std::unique_ptr<ASTProgram>& astProgram, std::unique_ptr<IRProgram>& irProgram) {
        try {
            IntermediateRepresentation intermediateRepresentation {};
            irProgram = intermediateRepresentation.formIR(astProgram.get());
        } catch (const std::exception& e) {
            std::cerr << std::format("\nForming Intermediate Representation: failed!\n{}\n", e.what());
            return ExitCode::IR_ERR;
        }
        return ExitCode::NO_ERR;
    }

    ExitCode handleCodeGeneration(std::unique_ptr<IRProgram>& irProgram, const std::string_view output) {
        try {
            std::string filePath { std::string{ output } };
            CodeGenerator codeGenerator{ filePath };
            codeGenerator.generateCode(irProgram.get());
        } catch (const std::exception& e) {
            std::cerr << std::format("\nCode Generation: failed!\n{}\n", e.what());
            return ExitCode::CODEGEN_ERR;
        }
        return ExitCode::NO_ERR;
    }
    
}

ExitCode Compiler::compile(std::string_view input, std::string_view output) {
    Lexer lexer{ input };

    // Handle lexical analysis
    ExitCode code = handleLexicalAnalysis(input, lexer);
    if (code != ExitCode::NO_ERR) return code;

    // Handle parsing
    std::unique_ptr<ASTProgram> astProgram;
    code = handleParsing(lexer, astProgram);
    if (code != ExitCode::NO_ERR) return code;

    // Handle semantic analysis
    code = handleSemanticAnalysis(astProgram);
    if (code != ExitCode::NO_ERR) return code;

    // Handle intermediate representation generation
    std::unique_ptr<IRProgram> irProgram;
    code = handleIntermediateRepresentation(astProgram, irProgram);
    if (code != ExitCode::NO_ERR) return code;

    // Handle code generation
    return handleCodeGeneration(irProgram, output);
}
*/