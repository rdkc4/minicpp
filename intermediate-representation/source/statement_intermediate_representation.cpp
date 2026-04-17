#include "../statement_intermediate_representation.hpp"

#include <cassert>
#include <utility>

IR::StatementIntermediateRepresentation::StatementIntermediateRepresentation(
    ir::IRFunctionContext& context
) : exprIR{ context } {}

std::unique_ptr<ir::IRStmt> 
IR::StatementIntermediateRepresentation::transformStmt(const syntax::ast::ASTStmt* astStmt){
    switch(astStmt->getNodeType()){
        case syntax::ast::ASTNodeType::VARIABLE_DECL_STMT:
            return transformVariableDeclStmt(static_cast<const syntax::ast::ASTVariableDeclStmt*>(astStmt));

        case syntax::ast::ASTNodeType::IF_STMT:
            return transformIfStmt(static_cast<const syntax::ast::ASTIfStmt*>(astStmt));

        case syntax::ast::ASTNodeType::COMPOUND_STMT:
            return transformCompoundStmt(static_cast<const syntax::ast::ASTCompoundStmt*>(astStmt));

        case syntax::ast::ASTNodeType::ASSIGN_STMT:
            return transformAssignStmt(static_cast<const syntax::ast::ASTAssignStmt*>(astStmt));

        case syntax::ast::ASTNodeType::RETURN_STMT:
            return transformReturnStmt(static_cast<const syntax::ast::ASTReturnStmt*>(astStmt));

        case syntax::ast::ASTNodeType::WHILE_STMT:
            return transformWhileStmt(static_cast<const syntax::ast::ASTWhileStmt*>(astStmt));

        case syntax::ast::ASTNodeType::FOR_STMT:
            return transformForStmt(static_cast<const syntax::ast::ASTForStmt*>(astStmt));

        case syntax::ast::ASTNodeType::DOWHILE_STMT:
            return transformDoWhileStmt(static_cast<const syntax::ast::ASTDoWhileStmt*>(astStmt));

        case syntax::ast::ASTNodeType::SWITCH_STMT:
            return transformSwitchStmt(static_cast<const syntax::ast::ASTSwitchStmt*>(astStmt));

        case syntax::ast::ASTNodeType::FUNCTION_CALL_STMT:
            return transformFunctionCallStmt(static_cast<const syntax::ast::ASTFunctionCallStmt*>(astStmt));

        default:
            std::unreachable();
    }
}

