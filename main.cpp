#include <chrono>
#include <exception>
#include <iostream>
#include <format>
#include <memory>

#include "common/abstract-syntax-tree/ASTree.hpp"
#include "common/intermediate-representation-tree/IRTree.hpp"
#include "parser/parser.hpp"
#include "analyzer/analyzer.hpp"
#include "symbol-handling/symbol-table/symbol_table.hpp"
#include "symbol-handling/scope-manager/scope_manager.hpp"
#include "intermediate-representation/intermediate_representation.hpp"
#include "code-generator/code-generator/code_generator.hpp"

bool compile(std::string input){

    Lexer lexer{ input };
    try{
        lexer.tokenize();
        std::cout << "\nLexical check: successful!\n";

    } catch(const std::exception& e){
        std::cerr << std::format("\nLexical check: failed\n{}\n", e.what());
        return false;
    }

    try {
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
            return false;
        }

        try{
            IntermediateRepresentation intermediateRepresentation {};
            std::unique_ptr<IRTree> irRoot = intermediateRepresentation.formIR(astRoot.get());
            std::cout << "\nForming Intermediate Representation: successful!\n";
            //irRoot->traverse(1); //display IRT

            try{
                std::string output { "output.s" };
                CodeGenerator codeGenerator{ output };
                codeGenerator.generateCode(irRoot.get());
                std::cout << "\nCode Generation: successful!\n";

            } catch(const std::exception& e){
                std::cerr << std::format("\nCode Generation: failed!\n{}\n", e.what());
                return false;
            }

        } catch(const std::exception& e){
            std::cerr << std::format("\nForming Intermediate Representation: failed!\n{}\n", e.what());
            return false;
        }

    } catch (const std::exception& e) {
        std::cerr << std::format("\nSyntax check: failed!\n{}\n", e.what());
        return false;
    }
    return true;
}

int main(){

    std::string input { "" };
    std::string line {};

    while (std::getline(std::cin, line)) {
        input += line + "\n";
    }

    auto start{ std::chrono::high_resolution_clock::now() };

    bool ret { compile(input) };

    auto end{ std::chrono::high_resolution_clock::now() };
    auto duration{ std::chrono::duration_cast<std::chrono::milliseconds>(end - start) };
    std::cout << std::format("\nCompilation time: {}ms\n", duration.count());
    
    if(ret){
        std::cout << "\nProgram successfully compiled!\n";
    }
    else{
        std::cerr << "\nProgram failed to compile!\n";
    }

    return 0;
}
