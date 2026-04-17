#include "../function_intermediate_representation.hpp"

IR::FunctionIntermediateRepresentation::FunctionIntermediateRepresentation()
    : stmtIR{ ctx } {}

std::unique_ptr<ir::IRFunction> 
IR::FunctionIntermediateRepresentation::transformFunction(const syntax::ast::ASTFunction* astFunction){
    std::unique_ptr<ir::IRFunction> irFunction{ 
        std::make_unique<ir::IRFunction>(
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
    ir::IRFunction* irFunction, 
    const syntax::ast::ASTFunction* astFunction
){
    for(const auto& astParameter : astFunction->getParameters()){
        irFunction->addParameter(
            std::make_unique<ir::IRParameter>(
                astParameter->getToken().value, 
                astParameter->getType()
            )
        );
    }
}

void IR::FunctionIntermediateRepresentation::transformBody(
    ir::IRFunction* irFunction, 
    const syntax::ast::ASTFunction* astFunction
){
    for(const auto& astStmt : astFunction->getBody()){
        irFunction->addStatement(stmtIR.transformStmt(astStmt.get()));

        // ignores all statements after return statement
        if(astStmt->getNodeType() == syntax::ast::ASTNodeType::RETURN_STMT){
            break;
        }
    }
}

const ir::IRFunctionContext& 
IR::FunctionIntermediateRepresentation::getContext() const noexcept {
    return ctx;
}