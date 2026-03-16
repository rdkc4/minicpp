#include "../function_intermediate_representation.hpp"

#include "../../optimization/dead_code.hpp"

FunctionIntermediateRepresentation::FunctionIntermediateRepresentation(std::unordered_map<std::string, std::vector<std::string>>& exceptions) 
    : exceptions{ exceptions } {}

thread_local IRThreadContext FunctionIntermediateRepresentation::irContext;

IRThreadContext& FunctionIntermediateRepresentation::getContext() noexcept {
    return irContext;
}

std::unique_ptr<IRFunction> FunctionIntermediateRepresentation::transformFunction(const ASTFunction* astFunction){
    std::unique_ptr<IRFunction> irFunction = 
        std::make_unique<IRFunction>(astFunction->getToken().value, astFunction->getType());
    
    irContext.init();

    transformParameters(irFunction.get(), astFunction);
    if(astFunction->isPredefined()){
        irFunction->setPredefined(true);
    }
    else {
        transformBody(irFunction.get(), astFunction);
        Optimization::DeadCode::eliminateDeadCode(irFunction.get());
    }

    {
        // pairing function name with its errors
        std::lock_guard<std::mutex> lock(exceptionMtx);
        exceptions[astFunction->getToken().value] = std::move(irContext.errors); // irContext.errors are cleared after std::move()
    }

    irContext.reset();

    return irFunction;
}

void FunctionIntermediateRepresentation::transformParameters(IRFunction* irFunction, const ASTFunction* astFunction){
    for(const auto& astParameter : astFunction->getParameters()){
        irFunction->addParameter(
            std::make_unique<IRParameter>(astParameter->getToken().value, astParameter->getType())
        );
    }
}

void FunctionIntermediateRepresentation::transformBody(IRFunction* irFunction, const ASTFunction* astFunction){
    for(const auto& astStmt : astFunction->getBody()){
        irFunction->addStatement(stmtIR.transformStmt(astStmt.get()));

        // ignores all statements after return statement
        if(astStmt->getNodeType() == ASTNodeType::RETURN_STATEMENT){
            break;
        }
    }
}