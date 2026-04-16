#include "../statement_intermediate_representation.hpp"

#include <cassert>
#include <utility>

IR::StatementIntermediateRepresentation::StatementIntermediateRepresentation(
    IR::defs::ctx::IRFunctionContext& context
) : exprIR{ context } {}

std::unique_ptr<IR::node::IRStmt> 
IR::StatementIntermediateRepresentation::transformStmt(const AST::node::ASTStmt* astStmt){
    switch(astStmt->getNodeType()){
        case AST::defs::ASTNodeType::VARIABLE_DECL_STMT:
            return transformVariableDeclStmt(static_cast<const AST::node::ASTVariableDeclStmt*>(astStmt));

        case AST::defs::ASTNodeType::IF_STMT:
            return transformIfStmt(static_cast<const AST::node::ASTIfStmt*>(astStmt));

        case AST::defs::ASTNodeType::COMPOUND_STMT:
            return transformCompoundStmt(static_cast<const AST::node::ASTCompoundStmt*>(astStmt));

        case AST::defs::ASTNodeType::ASSIGN_STMT:
            return transformAssignStmt(static_cast<const AST::node::ASTAssignStmt*>(astStmt));

        case AST::defs::ASTNodeType::RETURN_STMT:
            return transformReturnStmt(static_cast<const AST::node::ASTReturnStmt*>(astStmt));

        case AST::defs::ASTNodeType::WHILE_STMT:
            return transformWhileStmt(static_cast<const AST::node::ASTWhileStmt*>(astStmt));

        case AST::defs::ASTNodeType::FOR_STMT:
            return transformForStmt(static_cast<const AST::node::ASTForStmt*>(astStmt));

        case AST::defs::ASTNodeType::DOWHILE_STMT:
            return transformDoWhileStmt(static_cast<const AST::node::ASTDoWhileStmt*>(astStmt));

        case AST::defs::ASTNodeType::SWITCH_STMT:
            return transformSwitchStmt(static_cast<const AST::node::ASTSwitchStmt*>(astStmt));

        case AST::defs::ASTNodeType::FUNCTION_CALL_STMT:
            return transformFunctionCallStmt(static_cast<const AST::node::ASTFunctionCallStmt*>(astStmt));

        default:
            std::unreachable();
    }
}

std::unique_ptr<IR::node::IRVariableDeclStmt> 
IR::StatementIntermediateRepresentation::transformVariableDeclStmt(
    const AST::node::ASTVariableDeclStmt* astVariableDecl
){
    std::unique_ptr<IR::node::IRVariableDeclStmt> irVariableDecl{ 
        std::make_unique<IR::node::IRVariableDeclStmt>(
            astVariableDecl->getToken().value, 
            astVariableDecl->getType()
        )
    };
    
    if(astVariableDecl->hasAssignExpr()){
        auto temps{ exprIR.initiateTemporaries(astVariableDecl->getAssignExpr()) };
        irVariableDecl->setAssignExpr(
            exprIR.transformExpr(astVariableDecl->getAssignExpr()), 
            std::move(temps)
        );
    }
    return irVariableDecl;
}

std::unique_ptr<IR::node::IRIfStmt> 
IR::StatementIntermediateRepresentation::transformIfStmt(const AST::node::ASTIfStmt* astIfStmt){
    std::unique_ptr<IR::node::IRIfStmt> irIfStmt{ 
        std::make_unique<IR::node::IRIfStmt>() 
    };

    const auto& astConds{ astIfStmt->getConditionExprs() };
    const auto& astStmts{ astIfStmt->getStmts() };

    const size_t conditionCount{ astConds.size() };

    for(size_t i{0}; i < conditionCount; ++i){
        auto temps{ exprIR.initiateTemporaries(astConds[i].get()) };
        irIfStmt->addIfStmt(
            exprIR.transformExpr(astConds[i].get()), 
            transformStmt(astStmts[i].get()), 
            std::move(temps)
        );
    }

    if(conditionCount != astStmts.size()){
        irIfStmt->addElseStmt(transformStmt(astStmts.back().get()));
    }

    return irIfStmt;
}

