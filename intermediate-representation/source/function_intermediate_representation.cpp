#include "../function_intermediate_representation.hpp"

#include "../../optimization/dead_code.hpp"
#include "../../optimization/stack_memory.hpp"

FunctionIntermediateRepresentation::FunctionIntermediateRepresentation(std::unordered_map<std::string, std::vector<std::string>>& exceptions) : exceptions{ exceptions } {}

thread_local IRThreadContext FunctionIntermediateRepresentation::irContext;

IRThreadContext& FunctionIntermediateRepresentation::getContext() noexcept {
    return irContext;
}

std::unique_ptr<IRFunction> FunctionIntermediateRepresentation::function(const ASTFunction* _function){
    std::unique_ptr<IRFunction> _irFunction = 
        std::make_unique<IRFunction>(IRNodeType::FUNCTION, _function->getToken().value, _function->getType());
    
    irContext.init();

    parameter(_irFunction.get(), _function->getParameters());
    body(_irFunction.get(), _function->getBody());

    Optimization::DeadCode::eliminateDeadCode(_irFunction.get());

    // bytes allocated for local variables
    size_t requiredMemory = Optimization::StackMemory::computeStackMemory(_irFunction.get());
    std::string varCountStr{ std::to_string(requiredMemory) };
    _irFunction->setRequiredMemory(varCountStr);

    {
        // pairing function name with its errors
        std::lock_guard<std::mutex> lock(exceptionMtx);
        exceptions[_function->getToken().value] = std::move(irContext.errors); // irContext.errors are cleared after std::move()
    }

    irContext.reset();

    return _irFunction;
}

void FunctionIntermediateRepresentation::parameter(IRFunction* _irFunction, const std::vector<std::unique_ptr<ASTParameter>>& _parameters){
    for(const auto& _parameter : _parameters){
        _irFunction->addParameter(std::make_unique<IRParameter>(IRNodeType::PARAMETER, _parameter->getToken().value, _parameter->getType()));
    }
}

void FunctionIntermediateRepresentation::body(IRFunction* _irFunction, const std::vector<std::unique_ptr<ASTStatement>>& _body){
    for(const auto& _statement : _body){
        _irFunction->addStatement(stmtIR.statement(_statement.get()));

        // ignore all constructs after return statement
        if(_statement->getNodeType() == ASTNodeType::RETURN_STATEMENT){
            break;
        }
    }
}