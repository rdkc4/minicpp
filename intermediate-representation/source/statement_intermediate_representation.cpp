#include "../statement_intermediate_representation.hpp"

#include <cassert>
#include <utility>

std::unique_ptr<IRStmt> StatementIntermediateRepresentation::transformStmt(const ASTStmt* astStmt){
    switch(astStmt->getNodeType()){
        case ASTNodeType::VARIABLE:
            return transformVariableDeclStmt(static_cast<const ASTVariableDeclStmt*>(astStmt));
        case ASTNodeType::IF_STATEMENT:
            return transformIfStmt(static_cast<const ASTIfStmt*>(astStmt));
        case ASTNodeType::COMPOUND_STATEMENT:
            return transformCompoundStmt(static_cast<const ASTCompoundStmt*>(astStmt));
        case ASTNodeType::ASSIGNMENT_STATEMENT:
            return transformAssignStmt(static_cast<const ASTAssignStmt*>(astStmt));
        case ASTNodeType::RETURN_STATEMENT:
            return transformReturnStmt(static_cast<const ASTReturnStmt*>(astStmt));
        case ASTNodeType::WHILE_STATEMENT:
            return transformWhileStmt(static_cast<const ASTWhileStmt*>(astStmt));
        case ASTNodeType::FOR_STATEMENT:
            return transformForStmt(static_cast<const ASTForStmt*>(astStmt));
        case ASTNodeType::DO_WHILE_STATEMENT:
            return transformDoWhileStmt(static_cast<const ASTDoWhileStmt*>(astStmt));
        case ASTNodeType::SWITCH_STATEMENT:
            return transformSwitchStmt(static_cast<const ASTSwitchStmt*>(astStmt));
        case ASTNodeType::FUNCTION_CALL_STATEMENT:
            return transformFunctionCallStmt(static_cast<const ASTFunctionCallStmt*>(astStmt));
        default:
            std::unreachable();
    }
}

std::unique_ptr<IRVariableDeclStmt> StatementIntermediateRepresentation::transformVariableDeclStmt(const ASTVariableDeclStmt* astVariableDecl){
    std::unique_ptr<IRVariableDeclStmt> irVariableDecl = std::make_unique<IRVariableDeclStmt>(IRNodeType::VARIABLE, astVariableDecl->getToken().value, astVariableDecl->getType());
    if(astVariableDecl->hasAssign()){
        auto temps{ exprIR.initiateTemporaries(astVariableDecl->getAssign()) };
        irVariableDecl->setAssign(exprIR.transformNumericalExpr(astVariableDecl->getAssign()), std::move(temps));
    }
    return irVariableDecl;
}

std::unique_ptr<IRIfStmt> StatementIntermediateRepresentation::transformIfStmt(const ASTIfStmt* astIfStmt){
    std::unique_ptr<IRIfStmt> irIfStmt = std::make_unique<IRIfStmt>(IRNodeType::IF);
    const auto& astConds = astIfStmt->getConditions();
    const auto& astStmts = astIfStmt->getStatements();

    const size_t conditionCount = astConds.size();

    for(size_t i = 0; i < conditionCount; ++i){
        auto temps{ exprIR.initiateTemporaries(astConds[i].get()) };
        irIfStmt->addIf(exprIR.transformRelationalExpr(astConds[i].get()), transformStmt(astStmts[i].get()), std::move(temps));
    }

    if(conditionCount != astStmts.size()){
        irIfStmt->addElse(transformStmt(astStmts.back().get()));
    }

    return irIfStmt;
}

std::unique_ptr<IRCompoundStmt> StatementIntermediateRepresentation::transformCompoundStmt(const ASTCompoundStmt* astCompoundStmt){
    std::unique_ptr<IRCompoundStmt> irCompoundStmt = std::make_unique<IRCompoundStmt>(IRNodeType::COMPOUND);
    for(const auto& astStmt : astCompoundStmt->getStatements()){
        irCompoundStmt->addStatement(transformStmt(astStmt.get()));

        // ignore all statements after return statement
        if(astStmt->getNodeType() == ASTNodeType::RETURN_STATEMENT){
            break;
        }
    }
    return irCompoundStmt;
}

std::unique_ptr<IRAssignStmt> StatementIntermediateRepresentation::transformAssignStmt(const ASTAssignStmt* astAssignStmt){
    std::unique_ptr<IRAssignStmt> irAssignStmt = std::make_unique<IRAssignStmt>(IRNodeType::ASSIGN);

    // extracting function calls to temporary variables
    auto temps{ exprIR.initiateTemporaries(astAssignStmt->getExp()) };
    irAssignStmt->setAssignSt(exprIR.transformIdExpr(astAssignStmt->getVariable()), exprIR.transformNumericalExpr(astAssignStmt->getExp()), std::move(temps));

    return irAssignStmt;
}

std::unique_ptr<IRReturnStmt> StatementIntermediateRepresentation::transformReturnStmt(const ASTReturnStmt* astReturnStmt){
    std::unique_ptr<IRReturnStmt> irReturnStmt = std::make_unique<IRReturnStmt>(IRNodeType::RETURN);
    if(astReturnStmt->returns()){
        // extracting function calls to temporary variables
        auto temps{ exprIR.initiateTemporaries(astReturnStmt->getExp()) };

        irReturnStmt->setExp(exprIR.transformNumericalExpr(astReturnStmt->getExp()), std::move(temps));
    }
    return irReturnStmt;
}

