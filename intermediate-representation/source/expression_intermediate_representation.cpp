#include "../expression_intermediate_representation.hpp"
#include "../function_intermediate_representation.hpp"

#include <cassert>

// NUMEXP - reduced to (id, literal, function call) or arithmetic operation (add, sub, mul, div)
std::unique_ptr<IRExpr> ExpressionIntermediateRepresentation::transformNumericalExpr(const ASTExpr* astNumericalExpr){
    if(astNumericalExpr->getNodeType() == ASTNodeType::ID){
        return transformIdExpr(static_cast<const ASTIdExpr*>(astNumericalExpr));
    }
    else if(astNumericalExpr->getNodeType() == ASTNodeType::LITERAL){
        return transformLiteralExpr(static_cast<const ASTLiteralExpr*>(astNumericalExpr));
    }
    else if(astNumericalExpr->getNodeType() == ASTNodeType::FUNCTION_CALL){
        return replaceFunctionCallExpr(static_cast<const ASTFunctionCallExpr*>(astNumericalExpr));
    }
    else{
        return transformBinaryExpr(static_cast<const ASTBinaryExpr*>(astNumericalExpr));
    }
}

std::unique_ptr<IRExpr> ExpressionIntermediateRepresentation::transformBinaryExpr(const ASTBinaryExpr* astBinaryExpr){
    std::unique_ptr<IRExpr> leftOperand{ transformNumericalExpr(astBinaryExpr->getLeftOperandExpr()) };
    std::unique_ptr<IRExpr> rightOperand{ transformNumericalExpr(astBinaryExpr->getRightOperandExpr()) };

    if(leftOperand->getNodeType() == IRNodeType::LITERAL && rightOperand->getNodeType() == IRNodeType::LITERAL){
        if(leftOperand->getType() == Type::INT){
            auto res = mergeLiterals<int>(static_cast<const IRLiteralExpr*>(leftOperand.get()), 
                static_cast<const IRLiteralExpr*>(rightOperand.get()), astBinaryExpr
            );
            if(!res.error.empty()){
                auto& irContext = FunctionIntermediateRepresentation::getContext();
                irContext.errors.push_back(res.error);
            }
            return std::move(res.result);
        }
        else if(leftOperand->getType() == Type::UNSIGNED){
            auto res = mergeLiterals<unsigned>(static_cast<const IRLiteralExpr*>(leftOperand.get()), 
                static_cast<const IRLiteralExpr*>(rightOperand.get()), astBinaryExpr
            );
            if(!res.error.empty()){
                auto& irContext = FunctionIntermediateRepresentation::getContext();
                irContext.errors.push_back(res.error);
            }
            return std::move(res.result);
        }
    }
    
    std::string val{ astBinaryExpr->getToken().value };
    Type type{ leftOperand->getType() };
    IRNodeType iNodeType{ operatorToIRNodeType.at(astBinaryExpr->getOperator()).getOperation(type) };

    std::unique_ptr<IRBinaryExpr> irBinaryExpr = std::make_unique<IRBinaryExpr>(iNodeType, type);
    irBinaryExpr->setBinaryExpr(std::move(leftOperand), std::move(rightOperand), astBinaryExpr->getOperator());

    return irBinaryExpr;
}

std::unique_ptr<IRBinaryExpr> ExpressionIntermediateRepresentation::transformRelationalExpr(const ASTExpr* astRelationalExpr) {
    const ASTBinaryExpr* astBinaryExpr = static_cast<const ASTBinaryExpr*>(astRelationalExpr);
    
    IRNodeType irNodeType{ operatorToIRNodeType.at(astBinaryExpr->getOperator()).getOperation(astBinaryExpr->getType()) };
    std::unique_ptr<IRBinaryExpr> irBinaryExpr = std::make_unique<IRBinaryExpr>(irNodeType, astBinaryExpr->getType());

    irBinaryExpr->setBinaryExpr(
        transformNumericalExpr(astBinaryExpr->getLeftOperandExpr()), 
        transformNumericalExpr(astBinaryExpr->getRightOperandExpr()), astBinaryExpr->getOperator()
    );
    return irBinaryExpr;
}

std::unique_ptr<IRIdExpr> ExpressionIntermediateRepresentation::transformIdExpr(const ASTIdExpr* astIdExpr) const {
    return std::make_unique<IRIdExpr>(astIdExpr->getToken().value, astIdExpr->getType());
}

