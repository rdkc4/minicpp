#include "../expression_intermediate_representation.hpp"

#include <cassert>

#include "../../optimization/constant_folding.hpp"
#include "../../common/intermediate-representation-tree/ir_binary_expr.hpp"

IR::ExpressionIntermediateRepresentation::ExpressionIntermediateRepresentation(
    IR::defs::ctx::IRFunctionContext& context
) : ctx{ context } {};

std::unique_ptr<IR::node::IRExpr> 
IR::ExpressionIntermediateRepresentation::transformExpr(const syntax::ast::ASTExpr* astExpr){
    auto nodeType{ astExpr->getNodeType() };
    switch(nodeType){
        case syntax::ast::ASTNodeType::ID_EXPR:
            return transformIdExpr(static_cast<const syntax::ast::ASTIdExpr*>(astExpr));

        case syntax::ast::ASTNodeType::LITERAL_EXPR:
            return transformLiteralExpr(static_cast<const syntax::ast::ASTLiteralExpr*>(astExpr));

        case syntax::ast::ASTNodeType::FUNCTION_CALL_EXPR:
            return replaceFunctionCallExpr(static_cast<const syntax::ast::ASTFunctionCallExpr*>(astExpr));

        default:
            return transformBinaryExpr(static_cast<const syntax::ast::ASTBinaryExpr*>(astExpr));
    }
}

std::unique_ptr<IR::node::IRExpr> 
IR::ExpressionIntermediateRepresentation::transformBinaryExpr(const syntax::ast::ASTBinaryExpr* astBinaryExpr){
    std::unique_ptr<IR::node::IRExpr> leftOperand{ 
        transformExpr(astBinaryExpr->getLeftOperandExpr()) 
    };
    std::unique_ptr<IR::node::IRExpr> rightOperand{ 
        transformExpr(astBinaryExpr->getRightOperandExpr()) 
    };

    auto type{ leftOperand->getType() };
    if(leftOperand->getNodeType() == IR::defs::IRNodeType::LITERAL && 
       rightOperand->getNodeType() == IR::defs::IRNodeType::LITERAL) {

        Optimization::ConstantFolding::MergeResult<std::unique_ptr<IR::node::IRExpr>> res;

        switch(type) {
            case Type::INT:
                res = Optimization::ConstantFolding::mergeLiterals<int>(
                    static_cast<const IR::node::IRLiteralExpr*>(leftOperand.get()), 
                    static_cast<const IR::node::IRLiteralExpr*>(rightOperand.get()), 
                    astBinaryExpr
                );
                break;

            case Type::UNSIGNED:
                res = Optimization::ConstantFolding::mergeLiterals<unsigned>(
                    static_cast<const IR::node::IRLiteralExpr*>(leftOperand.get()), 
                    static_cast<const IR::node::IRLiteralExpr*>(rightOperand.get()), 
                    astBinaryExpr
                );
                break;
            
            default: break;
        }

        assert(res.result != nullptr);
        if(!res.error.empty()){
            ctx.errors.push_back(res.error);
        }

        return std::move(res.result);
    }
    
    auto nodeType{ IR::defs::resolveOperator(astBinaryExpr->getOperator(), type) };

    std::unique_ptr<IR::node::IRBinaryExpr> irBinaryExpr{ 
        std::make_unique<IR::node::IRBinaryExpr>(nodeType, type) 
    };

    irBinaryExpr->setBinaryExpr(
        std::move(leftOperand), 
        std::move(rightOperand), 
        astBinaryExpr->getOperator()
    );

    return irBinaryExpr;
}

std::unique_ptr<IR::node::IRIdExpr> 
IR::ExpressionIntermediateRepresentation::transformIdExpr(
    const syntax::ast::ASTIdExpr* astIdExpr
) const {
    return std::make_unique<IR::node::IRIdExpr>(
        astIdExpr->getToken().value, 
        astIdExpr->getType()
    );
}

std::unique_ptr<IR::node::IRLiteralExpr> 
IR::ExpressionIntermediateRepresentation::transformLiteralExpr(
    const syntax::ast::ASTLiteralExpr* astLiteralExpr
) const {
    return std::make_unique<IR::node::IRLiteralExpr>(
        astLiteralExpr->getToken().value, 
        astLiteralExpr->getType()
    );
}

