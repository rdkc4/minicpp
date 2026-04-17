#include "compiler.hpp"

#include <cstring>
#include <format>
#include <cassert>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../preprocessor/preprocessor.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include "../symbol-handling/scope-manager/scope_manager.hpp"
#include "../analyzer/analyzer.hpp"
#include "../intermediate-representation/intermediate_representation.hpp"
#include "../code-generator/code-generator/code_generator.hpp"
#include "../common/dump/ast_dumper.hpp"
#include "../common/dump/ir_dumper.hpp"

extern "C" {
    extern char** environ;
}

compiler::CompileOptions compiler::parseOptions(int argc, char** argv){
    compiler::CompileOptions options;
    for(int i{1}; i < argc; ++i){
        std::string arg{ argv[i] };

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

std::string compiler::readSourceCode(const std::string& input){
    std::ifstream inputStream{ input };

    if(!inputStream.is_open()){
        throw std::runtime_error(std::format("Failed to open file '{}'", input));
    }

    std::stringstream sourceCode;
    sourceCode << inputStream.rdbuf();
    inputStream.close();

    return sourceCode.str();
}

const compiler::PreprocessResult compiler::preprocess(const std::string& source) {
    preprocessing::Preprocessor preprocessor;
    preprocessor.preprocess(source);

    bool isInvalid{ preprocessor.hasErrors() };
    if(isInvalid){
        std::cerr << preprocessor.getPreprocessErrors();
    }
    
    return {
        .exitCode = isInvalid 
            ? compiler::ExitCode::PREPROCESS_ERR 
            : compiler::ExitCode::NO_ERR,
        .source = preprocessor.getPreprocessed()
    };
}

compiler::ExitCode compiler::lexicalAnalysis(Lexer& lexer){
    lexer.tokenize();
    
    if(lexer.hasErrors()){
        std::cerr << lexer.getErrors();
        return compiler::ExitCode::LEXICAL_ERR;
    }

    return compiler::ExitCode::NO_ERR;
}

compiler::ExitCode 
compiler::syntaxAnalysis(Lexer& lexer, std::unique_ptr<syntax::ast::ASTProgram>& astProgram){
    try{
        assert(lexer.completedTokenization());
        TokenConsumer tokenConsumer{ lexer };
        Parser parser{ tokenConsumer };
        astProgram = parser.parseProgram();
    }
    catch(std::exception& e){
        std::cerr << std::format("Syntax check: failed!\n{}\n", e.what());
        return ExitCode::SYNTAX_ERR;
    }

    return compiler::ExitCode::NO_ERR;
}

compiler::ExitCode compiler::semanticAnalysis(
    std::unique_ptr<syntax::ast::ASTProgram>& astProgram, 
    ThreadPool& threadPool
){
    semantics::SymbolTable symbolTable {};
    semantics::ScopeManager scopeManager{ symbolTable };
    semantics::Analyzer analyzer{scopeManager, threadPool};
    astProgram->accept(analyzer);

    if(analyzer.hasSemanticErrors(astProgram.get())){
        std::cerr << analyzer.getSemanticErrors(astProgram.get());
        return compiler::ExitCode::SEMANTIC_ERR;
    }

    return compiler::ExitCode::NO_ERR;
}

compiler::ExitCode compiler::transformASTToIRT(
    std::unique_ptr<syntax::ast::ASTProgram>& astProgram, 
    std::unique_ptr<ir::IRProgram>& irProgram, 
    ThreadPool& threadPool
){
        ir::IntermediateRepresentation intermediateRepresentation{threadPool};
        irProgram = intermediateRepresentation.transformProgram(astProgram.get());

        if(intermediateRepresentation.hasErrors(irProgram.get())){
            std::cerr << intermediateRepresentation.getErrors(irProgram.get());
            return compiler::ExitCode::IR_ERR;
        }

        return compiler::ExitCode::NO_ERR;
}

compiler::ExitCode compiler::generateProgram(
    const ir::IRProgram* irProgram, 
    std::string_view output, 
    ThreadPool& threadPool
){
    std::string outputFilePath{ std::format("{}.s", output) };
    CodeGenerator codeGenerator{ outputFilePath, threadPool };
    try{
        codeGenerator.generateProgram(irProgram);
        if(!codeGenerator.successful()){
            std::cerr << std::format("Unable to open '{}'", outputFilePath);
            return compiler::ExitCode::CODEGEN_ERR;
        }
    }
    catch(std::exception& e){
        std::cerr << std::format("\nCode Generation: failed\n{}\n", e.what());
        return compiler::ExitCode::CODEGEN_ERR;
    }

    return compiler::ExitCode::NO_ERR;
}

compiler::ExitCode compiler::assembleAndLink(
    const ir::IRProgram* irProgram, 
    std::string_view output
){
    std::string source{ std::format("{}.s", output) };

    std::vector<std::string> args{ 
        { "clang", source, "-o", std::string{output}}
    };
    auto libs{ irProgram->getLinkedLibs() };
    args.insert(args.end(), libs.begin(), libs.end());
    args.push_back("-nostdlib");

    std::vector<char*> argv;
    argv.reserve(args.size() + 1);
    for (auto& arg : args) {
        argv.push_back(arg.data());
    }
    argv.push_back(nullptr);

    pid_t pid;
    if(posix_spawnp(&pid, "clang", nullptr, nullptr, argv.data(), environ) != 0){
        std::cerr << "Error: Failed to spawn clang process\n";
        return compiler::ExitCode::ASM_LINK_ERR;
    }

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        std::cerr << "Error: waitpid failed: " << strerror(errno) << "\n";
        return compiler::ExitCode::ASM_LINK_ERR;
    }

    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        std::cerr << "Error: Clang returned non-zero exit code\n";
        return compiler::ExitCode::ASM_LINK_ERR;
    }

    return compiler::ExitCode::NO_ERR;
}

