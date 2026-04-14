#include "../expression_intermediate_representation.hpp"

#include <cassert>

#include "../function_intermediate_representation.hpp"
#include "../../optimization/constant_folding.hpp"
#include "../../common/intermediate-representation-tree/ir_binary_expr.hpp"

std::unique_ptr<IR::node::IRExpr> IR::ExpressionIntermediateRepresentation::transformExpr(const ASTExpr* astExpr){
    if(astExpr->getNodeType() == ASTNodeType::ID_EXPR){
        return transformIdExpr(static_cast<const ASTIdExpr*>(astExpr));
    }
    else if(astExpr->getNodeType() == ASTNodeType::LITERAL_EXPR){
        return transformLiteralExpr(static_cast<const ASTLiteralExpr*>(astExpr));
    }
    else if(astExpr->getNodeType() == ASTNodeType::FUNCTION_CALL_EXPR){
        return replaceFunctionCallExpr(static_cast<const ASTFunctionCallExpr*>(astExpr));
    }
    else{
        return transformBinaryExpr(static_cast<const ASTBinaryExpr*>(astExpr));
    }
}

std::unique_ptr<IR::node::IRExpr> IR::ExpressionIntermediateRepresentation::transformBinaryExpr(const ASTBinaryExpr* astBinaryExpr){
    std::unique_ptr<IR::node::IRExpr> leftOperand{ 
        transformExpr(astBinaryExpr->getLeftOperandExpr()) 
    };
    std::unique_ptr<IR::node::IRExpr> rightOperand{ 
        transformExpr(astBinaryExpr->getRightOperandExpr()) 
    };

    if(leftOperand->getNodeType() == IR::defs::IRNodeType::LITERAL && rightOperand->getNodeType() == IR::defs::IRNodeType::LITERAL){

        Optimization::ConstantFolding::MergeResult<std::unique_ptr<IR::node::IRExpr>> res;
        if(leftOperand->getType() == Type::INT){
            res = Optimization::ConstantFolding::mergeLiterals<int>(
                static_cast<const IR::node::IRLiteralExpr*>(leftOperand.get()), 
                static_cast<const IR::node::IRLiteralExpr*>(rightOperand.get()), 
                astBinaryExpr
            );
        }
        else if(leftOperand->getType() == Type::UNSIGNED){
            res = Optimization::ConstantFolding::mergeLiterals<unsigned>(
                static_cast<const IR::node::IRLiteralExpr*>(leftOperand.get()), 
                static_cast<const IR::node::IRLiteralExpr*>(rightOperand.get()), 
                astBinaryExpr
            );
        }

        assert(res.result != nullptr);
        if(!res.error.empty()){
            auto& irContext{ FunctionIntermediateRepresentation::getContext() };
            irContext.errors.push_back(res.error);
        }

        return std::move(res.result);
    }
    
    Type type{ leftOperand->getType() };
    IR::defs::IRNodeType nodeType{ IR::defs::resolveOperator(astBinaryExpr->getOperator(), type) };

    std::unique_ptr<IR::node::IRBinaryExpr> irBinaryExpr{ std::make_unique<IR::node::IRBinaryExpr>(nodeType, type) };
    irBinaryExpr->setBinaryExpr(
        std::move(leftOperand), 
        std::move(rightOperand), 
        astBinaryExpr->getOperator()
    );

    return irBinaryExpr;
}

std::unique_ptr<IR::node::IRIdExpr> IR::ExpressionIntermediateRepresentation::transformIdExpr(const ASTIdExpr* astIdExpr) const {
    return std::make_unique<IR::node::IRIdExpr>(
        astIdExpr->getToken().value, 
        astIdExpr->getType()
    );
}

std::unique_ptr<IR::node::IRLiteralExpr> IR::ExpressionIntermediateRepresentation::transformLiteralExpr(const ASTLiteralExpr* astLiteralExpr) const {
    return std::make_unique<IR::node::IRLiteralExpr>(
        astLiteralExpr->getToken().value, 
        astLiteralExpr->getType()
    );
}

std::unique_ptr<IR::node::IRFunctionCallExpr> IR::ExpressionIntermediateRepresentation::transformFunctionCallExpr(const ASTFunctionCallExpr* astCallExpr){
    std::unique_ptr<IR::node::IRFunctionCallExpr> irCallExpr{ 
        std::make_unique<IR::node::IRFunctionCallExpr>(
            astCallExpr->getToken().value, astCallExpr->getType()
        )
    };
    transformArguments(irCallExpr.get(), astCallExpr);

    return irCallExpr;
}