std::unique_ptr<IR::node::IRCompoundStmt> 
IR::StatementIntermediateRepresentation::transformCompoundStmt(
    const AST::node::ASTCompoundStmt* astCompoundStmt
){
    std::unique_ptr<IR::node::IRCompoundStmt> irCompoundStmt{ 
        std::make_unique<IR::node::IRCompoundStmt>() 
    };

    for(const auto& astStmt : astCompoundStmt->getStmts()){
        irCompoundStmt->addStmt(transformStmt(astStmt.get()));

        // ignore all statements after return statement
        if(astStmt->getNodeType() == AST::defs::ASTNodeType::RETURN_STMT){
            break;
        }
    }
    return irCompoundStmt;
}

std::unique_ptr<IR::node::IRAssignStmt> 
IR::StatementIntermediateRepresentation::transformAssignStmt(
    const AST::node::ASTAssignStmt* astAssignStmt
){
    std::unique_ptr<IR::node::IRAssignStmt> irAssignStmt{ 
        std::make_unique<IR::node::IRAssignStmt>() 
    };

    // extracting function calls to temporary variables
    auto tempExpr{ exprIR.initiateTemporaries(astAssignStmt->getAssignedExpr()) };
    irAssignStmt->setAssignStmt(
        exprIR.transformIdExpr(astAssignStmt->getVariableIdExpr()), 
        exprIR.transformExpr(astAssignStmt->getAssignedExpr()), 
        std::move(tempExpr)
    );

    return irAssignStmt;
}

std::unique_ptr<IR::node::IRReturnStmt> 
IR::StatementIntermediateRepresentation::transformReturnStmt(
    const AST::node::ASTReturnStmt* astReturnStmt
){
    std::unique_ptr<IR::node::IRReturnStmt> irReturnStmt{ 
        std::make_unique<IR::node::IRReturnStmt>() 
    };

    if(astReturnStmt->hasReturnExpr()){
        // extracting function calls to temporary variables
        auto temps{ exprIR.initiateTemporaries(astReturnStmt->getReturnExpr()) };

        irReturnStmt->setReturnExpr(
            exprIR.transformExpr(astReturnStmt->getReturnExpr()), 
            std::move(temps)
        );
    }
    return irReturnStmt;
}

std::unique_ptr<IR::node::IRFunctionCallStmt> 
IR::StatementIntermediateRepresentation::transformFunctionCallStmt(
    const AST::node::ASTFunctionCallStmt* astCallStmt
){
    std::unique_ptr<IR::node::IRFunctionCallStmt> irCallStmt{ 
        std::make_unique<IR::node::IRFunctionCallStmt>() 
    };

    irCallStmt->setFunctionCallStmt(
        exprIR.transformFunctionCallExpr(
            astCallStmt->getFunctionCallExpr()
        )
    );

    return irCallStmt;
}

std::unique_ptr<IR::node::IRWhileStmt> 
IR::StatementIntermediateRepresentation::transformWhileStmt(
    const AST::node::ASTWhileStmt* astWhileStmt
){
    std::unique_ptr<IR::node::IRWhileStmt> irWhileStmt{ 
        std::make_unique<IR::node::IRWhileStmt>() 
    };

    auto temps{ exprIR.initiateTemporaries(astWhileStmt->getConditionExpr()) };
    irWhileStmt->setWhileStmt(
        exprIR.transformExpr(astWhileStmt->getConditionExpr()), 
        transformStmt(astWhileStmt->getStmt()), 
        std::move(temps)
    );

    return irWhileStmt;
}

std::unique_ptr<IR::node::IRForStmt> 
IR::StatementIntermediateRepresentation::transformForStmt(
    const AST::node::ASTForStmt* astForStmt
){
    std::unique_ptr<IR::node::IRForStmt> irForStmt{ 
        std::make_unique<IR::node::IRForStmt>() 
    };
    
    std::unique_ptr<IR::node::IRAssignStmt> irForInit{ nullptr }, irForInc{ nullptr };
    std::unique_ptr<IR::node::IRExpr> irForCondition{ nullptr };
    std::unique_ptr<IR::node::IRTemporaryExpr> temps{ nullptr };

    if(astForStmt->hasInitializerStmt()){
        irForInit = transformAssignStmt(astForStmt->getInitializerStmt());
    }
    if(astForStmt->hasConditionExpr()){
        temps = exprIR.initiateTemporaries(astForStmt->getConditionExpr());
        irForCondition = exprIR.transformExpr(astForStmt->getConditionExpr());
    }
    if(astForStmt->hasIncrementerStmt()){
        irForInc = transformAssignStmt(astForStmt->getIncrementerStmt());
    }

    irForStmt->setForStmt(
        std::move(irForInit), 
        std::move(irForCondition), 
        std::move(irForInc), 
        transformStmt(astForStmt->getStmt()), 
        std::move(temps)
    );

    return irForStmt;
}

