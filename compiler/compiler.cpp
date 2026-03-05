#include "compiler.hpp"

#include <cstdlib>
#include <iostream>
#include <format>
#include <memory>
#include <cassert>
#include <string_view>

#include "../preprocessor/preprocessor.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "../analyzer/analyzer.hpp"
#include "../intermediate-representation/intermediate_representation.hpp"
#include "../code-generator/code-generator/code_generator.hpp"

const std::pair<Compiler::ExitCode, std::vector<std::string>> Compiler::preprocess(const std::string& source) {
    Preprocessor preprocessor;
    preprocessor.preprocess(source);

    if(preprocessor.hasErrors()){
        std::cerr << preprocessor.getPreprocessErrors();
        return { Compiler::ExitCode::PREPROCESS_ERR, preprocessor.getPreprocessed() };
    }

    return { Compiler::ExitCode::NO_ERR, preprocessor.getPreprocessed() };
}

Compiler::ExitCode Compiler::lexicalAnalysis(Lexer& lexer){
    lexer.tokenize();
    
    if(lexer.hasLexicalErrors()){
        std::cerr << lexer.getLexicalErrors();
        return Compiler::ExitCode::LEXICAL_ERR;
    }

    return Compiler::ExitCode::NO_ERR;
}

Compiler::ExitCode Compiler::syntaxAnalysis(Lexer& lexer, std::unique_ptr<ASTProgram>& astProgram){
    try{
        assert(lexer.completedTokenization());
        TokenConsumer tokenConsumer{ lexer };
        Parser parser{ tokenConsumer };
        astProgram = parser.parseProgram();
    }
    catch(std::exception& e){
        std::cerr << std::format("\nSyntax check: failed!\n{}\n", e.what());
        return ExitCode::SYNTAX_ERR;
    }

    return Compiler::ExitCode::NO_ERR;
}

Compiler::ExitCode Compiler::semanticAnalysis(std::unique_ptr<ASTProgram>& astProgram){
    SymbolTable symbolTable {};
    ScopeManager scopeManager{ symbolTable };
    Analyzer analyzer{ scopeManager };
    analyzer.semanticCheck(astProgram.get());

    if(analyzer.hasSemanticError(astProgram.get())){
        std::cerr << analyzer.getSemanticErrors(astProgram.get());
        return Compiler::ExitCode::SEMANTIC_ERR;
    }

    return Compiler::ExitCode::NO_ERR;
}

Compiler::ExitCode Compiler::transformASTToIRT(std::unique_ptr<ASTProgram>& astProgram, std::unique_ptr<IRProgram>& irProgram){
        IntermediateRepresentation intermediateRepresentation{};
        irProgram = intermediateRepresentation.formIR(astProgram.get());

        if(intermediateRepresentation.hasErrors(irProgram.get())){
            std::cerr << intermediateRepresentation.getErrors(irProgram.get());
            return Compiler::ExitCode::IR_ERR;
        }
        std::cout << irProgram->getLinkedLibs() << "\n";

        return Compiler::ExitCode::NO_ERR;
}

Compiler::ExitCode Compiler::generateCode(const IRProgram* irProgram, const std::string_view output){
    std::string outputFilePath = std::string{ output } + ".s";
    CodeGenerator codeGenerator{ std::string{ outputFilePath } };
    try{
        codeGenerator.generateCode(irProgram);
        if(!codeGenerator.successful()){
            std::cerr << std::format("Unable to open '{}'", outputFilePath);
            return Compiler::ExitCode::CODEGEN_ERR;
        }
    }
    catch(std::exception& e){
        std::cerr << std::format("\nCode Generation: failed\n{}\n", e.what());
        return Compiler::ExitCode::CODEGEN_ERR;
    }

    return Compiler::ExitCode::NO_ERR;
}

Compiler::ExitCode Compiler::assembleAndLink(const IRProgram* irProgram, const std::string_view output){
    std::string cmd = std::format("clang {}.s{} -o {} -nostdlib", output, irProgram->getLinkedLibs(), output);

    int result = std::system(cmd.c_str());
    if(result != 0){
        std::cerr << "Error: compilation/linking failed\n";
        return Compiler::ExitCode::ASM_LINK_ERR;
    }
    return Compiler::ExitCode::NO_ERR;
}

Compiler::ExitCode Compiler::compile(const std::string& input, std::string_view output) {
    auto preprocessResult = preprocess(std::string{ input });
    if(preprocessResult.first != Compiler::ExitCode::NO_ERR){
        return preprocessResult.first;
    }
    Lexer lexer{ preprocessResult.second };

    Compiler::ExitCode result;

    result = lexicalAnalysis(lexer);
    if(result != Compiler::ExitCode::NO_ERR){
        return result;
    }
    
    std::unique_ptr<ASTProgram> astProgram;
    result = syntaxAnalysis(lexer, astProgram);
    if(result != Compiler::ExitCode::NO_ERR){
        return result;
    }

    result = semanticAnalysis(astProgram);
    if(result != Compiler::ExitCode::NO_ERR){
        return result;
    }

    std::unique_ptr<IRProgram> irProgram;
    result = transformASTToIRT(astProgram, irProgram);
    if(result != Compiler::ExitCode::NO_ERR){
        return result;
    }

    result = generateCode(irProgram.get(), output);
    if(result != Compiler::ExitCode::NO_ERR){
        return result;
    }

    return assembleAndLink(irProgram.get(), output);
}