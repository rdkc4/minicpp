#include "compiler.hpp"

#include <iostream>
#include <format>
#include <memory>
#include <cassert>

#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "../analyzer/analyzer.hpp"
#include "../intermediate-representation/intermediate_representation.hpp"
#include "../code-generator/code-generator/code_generator.hpp"

Compiler::ExitCode Compiler::lexicalAnalysis(Lexer& lexer){
    lexer.tokenize();
    if(lexer.hasLexicalErrors()){
        lexer.showLexicalErrors();
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
        analyzer.showSemanticErrors(astProgram.get());
        return Compiler::ExitCode::SEMANTIC_ERR;
    }

    return Compiler::ExitCode::NO_ERR;
}

Compiler::ExitCode Compiler::transformASTToIRT(std::unique_ptr<ASTProgram>& astProgram, std::unique_ptr<IRProgram>& irProgram){
        IntermediateRepresentation intermediateRepresentation{};
        irProgram = intermediateRepresentation.formIR(astProgram.get());
        if(intermediateRepresentation.hasErrors(irProgram.get())){
            intermediateRepresentation.showErrors(irProgram.get());
            return Compiler::ExitCode::IR_ERR;
        }

        return Compiler::ExitCode::NO_ERR;
}

Compiler::ExitCode Compiler::generateCode(std::unique_ptr<IRProgram>& irProgram, const std::string_view output){
    CodeGenerator codeGenerator{ std::string{ output } };
    try{
        codeGenerator.generateCode(irProgram.get());
        if(!codeGenerator.successful()){
            std::cerr << std::format("Unable to open '{}'", output);
            return Compiler::ExitCode::CODEGEN_ERR;
        }
    }
    catch(std::exception& e){
        std::cerr << std::format("\nCode Generation: failed\n{}\n", e.what());
        return Compiler::ExitCode::CODEGEN_ERR;
    }

    return Compiler::ExitCode::NO_ERR;
}

Compiler::ExitCode Compiler::compile(std::string_view input, std::string_view output) {
    Lexer lexer{ input };

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
    //astProgram->print(1);

    result = semanticAnalysis(astProgram);
    if(result != Compiler::ExitCode::NO_ERR){
        return result;
    }

    std::unique_ptr<IRProgram> irProgram;
    result = transformASTToIRT(astProgram, irProgram);
    if(result != Compiler::ExitCode::NO_ERR){
        return result;
    }
    //irProgram->print(1);

    return generateCode(irProgram, output);
}