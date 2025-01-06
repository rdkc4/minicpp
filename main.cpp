#include <exception>
#include <iostream>

#include "parser/parser.hpp"
#include "analyzer/analyzer.hpp"
#include "symbol-handling/symbol-table/symbol_table.hpp"
#include "symbol-handling/scope-manager/scope_manager.hpp"
#include "intermediate-representation/intermediate_representation.hpp"
#include "code-generator/code_generator.hpp"

int main(){
    std::string input = "";
    std::string line;

    while (std::getline(std::cin, line)) {
        input += line + "\n";
    }

    Lexer lexer(input);
    try{
        lexer.tokenize();
        std::cout << "\nLexical check: successful!\n";

    } catch(const std::exception& e){
        std::cerr << "\nLexical check: failed\n" << e.what() << "\n";
        return -1;
    }

    Parser parser(lexer);

    SymbolTable symbolTable;
    ScopeManager scopeManager(symbolTable);
    Analyzer analyzer(scopeManager);
    
    std::string output = "output.s";
    IntermediateRepresentation intermediateRepresentation;
    CodeGenerator codeGenerator(output);

    try {
        auto astRoot = parser.parseProgram();
        std::cout << "\nSyntax check: successful!\n";
        astRoot->traverse(1);

        try{
            analyzer.semanticCheck(astRoot);
            std::cout << "\nSemantic check: successful!\n";
        
        } catch(const std::exception& e){
            std::cerr << "\nSemantic check: failed!\n" << e.what() << "\n";
            return -1;
        }

        try{
            auto irRoot = intermediateRepresentation.formIR(astRoot);
            std::cout << "\nForming Intermediate Representation: successful!\n";
            irRoot->traverse(1);

            try{
                codeGenerator.generateCode(irRoot);
                std::cout << "\nCode Generation: successful!\n";
                
            } catch(const std::exception& e){
                std::cerr << "\nCode Generation: failed\n" << e.what() << "\n";
                return -1;
            }

        } catch(const std::exception& e){
            std::cerr << "\nForming Intermediate Representation: failed!" << e.what() << "\n";
            return -1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "\nSyntax check: failed!\n" << e.what() << "\n";
        return -1;
    }
    
    return 0;
}