std::unique_ptr<ir::IRVariableDeclStmt> 
IR::StatementIntermediateRepresentation::transformVariableDeclStmt(
    const syntax::ast::ASTVariableDeclStmt* astVariableDecl
){
    std::unique_ptr<ir::IRVariableDeclStmt> irVariableDecl{ 
        std::make_unique<ir::IRVariableDeclStmt>(
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

std::unique_ptr<ir::IRIfStmt> 
IR::StatementIntermediateRepresentation::transformIfStmt(const syntax::ast::ASTIfStmt* astIfStmt){
    std::unique_ptr<ir::IRIfStmt> irIfStmt{ 
        std::make_unique<ir::IRIfStmt>() 
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

std::unique_ptr<ir::IRCompoundStmt> 
IR::StatementIntermediateRepresentation::transformCompoundStmt(
    const syntax::ast::ASTCompoundStmt* astCompoundStmt
){
    std::unique_ptr<ir::IRCompoundStmt> irCompoundStmt{ 
        std::make_unique<ir::IRCompoundStmt>() 
    };

    for(const auto& astStmt : astCompoundStmt->getStmts()){
        irCompoundStmt->addStmt(transformStmt(astStmt.get()));

        // ignore all statements after return statement
        if(astStmt->getNodeType() == syntax::ast::ASTNodeType::RETURN_STMT){
            break;
        }
    }
    return irCompoundStmt;
}

std::unique_ptr<ir::IRAssignStmt> 
IR::StatementIntermediateRepresentation::transformAssignStmt(
    const syntax::ast::ASTAssignStmt* astAssignStmt
){
    std::unique_ptr<ir::IRAssignStmt> irAssignStmt{ 
        std::make_unique<ir::IRAssignStmt>() 
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

std::unique_ptr<ir::IRReturnStmt> 
IR::StatementIntermediateRepresentation::transformReturnStmt(
    const syntax::ast::ASTReturnStmt* astReturnStmt
){
    std::unique_ptr<ir::IRReturnStmt> irReturnStmt{ 
        std::make_unique<ir::IRReturnStmt>() 
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

std::unique_ptr<ir::IRFunctionCallStmt> 
IR::StatementIntermediateRepresentation::transformFunctionCallStmt(
    const syntax::ast::ASTFunctionCallStmt* astCallStmt
){
    std::unique_ptr<ir::IRFunctionCallStmt> irCallStmt{ 
        std::make_unique<ir::IRFunctionCallStmt>() 
    };

    irCallStmt->setFunctionCallStmt(
        exprIR.transformFunctionCallExpr(
            astCallStmt->getFunctionCallExpr()
        )
    );

    return irCallStmt;
}

std::unique_ptr<ir::IRWhileStmt> 
IR::StatementIntermediateRepresentation::transformWhileStmt(
    const syntax::ast::ASTWhileStmt* astWhileStmt
){
    std::unique_ptr<ir::IRWhileStmt> irWhileStmt{ 
        std::make_unique<ir::IRWhileStmt>() 
    };

    auto temps{ exprIR.initiateTemporaries(astWhileStmt->getConditionExpr()) };
    irWhileStmt->setWhileStmt(
        exprIR.transformExpr(astWhileStmt->getConditionExpr()), 
        transformStmt(astWhileStmt->getStmt()), 
        std::move(temps)
    );

    return irWhileStmt;
}

std::unique_ptr<ir::IRForStmt> 
IR::StatementIntermediateRepresentation::transformForStmt(
    const syntax::ast::ASTForStmt* astForStmt
){
    std::unique_ptr<ir::IRForStmt> irForStmt{ 
        std::make_unique<ir::IRForStmt>() 
    };
    
    std::unique_ptr<ir::IRAssignStmt> irForInit{ nullptr }, irForInc{ nullptr };
    std::unique_ptr<ir::IRExpr> irForCondition{ nullptr };
    std::unique_ptr<ir::IRTemporaryExpr> temps{ nullptr };

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

std::unique_ptr<ir::IRDoWhileStmt> 
IR::StatementIntermediateRepresentation::transformDoWhileStmt(
    const syntax::ast::ASTDoWhileStmt* astDowhileStmt
){
    std::unique_ptr<ir::IRDoWhileStmt> irDowhileStmt{ 
        std::make_unique<ir::IRDoWhileStmt>() 
    };

    auto temps{ exprIR.initiateTemporaries(astDowhileStmt->getConditionExpr()) };
    irDowhileStmt->setDoWhileStmt(
        exprIR.transformExpr(astDowhileStmt->getConditionExpr()), 
        transformStmt(astDowhileStmt->getStmt()), std::move(temps)
    );

    return irDowhileStmt;
}

std::unique_ptr<ir::IRSwitchStmt> 
IR::StatementIntermediateRepresentation::transformSwitchStmt(
    const syntax::ast::ASTSwitchStmt* astSwitchStmt
){
    std::unique_ptr<ir::IRSwitchStmt> irSwitchStmt{ 
        std::make_unique<ir::IRSwitchStmt>() 
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

std::unique_ptr<ir::IRCaseStmt> 
IR::StatementIntermediateRepresentation::transformCaseStmt(
    const syntax::ast::ASTCaseStmt* astCaseStmt
){
    std::unique_ptr<ir::IRCaseStmt> irCaseStmt{ std::make_unique<ir::IRCaseStmt>() };
    irCaseStmt->setCase(
        exprIR.transformLiteralExpr(astCaseStmt->getLiteralExpr()), 
        transformSwitchBlockStmt(astCaseStmt->getSwitchBlockStmt()), 
        astCaseStmt->hasBreakStmt()
    );

    return irCaseStmt;
}

std::unique_ptr<ir::IRDefaultStmt> 
IR::StatementIntermediateRepresentation::transformDefaultStmt(
    const syntax::ast::ASTDefaultStmt* astDefaultStmt
){
    std::unique_ptr<ir::IRDefaultStmt> irDefaultStmt{ std::make_unique<ir::IRDefaultStmt>() };
    irDefaultStmt->setSwitchBlock(
        transformSwitchBlockStmt(
            astDefaultStmt->getSwitchBlockStmt()
        )
    );

    return irDefaultStmt;
}

std::unique_ptr<ir::IRSwitchBlockStmt> 
IR::StatementIntermediateRepresentation::transformSwitchBlockStmt(
    const syntax::ast::ASTSwitchBlockStmt* astSwitchBlockStmt
){
    std::unique_ptr<ir::IRSwitchBlockStmt> irSwitchBlockStmt{ 
        std::make_unique<ir::IRSwitchBlockStmt>() 
    };
    
    for(const auto& astStmt : astSwitchBlockStmt->getStmts()){
        irSwitchBlockStmt->addStmt(transformStmt(astStmt.get()));

        // ignore all statements after return statement
        if(astStmt->getNodeType() == syntax::ast::ASTNodeType::RETURN_STMT){
            break;
        }
    }
    
    return irSwitchBlockStmt;
}