std::unique_ptr<IRFunctionCallStmt> StatementIntermediateRepresentation::transformFunctionCallStmt(const ASTFunctionCallStmt* astCallStmt){
    std::unique_ptr<IRFunctionCallStmt> irCallStmt = std::make_unique<IRFunctionCallStmt>(IRNodeType::CALL);
    irCallStmt->setFunctionCallSt(exprIR.transformFunctionCallExpr(astCallStmt->getFunctionCall()));

    return irCallStmt;
}

std::unique_ptr<IRWhileStmt> StatementIntermediateRepresentation::transformWhileStmt(const ASTWhileStmt* astWhileStmt){
    std::unique_ptr<IRWhileStmt> irWhileStmt = std::make_unique<IRWhileStmt>(IRNodeType::WHILE);
    auto temps{ exprIR.initiateTemporaries(astWhileStmt->getCondition()) };
    irWhileStmt->setWhileSt(exprIR.transformRelationalExpr(astWhileStmt->getCondition()), transformStmt(astWhileStmt->getStatement()), std::move(temps));

    return irWhileStmt;
}

std::unique_ptr<IRForStmt> StatementIntermediateRepresentation::transformForStmt(const ASTForStmt* astForStmt){
    std::unique_ptr<IRForStmt> irForStmt = std::make_unique<IRForStmt>(IRNodeType::FOR);
    
    std::unique_ptr<IRAssignStmt> irForInit{ nullptr }, irForInc{ nullptr };
    std::unique_ptr<IRExpr> irForCondition{ nullptr };
    std::unique_ptr<IRTemporaryExpr> temps{ nullptr };

    if(astForStmt->hasInitializer()){
        irForInit = transformAssignStmt(astForStmt->getInitializer());
    }
    if(astForStmt->hasCondition()){
        temps = exprIR.initiateTemporaries(astForStmt->getCondition());
        irForCondition = exprIR.transformRelationalExpr(astForStmt->getCondition());
    }
    if(astForStmt->hasIncrementer()){
        irForInc = transformAssignStmt(astForStmt->getIncrementer());
    }

    irForStmt->setForSt(std::move(irForInit), std::move(irForCondition), std::move(irForInc), transformStmt(astForStmt->getStatement()), std::move(temps));

    return irForStmt;
}

std::unique_ptr<IRDoWhileStmt> StatementIntermediateRepresentation::transformDoWhileStmt(const ASTDoWhileStmt* astDowhileStmt){
    std::unique_ptr<IRDoWhileStmt> irDowhileStmt = std::make_unique<IRDoWhileStmt>(IRNodeType::DO_WHILE);
    auto temps{ exprIR.initiateTemporaries(astDowhileStmt->getCondition()) };
    irDowhileStmt->setDoWhileSt(exprIR.transformRelationalExpr(astDowhileStmt->getCondition()), transformStmt(astDowhileStmt->getStatement()), std::move(temps));

    return irDowhileStmt;
}

std::unique_ptr<IRSwitchStmt> StatementIntermediateRepresentation::transformSwitchStmt(const ASTSwitchStmt* astSwitchStmt){
    std::unique_ptr<IRSwitchStmt> irSwitchStmt = std::make_unique<IRSwitchStmt>(IRNodeType::SWITCH);

    irSwitchStmt->setVariable(exprIR.transformIdExpr(astSwitchStmt->getVariable()));

    for(const auto& _swCase : astSwitchStmt->getCases()){
        irSwitchStmt->addCase(transformCaseStmt(_swCase.get()));
    }
    
    if(astSwitchStmt->hasDefault()){
        irSwitchStmt->setDefault(transformDefaultStmt(astSwitchStmt->getDefault()));
    }

    return irSwitchStmt;
}

std::unique_ptr<IRCaseStmt> StatementIntermediateRepresentation::transformCaseStmt(const ASTCaseStmt* astCaseStmt){
    std::unique_ptr<IRCaseStmt> irCaseStmt = std::make_unique<IRCaseStmt>(IRNodeType::CASE);
    irCaseStmt->setCase(exprIR.transformLiteralExpr(astCaseStmt->getLiteral()), transformSwitchBlockStmt(astCaseStmt->getSwitchBlock()), astCaseStmt->hasBreak());

    return irCaseStmt;
}

std::unique_ptr<IRDefaultStmt> StatementIntermediateRepresentation::transformDefaultStmt(const ASTDefaultStmt* astDefaultStmt){
    std::unique_ptr<IRDefaultStmt> irDefaultStmt = std::make_unique<IRDefaultStmt>(IRNodeType::DEFAULT);
    irDefaultStmt->setSwitchBlock(transformSwitchBlockStmt(astDefaultStmt->getSwitchBlock()));

    return irDefaultStmt;
}

std::unique_ptr<IRSwitchBlockStmt> StatementIntermediateRepresentation::transformSwitchBlockStmt(const ASTSwitchBlockStmt* astSwitchBlockStmt){
    std::unique_ptr<IRSwitchBlockStmt> irSwitchBlockStmt = std::make_unique<IRSwitchBlockStmt>(IRNodeType::SWITCH_BLOCK);
    for(const auto& astStmt : astSwitchBlockStmt->getStatements()){
        irSwitchBlockStmt->addStatement(transformStmt(astStmt.get()));

        // ignore all statements after return statement
        if(astStmt->getNodeType() == ASTNodeType::RETURN_STATEMENT){
            break;
        }
    }
    
    return irSwitchBlockStmt;
}