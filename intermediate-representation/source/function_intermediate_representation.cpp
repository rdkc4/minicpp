#include "../function_intermediate_representation.hpp"

IR::FunctionIntermediateRepresentation::FunctionIntermediateRepresentation()
    : stmtIR{ ctx } {}

std::unique_ptr<IR::node::IRFunction> 
IR::FunctionIntermediateRepresentation::transformFunction(const AST::node::ASTFunction* astFunction){
    std::unique_ptr<IR::node::IRFunction> irFunction{ 
        std::make_unique<IR::node::IRFunction>(
            astFunction->getToken().value, 
            astFunction->getType()
        )
    };

    transformParameters(irFunction.get(), astFunction);
    if(astFunction->isPredefined()){
        irFunction->setPredefined(true);
    }
    else {
        transformBody(irFunction.get(), astFunction);
    }

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

const IR::defs::ctx::IRFunctionContext& 
IR::FunctionIntermediateRepresentation::getContext() const noexcept {
    return ctx;
}