std::unique_ptr<IR::node::IRFunctionCallExpr> 
IR::ExpressionIntermediateRepresentation::transformFunctionCallExpr(
    const syntax::ast::ASTFunctionCallExpr* astCallExpr
){
    std::unique_ptr<IR::node::IRFunctionCallExpr> irCallExpr{ 
        std::make_unique<IR::node::IRFunctionCallExpr>(
            astCallExpr->getToken().value, astCallExpr->getType()
        )
    };
    transformArguments(irCallExpr.get(), astCallExpr);

    return irCallExpr;
}

void IR::ExpressionIntermediateRepresentation::transformArguments(
    IR::node::IRFunctionCallExpr* irCallExpr, 
    const syntax::ast::ASTFunctionCallExpr* astCallExpr
){
    for(const auto& astArgument : astCallExpr->getArguments()){
        auto temps{ initiateTemporaries(astArgument.get()) };
        irCallExpr->addArgument(
            transformExpr(astArgument.get()), 
            std::move(temps)
        );
    }
}

// counting the number of function calls that should be replaced by temporary variables
size_t IR::ExpressionIntermediateRepresentation::countTemporaries(const syntax::ast::ASTExpr* astExpr) const {
    auto nodeType{ astExpr->getNodeType() };
    switch(nodeType){
        case syntax::ast::ASTNodeType::FUNCTION_CALL_EXPR:
            return 1;
        
        case syntax::ast::ASTNodeType::LITERAL_EXPR:
        case syntax::ast::ASTNodeType::ID_EXPR:
        case syntax::ast::ASTNodeType::VARIABLE_DECL_STMT:
            return 0;

        default:
            const auto* binExp{ static_cast<const syntax::ast::ASTBinaryExpr*>(astExpr) }; 
            return countTemporaries(binExp->getLeftOperandExpr()) + 
                countTemporaries(binExp->getRightOperandExpr());
    }
}

// generating temporary variables
std::string IR::ExpressionIntermediateRepresentation::generateTemporaries(){
    std::string name{ std::format("_t{}", ++ctx.temporaries) };
    ctx.temporaryNames.push(name);
    return name;
}

// assigning a returned value to temporary variables
void IR::ExpressionIntermediateRepresentation::assignTemporaries(
    IR::node::IRTemporaryExpr* temporaryRoot, 
    const syntax::ast::ASTExpr* astExpr, 
    size_t& idx
){
    auto nodeType{ astExpr->getNodeType() };
    switch(nodeType){
        case syntax::ast::ASTNodeType::FUNCTION_CALL_EXPR: {
            const auto* funcCall{ static_cast<const syntax::ast::ASTFunctionCallExpr*>(astExpr) };
            temporaryRoot->setTemporaryExprAtN(
                transformFunctionCallExpr(funcCall), 
                astExpr->getType(), 
                idx++
            );
            break;
        }
        
        case syntax::ast::ASTNodeType::LITERAL_EXPR:
        case syntax::ast::ASTNodeType::VARIABLE_DECL_STMT:
        case syntax::ast::ASTNodeType::ID_EXPR:
            break;

        default: {
            const auto* binExp{ static_cast<const syntax::ast::ASTBinaryExpr*>(astExpr) };
            assignTemporaries(temporaryRoot, binExp->getLeftOperandExpr(), idx);
            assignTemporaries(temporaryRoot, binExp->getRightOperandExpr(), idx);
        }
    }
}

// replacing function calls with temporary variables in expression
std::unique_ptr<IR::node::IRIdExpr> 
IR::ExpressionIntermediateRepresentation::replaceFunctionCallExpr(
    const syntax::ast::ASTFunctionCallExpr* astCallExpr
){
    assert(!ctx.temporaryNames.empty());
    std::string name{ ctx.temporaryNames.top() };
    ctx.temporaryNames.pop();
    return std::make_unique<IR::node::IRIdExpr>(name, astCallExpr->getType());
}

std::unique_ptr<IR::node::IRTemporaryExpr> 
IR::ExpressionIntermediateRepresentation::initiateTemporaries(
    const syntax::ast::ASTExpr* astExpr
){
    size_t tmpCount{ countTemporaries(astExpr) };
    if(tmpCount > 0){
        std::unique_ptr<IR::node::IRTemporaryExpr> temporaryRoot{ 
            std::make_unique<IR::node::IRTemporaryExpr>() 
        };
        
        size_t firstTemporaryIndex = 0;
        for(size_t i{0}; i < tmpCount; ++i){
            temporaryRoot->addTemporaryExpr(generateTemporaries());
        }
        assignTemporaries(temporaryRoot.get(), astExpr, firstTemporaryIndex);
        return temporaryRoot;
    }
    return nullptr;
}