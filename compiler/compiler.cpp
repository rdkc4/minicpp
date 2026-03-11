#include "compiler.hpp"

#include <cstdlib>
#include <format>
#include <cassert>
#include <stdexcept>
#include <filesystem>
#include <fstream>

#include "../preprocessor/preprocessor.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "../analyzer/analyzer.hpp"
#include "../intermediate-representation/intermediate_representation.hpp"
#include "../code-generator/code-generator/code_generator.hpp"
#include "../common/dump/ast_dumper.hpp"
#include "../common/dump/ir_dumper.hpp"

Compiler::CompileOptions Compiler::parseOptions(int argc, char** argv){
    Compiler::CompileOptions options;
    for(int i = 1; i < argc; ++i){
        std::string arg = argv[i];

        if(arg == "--dump-ast"){
            options.dumpAST = true;
        }
        else if(arg == "--dump-ir"){
            options.dumpIR = true;
        }
        else if(arg == "-o"){
            if(i + 1 >= argc){
                throw std::runtime_error("-o requires argument");
            }
            options.output = argv[++i];
        }
        else if(arg == "-s"){
            options.stopAfterAssembly = true;
        }
        else if (arg.starts_with("-")){
            throw std::runtime_error(std::format("Unknown compiler flag: {}", arg));
        }
        else if(options.input.empty()){
            options.input = arg;
        }
        else {
            throw std::runtime_error(std::format("Unexpected argument: {}", arg));
        }
    }

    if(options.input.empty()){
        throw std::runtime_error("Missing input file");
    }

    if(std::filesystem::path(options.input).extension() != ".mcpp"){
        throw std::runtime_error("Input file must have .mcpp extension");
    }

    return options;
}

std::string Compiler::readSourceCode(const std::string& input){
    std::ifstream inputStream{ input };

    if(!inputStream.is_open()){
        throw std::runtime_error(std::format("Failed to open file '{}'", input));
    }

    std::stringstream sourceCode;
    sourceCode << inputStream.rdbuf();
    inputStream.close();

    return sourceCode.str();
}

const Compiler::PreprocessResult Compiler::preprocess(const std::string& source) {
    Preprocessor preprocessor;
    preprocessor.preprocess(source);

    bool isInvalid = preprocessor.hasErrors();
    if(isInvalid){
        std::cerr << preprocessor.getPreprocessErrors();
    }
    
    return {
        .exitCode = isInvalid ? Compiler::ExitCode::PREPROCESS_ERR : Compiler::ExitCode::NO_ERR,
        .source = preprocessor.getPreprocessed()
    };
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
        irProgram = intermediateRepresentation.transformProgram(astProgram.get());

        if(intermediateRepresentation.hasErrors(irProgram.get())){
            std::cerr << intermediateRepresentation.getErrors(irProgram.get());
            return Compiler::ExitCode::IR_ERR;
        }

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

Compiler::ExitCode Compiler::compile(Compiler::CompileOptions options) {
    std::string source = readSourceCode(options.input);

    Compiler::PreprocessResult preprocessResult{ preprocess(source) };
    if(preprocessResult.exitCode != Compiler::ExitCode::NO_ERR){
        return preprocessResult.exitCode;
    }

    Compiler::ExitCode result;

    Lexer lexer{ preprocessResult.source };
    result = lexicalAnalysis(lexer);
    if(result != Compiler::ExitCode::NO_ERR){
        return result;
    }
    
    std::unique_ptr<ASTProgram> astProgram;
    result = syntaxAnalysis(lexer, astProgram);
    if(result != Compiler::ExitCode::NO_ERR){
        return result;
    }

    if(options.dumpAST){
        dumpAST(astProgram.get());
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

    if(options.dumpIR){
        dumpIR(irProgram.get());
    }

    result = generateCode(irProgram.get(), options.output);
    if(result != Compiler::ExitCode::NO_ERR){
        return result;
    }
    
    if(!options.stopAfterAssembly){
        result = assembleAndLink(irProgram.get(), options.output);
    }

    return result;
}

void Compiler::dumpAST(ASTProgram* program, std::ostream& out){
    ASTDumper dump{out};
    program->accept(dump);
}

void Compiler::dumpIR(IRProgram* program, std::ostream& out){
    IRDumper dump{out};
    program->accept(dump);
}