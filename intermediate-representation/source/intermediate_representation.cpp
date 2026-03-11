#include "../intermediate_representation.hpp"

#include <cassert>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <latch>

#include "../../common/abstract-syntax-tree/ast_include_dir.hpp"
#include "../../thread-pool/thread_pool.hpp"

IntermediateRepresentation::IntermediateRepresentation() : funcIR{ exceptions } {}

std::unique_ptr<IRProgram> IntermediateRepresentation::transformProgram(const ASTProgram* program){
    std::unique_ptr<IRProgram> irProgram = std::make_unique<IRProgram>(IRNodeType::PROGRAM);

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

    for(const auto& dir : program->getDirectives()) {
        if(dir->getNodeType() == ASTNodeType::INCLUDE){
            irProgram->addLinkedLibrary(static_cast<const ASTIncludeDir*>(dir.get())->getLibName());
        }
    }

    return irProgram;
}

bool IntermediateRepresentation::hasErrors(const IRProgram* _program) const noexcept {
    for(const auto& _function : _program->getFunctions()){
        if(!exceptions.at(_function->getFunctionName()).empty()){
            return true;
        }
    }
    return false;
}

std::string IntermediateRepresentation::getErrors(const IRProgram* _program) const noexcept {
    if(exceptions.empty()){
        return "";
    }

    std::stringstream errors{"Forming Intermediate Representation failed:\n"};
    size_t errLen = errors.str().length();

    for(const auto& _function : _program->getFunctions()){
        const std::vector<std::string>& funcErrors = exceptions.at(_function->getFunctionName());
        for(const auto& error : funcErrors){
            errors << error << "\n";
        }
    }

    std::string strErrors = errors.str(); 

    return strErrors.length() != errLen ? strErrors : "";
}