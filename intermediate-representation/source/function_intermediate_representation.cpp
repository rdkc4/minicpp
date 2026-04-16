#include "../function_intermediate_representation.hpp"

IR::FunctionIntermediateRepresentation::FunctionIntermediateRepresentation(
    std::unordered_map<std::string, std::vector<std::string>>& exceptions
) : exceptions{ exceptions } {}

thread_local IR::defs::ctx::IRThreadContext IR::FunctionIntermediateRepresentation::irContext;

IR::defs::ctx::IRThreadContext& 
IR::FunctionIntermediateRepresentation::getContext() noexcept {
    return irContext;
}

std::unique_ptr<IR::node::IRFunction> 
IR::FunctionIntermediateRepresentation::transformFunction(const AST::node::ASTFunction* astFunction){
    std::unique_ptr<IR::node::IRFunction> irFunction{ 
        std::make_unique<IR::node::IRFunction>(
            astFunction->getToken().value, 
            astFunction->getType()
        )
    };
    
    irContext.init();

    transformParameters(irFunction.get(), astFunction);
    if(astFunction->isPredefined()){
        irFunction->setPredefined(true);
    }
    else {
        transformBody(irFunction.get(), astFunction);
    }

    {
        // pairing function name with its errors
        std::lock_guard<std::mutex> lock(exceptionMtx);
        exceptions[astFunction->getToken().value] = std::move(irContext.errors);
    }

    irContext.reset();

    return irFunction;
}

void IR::FunctionIntermediateRepresentation::transformParameters(
    IR::node::IRFunction* irFunction, 
    const AST::node::ASTFunction* astFunction
){
    for(const auto& astParameter : astFunction->getParameters()){
        irFunction->addParameter(
            std::make_unique<IR::node::IRParameter>(
                astParameter->getToken().value, 
                astParameter->getType()
            )
        );
    }
}

void IR::FunctionIntermediateRepresentation::transformBody(
    IR::node::IRFunction* irFunction, 
    const AST::node::ASTFunction* astFunction
){
    for(const auto& astStmt : astFunction->getBody()){
        irFunction->addStatement(stmtIR.transformStmt(astStmt.get()));

        // ignores all statements after return statement
        if(astStmt->getNodeType() == AST::defs::ASTNodeType::RETURN_STMT){
            break;
        }
    }
}