std::unique_ptr<IR::node::IRDoWhileStmt> 
IR::StatementIntermediateRepresentation::transformDoWhileStmt(
    const AST::node::ASTDoWhileStmt* astDowhileStmt
){
    std::unique_ptr<IR::node::IRDoWhileStmt> irDowhileStmt{ 
        std::make_unique<IR::node::IRDoWhileStmt>() 
    };

    auto temps{ exprIR.initiateTemporaries(astDowhileStmt->getConditionExpr()) };
    irDowhileStmt->setDoWhileStmt(
        exprIR.transformExpr(astDowhileStmt->getConditionExpr()), 
        transformStmt(astDowhileStmt->getStmt()), std::move(temps)
    );

    return irDowhileStmt;
}

std::unique_ptr<IR::node::IRSwitchStmt> 
IR::StatementIntermediateRepresentation::transformSwitchStmt(
    const AST::node::ASTSwitchStmt* astSwitchStmt
){
    std::unique_ptr<IR::node::IRSwitchStmt> irSwitchStmt{ 
        std::make_unique<IR::node::IRSwitchStmt>() 
    };

    irSwitchStmt->setVariableIdExpr(
        exprIR.transformIdExpr(
            astSwitchStmt->getVariableIdExpr()
        )
    );

    for(const auto& _swCase : astSwitchStmt->getCaseStmts()){
        irSwitchStmt->addCaseStmt(transformCaseStmt(_swCase.get()));
    }
    
    if(astSwitchStmt->hasDefaultStmt()){
        irSwitchStmt->setDefaultStmt(
            transformDefaultStmt(
                astSwitchStmt->getDefaultStmt()
            )
        );
    }

    return irSwitchStmt;
}

std::unique_ptr<IR::node::IRCaseStmt> 
IR::StatementIntermediateRepresentation::transformCaseStmt(
    const AST::node::ASTCaseStmt* astCaseStmt
){
    std::unique_ptr<IR::node::IRCaseStmt> irCaseStmt{ std::make_unique<IR::node::IRCaseStmt>() };
    irCaseStmt->setCase(
        exprIR.transformLiteralExpr(astCaseStmt->getLiteralExpr()), 
        transformSwitchBlockStmt(astCaseStmt->getSwitchBlockStmt()), 
        astCaseStmt->hasBreakStmt()
    );

    return irCaseStmt;
}

std::unique_ptr<IR::node::IRDefaultStmt> 
IR::StatementIntermediateRepresentation::transformDefaultStmt(
    const AST::node::ASTDefaultStmt* astDefaultStmt
){
    std::unique_ptr<IR::node::IRDefaultStmt> irDefaultStmt{ std::make_unique<IR::node::IRDefaultStmt>() };
    irDefaultStmt->setSwitchBlock(
        transformSwitchBlockStmt(
            astDefaultStmt->getSwitchBlockStmt()
        )
    );

    return irDefaultStmt;
}

std::unique_ptr<IR::node::IRSwitchBlockStmt> 
IR::StatementIntermediateRepresentation::transformSwitchBlockStmt(
    const AST::node::ASTSwitchBlockStmt* astSwitchBlockStmt
){
    std::unique_ptr<IR::node::IRSwitchBlockStmt> irSwitchBlockStmt{ 
        std::make_unique<IR::node::IRSwitchBlockStmt>() 
    };
    
    for(const auto& astStmt : astSwitchBlockStmt->getStmts()){
        irSwitchBlockStmt->addStmt(transformStmt(astStmt.get()));

        // ignore all statements after return statement
        if(astStmt->getNodeType() == AST::defs::ASTNodeType::RETURN_STMT){
            break;
        }
    }
    
    return irSwitchBlockStmt;
}