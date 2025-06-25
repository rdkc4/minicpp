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

ExitCode Compiler::compile(std::string_view input, std::string_view output) {
    Lexer lexer{ input };
    try{
        lexer.tokenize();
        std::cout << "\nLexical check: successful!\n";

    } catch(const std::exception& e){
        std::cerr << std::format("\nLexical check: failed\n{}\n", e.what());
        return ExitCode::LEXICAL_ERR;
    }

    try {
        // making sure tokenization was done
        assert(lexer.completedTokenization());
        
        Parser parser{ lexer };
        std::unique_ptr<ASTree> astRoot = parser.parseProgram();
        std::cout << "\nSyntax check: successful!\n";
        //astRoot->traverse(1); //display AST

        try{
            SymbolTable symbolTable {};
            ScopeManager scopeManager{ symbolTable };
            Analyzer analyzer{ scopeManager };

            analyzer.semanticCheck(astRoot.get());
            std::cout << "\nSemantic check: successful!\n";

        } catch(const std::exception& e){
            std::cerr << std::format("\nSemantic check: failed!\n{}\n", e.what());
            return ExitCode::SEMANTIC_ERR;
        }

        try{
            IntermediateRepresentation intermediateRepresentation {};
            std::unique_ptr<IRTree> irRoot = intermediateRepresentation.formIR(astRoot.get());
            std::cout << "\nForming Intermediate Representation: successful!\n";
            //irRoot->traverse(1); //display IRT

            try{
                std::string filePath { std::string{ output } };
                CodeGenerator codeGenerator{ filePath };
                codeGenerator.generateCode(irRoot.get());
                std::cout << "\nCode Generation: successful!\n";

            } catch(const std::exception& e){
                std::cerr << std::format("\nCode Generation: failed!\n{}\n", e.what());
                return ExitCode::CODEGEN_ERR;
            }

        } catch(const std::exception& e){
            std::cerr << std::format("\nForming Intermediate Representation: failed!\n{}\n", e.what());
            return ExitCode::IR_ERR;
        }

    } catch (const std::exception& e) {
        std::cerr << std::format("\nSyntax check: failed!\n{}\n", e.what());
        return ExitCode::SYNTAX_ERR;
    }
    return ExitCode::NO_ERR;
}