void IR::ExpressionIntermediateRepresentation::transformArguments(IR::node::IRFunctionCallExpr* irCallExpr, const ASTFunctionCallExpr* astCallExpr){
    for(const auto& astArgument : astCallExpr->getArguments()){
        auto temps{ initiateTemporaries(astArgument.get()) };
        irCallExpr->addArgument(
            transformExpr(astArgument.get()), 
            std::move(temps)
        );
    }
}

// counting the number of function calls that should be replaced by temporary variables
size_t IR::ExpressionIntermediateRepresentation::countTemporaries(const ASTExpr* astExpr) const {
    if(astExpr->getNodeType() == ASTNodeType::FUNCTION_CALL_EXPR){
        return 1;
    }
    else if(astExpr->getNodeType() == ASTNodeType::LITERAL_EXPR || 
        astExpr->getNodeType() == ASTNodeType::ID_EXPR || 
        astExpr->getNodeType() == ASTNodeType::VARIABLE_DECL_STMT
    ){
        return 0;
    }
    else{
        const ASTBinaryExpr* binExp{ static_cast<const ASTBinaryExpr*>(astExpr) }; 
        return countTemporaries(binExp->getLeftOperandExpr()) + 
            countTemporaries(binExp->getRightOperandExpr());
    }
}

// generating temporary variables
std::string IR::ExpressionIntermediateRepresentation::generateTemporaries(){
    auto& irContext{ FunctionIntermediateRepresentation::getContext() };
    std::string name{ std::format("_t{}", ++irContext.temporaries) };
    irContext.temporaryNames.push(name);
    return name;
}

// assigning a returned value to temporary variables
void IR::ExpressionIntermediateRepresentation::assignTemporaries(IR::node::IRTemporaryExpr* temporaryRoot, const ASTExpr* astExpr, size_t& idx){
    if(astExpr->getNodeType() == ASTNodeType::FUNCTION_CALL_EXPR){
    const ASTFunctionCallExpr* funcCall{ static_cast<const ASTFunctionCallExpr*>(astExpr) };

        temporaryRoot->setTemporaryExprAtN(
            transformFunctionCallExpr(funcCall), 
            astExpr->getType(), 
            idx++
        );
    }
    else if(astExpr->getNodeType() == ASTNodeType::LITERAL_EXPR || 
        astExpr->getNodeType() == ASTNodeType::VARIABLE_DECL_STMT || 
        astExpr->getNodeType() == ASTNodeType::ID_EXPR
    ){
        return;
    }
    else{
        const ASTBinaryExpr* binExp{ static_cast<const ASTBinaryExpr*>(astExpr) };
        assignTemporaries(temporaryRoot, binExp->getLeftOperandExpr(), idx);
        assignTemporaries(temporaryRoot, binExp->getRightOperandExpr(), idx);
    }
}

// replacing function calls with temporary variables in expression
std::unique_ptr<IR::node::IRIdExpr> IR::ExpressionIntermediateRepresentation::replaceFunctionCallExpr(const ASTFunctionCallExpr* astCallExpr){
    auto& irContext{ FunctionIntermediateRepresentation::getContext() }; 
    assert(!irContext.temporaryNames.empty());
    std::string name{ irContext.temporaryNames.top() };
    irContext.temporaryNames.pop();
    return std::make_unique<IR::node::IRIdExpr>(name, astCallExpr->getType());
}

std::unique_ptr<IR::node::IRTemporaryExpr> IR::ExpressionIntermediateRepresentation::initiateTemporaries(const ASTExpr* astExpr){
    size_t tmpCount{ countTemporaries(astExpr) };
    if(tmpCount > 0){
        std::unique_ptr<IR::node::IRTemporaryExpr> temporaryRoot{ std::make_unique<IR::node::IRTemporaryExpr>() };
        size_t firstTemporaryIndex = 0;
        for(size_t i{0}; i < tmpCount; ++i){
            temporaryRoot->addTemporaryExpr(generateTemporaries());
        }
        assignTemporaries(temporaryRoot.get(), astExpr, firstTemporaryIndex);
        return temporaryRoot;
    }
    return nullptr;
}