compiler::ExitCode compiler::compile(compiler::CompileOptions options) {
    std::string source{ readSourceCode(options.input) };

    compiler::PreprocessResult preprocessResult{ preprocess(source) };
    if(preprocessResult.exitCode != compiler::ExitCode::NO_ERR){
        return preprocessResult.exitCode;
    }

    compiler::ExitCode result;

    Lexer lexer{ preprocessResult.source };
    result = lexicalAnalysis(lexer);
    if(result != compiler::ExitCode::NO_ERR){
        return result;
    }
    
    std::unique_ptr<syntax::ast::ASTProgram> astProgram;
    result = syntaxAnalysis(lexer, astProgram);
    if(result != compiler::ExitCode::NO_ERR){
        return result;
    }

    if(options.dumpAST){
        dumpAST(astProgram.get());
    }

    ThreadPool threadPool{ std::thread::hardware_concurrency() };

    result = semanticAnalysis(astProgram, threadPool);
    if(result != compiler::ExitCode::NO_ERR){
        return result;
    }

    std::unique_ptr<ir::IRProgram> irProgram;
    result = transformASTToIRT(astProgram, irProgram, threadPool);
    if(result != compiler::ExitCode::NO_ERR){
        return result;
    }

    if(options.dumpIR){
        dumpIR(irProgram.get());
    }

    result = generateProgram(irProgram.get(), options.output, threadPool);
    if(result != compiler::ExitCode::NO_ERR){
        return result;
    }
    
    if(!options.stopAfterAssembly){
        result = assembleAndLink(irProgram.get(), options.output);
    }

    return result;
}

void compiler::dumpAST(syntax::ast::ASTProgram* program, std::ostream& out){
    syntax::ast::ASTDumper dump{out};
    program->accept(dump);
}

void compiler::dumpIR(ir::IRProgram* program, std::ostream& out){
    ir::IRDumper dump{out};
    program->accept(dump);
}