std::unique_ptr<IRLiteralExpr> ExpressionIntermediateRepresentation::transformLiteralExpr(const ASTLiteralExpr* astLiteralExpr) const {
    return std::make_unique<IRLiteralExpr>(astLiteralExpr->getToken().value, astLiteralExpr->getType());
}

std::unique_ptr<IRFunctionCallExpr> ExpressionIntermediateRepresentation::transformFunctionCallExpr(const ASTFunctionCallExpr* astCallExpr){
    std::unique_ptr<IRFunctionCallExpr> irCallExpr = 
        std::make_unique<IRFunctionCallExpr>(astCallExpr->getToken().value, astCallExpr->getType());
    transformArguments(irCallExpr.get(), astCallExpr);

    return irCallExpr;
}

void ExpressionIntermediateRepresentation::transformArguments(IRFunctionCallExpr* irCallExpr, const ASTFunctionCallExpr* astCallExpr){
    for(const auto& astArgument : astCallExpr->getArguments()){
        auto temps{ initiateTemporaries(astArgument.get()) };
        irCallExpr->addArgument(transformNumericalExpr(astArgument.get()), std::move(temps));
    }
}

// counting the number of function calls that should be replaced by temporary variables
size_t ExpressionIntermediateRepresentation::countTemporaries(const ASTExpr* astExpr) const {
    if(astExpr->getNodeType() == ASTNodeType::FUNCTION_CALL){
        return 1;
    }
    else if(astExpr->getNodeType() == ASTNodeType::LITERAL || astExpr->getNodeType() == ASTNodeType::ID || astExpr->getNodeType() == ASTNodeType::VARIABLE){
        return 0;
    }
    else{
        const ASTBinaryExpr* binExp = static_cast<const ASTBinaryExpr*>(astExpr); 
        return countTemporaries(binExp->getLeftOperandExpr()) + countTemporaries(binExp->getRightOperandExpr());
    }
}

// generating temporary variables
std::string ExpressionIntermediateRepresentation::generateTemporaries(){
    auto& irContext = FunctionIntermediateRepresentation::getContext();
    std::string name = std::format("_t{}", ++irContext.temporaries);
    irContext.temporaryNames.push(name);
    return name;
}

// assigning a returned value to temporary variables
void ExpressionIntermediateRepresentation::assignTemporaries(IRTemporaryExpr* temporaryRoot, const ASTExpr* astExpr, size_t& idx){
    if(astExpr->getNodeType() == ASTNodeType::FUNCTION_CALL){
        const ASTFunctionCallExpr* funcCall = static_cast<const ASTFunctionCallExpr*>(astExpr);

        temporaryRoot->setTemporaryExprAtN(transformFunctionCallExpr(funcCall), astExpr->getType(), idx++);
    }
    else if(astExpr->getNodeType() == ASTNodeType::LITERAL || astExpr->getNodeType() == ASTNodeType::VARIABLE || astExpr->getNodeType() == ASTNodeType::ID){
        return;
    }
    else{
        const ASTBinaryExpr* binExp = static_cast<const ASTBinaryExpr*>(astExpr);
        assignTemporaries(temporaryRoot, binExp->getLeftOperandExpr(), idx);
        assignTemporaries(temporaryRoot, binExp->getRightOperandExpr(), idx);
    }
}

// replacing function calls with temporary variables in numerical expression
std::unique_ptr<IRIdExpr> ExpressionIntermediateRepresentation::replaceFunctionCallExpr(const ASTFunctionCallExpr* astCallExpr){
    auto& irContext = FunctionIntermediateRepresentation::getContext(); 
    assert(!irContext.temporaryNames.empty());
    std::string name = irContext.temporaryNames.top();
    irContext.temporaryNames.pop();
    return std::make_unique<IRIdExpr>(name, astCallExpr->getType());
}

std::unique_ptr<IRTemporaryExpr> ExpressionIntermediateRepresentation::initiateTemporaries(const ASTExpr* astExpr){
    size_t tmpCount = countTemporaries(astExpr);
    if(tmpCount > 0){
        std::unique_ptr<IRTemporaryExpr> temporaryRoot = std::make_unique<IRTemporaryExpr>();
        size_t firstTemporaryIndex = 0;
        for(size_t i = 0; i < tmpCount; ++i){
            temporaryRoot->addTemporaryExpr(generateTemporaries());
        }
        assignTemporaries(temporaryRoot.get(), astExpr, firstTemporaryIndex);
        return temporaryRoot;
    }
    return nullptr;
}