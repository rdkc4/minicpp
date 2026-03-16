#include "../intermediate_representation.hpp"

#include <cassert>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <latch>

#include "../../common/abstract-syntax-tree/ast_include_dir.hpp"
#include "../../thread-pool/thread_pool.hpp"
#include "../../optimization/stack_frame_analyzer.hpp"
#include "../../optimization/dead_code_eliminator.hpp"

IntermediateRepresentation::IntermediateRepresentation() : funcIR{ exceptions } {}

std::unique_ptr<IRProgram> IntermediateRepresentation::transformProgram(const ASTProgram* program){
    std::unique_ptr<IRProgram> irProgram = std::make_unique<IRProgram>();

    dirIR.transformDir(irProgram.get(), program);
    
    const size_t total = program->getFunctionCount();
    irProgram->resizeFunctions(total);

    std::latch doneLatch{ static_cast<std::ptrdiff_t>(total) };

    ThreadPool threadPool{ std::thread::hardware_concurrency() };

    for(size_t i = 0; i < total; ++i){
        threadPool.enqueue([&, irProgram=irProgram.get(), i, function=program->getFunctionAtN(i)]{
            // generating ir of a function
            std::unique_ptr<IRFunction> irFunction = funcIR.transformFunction(function);
            irProgram->setFunctionAtN(std::move(irFunction), i);

            doneLatch.count_down();
        });
    }

    doneLatch.wait();

    // eliminating dead code from the program
    DeadCodeEliminator dce{threadPool};
    irProgram->accept(dce);

    // calculating required memory for the stack of each function
    StackFrameAnalyzer stackFrameAnalyzer{threadPool};
    irProgram->accept(stackFrameAnalyzer); 

    for(const auto& dir : program->getDirs()) {
        if(dir->getNodeType() == ASTNodeType::INCLUDE){
            irProgram->addLinkedLib(static_cast<const ASTIncludeDir*>(dir.get())->getLibName());
        }
    }

    return irProgram;
}

bool IntermediateRepresentation::hasErrors(const IRProgram* program) const noexcept {
    for(const auto& function : program->getFunctions()){
        if(!exceptions.at(function->getFunctionName()).empty()){
            return true;
        }
    }
    return false;
}

std::string IntermediateRepresentation::getErrors(const IRProgram* program) const noexcept {
    if(exceptions.empty()){
        return "";
    }

    std::stringstream errors{"Forming Intermediate Representation failed:\n"};
    size_t errLen = errors.str().length();

    for(const auto& function : program->getFunctions()){
        const std::vector<std::string>& funcErrors = exceptions.at(function->getFunctionName());
        for(const auto& error : funcErrors){
            errors << error << "\n";
        }
    }

    std::string strErrors = errors.str(); 

    return strErrors.length() != errLen ? strErrors : "";
}