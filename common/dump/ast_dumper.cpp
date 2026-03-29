#include "ast_dumper.hpp"

#include "indent_guard.hpp"

ASTDumper::ASTDumper(std::ostream& out) : out{ out }, indent{ 0 } {}

void ASTDumper::visit(ASTProgram* program){
    dumpNode(program);

    IndentGuard programGuard{indent};

    dumpNode("DIRECTIVES");
    {
        IndentGuard dirGuard{indent};
        for(const auto& dir : program->getDirs()){
            dir->accept(*this);
        }
    }

    dumpNode("FUNCTIONS");
    {
        IndentGuard functionsGuard{indent};
        for(const auto& func : program->getFunctions()){
            func->accept(*this);
        }
    }
}

void ASTDumper::visit(ASTIncludeDir* lib){
    dumpNode(lib);
}

void ASTDumper::visit(ASTFunction* function){
    dumpNode(function);

    IndentGuard functionGuard{indent};

    dumpNode("PARAMETERS");
    {
        IndentGuard paramGuard{indent};
        for(const auto& param : function->getParameters()){
            param->accept(*this);
        }
    }

    if(!function->isPredefined()){
        dumpNode("BODY");
        IndentGuard bodyGuard{indent};
        for(const auto& stmt : function->getBody()){
            stmt->accept(*this);
        }
    }
}

void ASTDumper::visit(ASTParameter* parameter){
    dumpNode(parameter);
}

void ASTDumper::visit(ASTVariableDeclStmt* variableDecl){
    dumpNode(variableDecl);

    if(variableDecl->hasAssignExpr()){
        IndentGuard variableGuard{indent};
        variableDecl->getAssignExpr()->accept(*this);
    }
}

void ASTDumper::visit(ASTAssignStmt* assignStmt){
    dumpNode(assignStmt);
    
    IndentGuard assignGuard{indent};
    assignStmt->getVariableIdExpr()->accept(*this);
    assignStmt->getAssignedExpr()->accept(*this);
}

void ASTDumper::visit(ASTCompoundStmt* compoundStmt){
    dumpNode(compoundStmt);

    IndentGuard compoundGuard{indent};
    for(const auto& stmt : compoundStmt->getStmts()){
        stmt->accept(*this);
    }
}

void ASTDumper::visit(ASTForStmt* forStmt){
    dumpNode(forStmt);

    IndentGuard forGuard{indent};
    if(forStmt->hasInitializerStmt()){
        forStmt->getInitializerStmt()->accept(*this);
    }
    if(forStmt->hasConditionExpr()){
        forStmt->getConditionExpr()->accept(*this);
    }
    if(forStmt->hasIncrementerStmt()){
        forStmt->getIncrementerStmt()->accept(*this);
    }
    forStmt->getStmt()->accept(*this);
}

void ASTDumper::visit(ASTFunctionCallStmt* callStmt){
    dumpNode(callStmt);

    IndentGuard callGuard{indent};
    callStmt->getFunctionCallExpr()->accept(*this);
}

void ASTDumper::visit(ASTIfStmt* ifStmt){
    dumpNode(ifStmt);

    IndentGuard ifGuard{indent};
    const auto& conditions{ ifStmt->getConditionExprs() };
    const auto& statements{ ifStmt->getStmts() };

    for(size_t i{0}; i < conditions.size(); ++i){
        conditions[i]->accept(*this);
        statements[i]->accept(*this);
    }
    if(ifStmt->hasElseStmt()){
        statements.back()->accept(*this);
    }
}

void ASTDumper::visit(ASTReturnStmt* returnStmt){
    dumpNode(returnStmt);

    if(returnStmt->hasReturnExpr()){
        IndentGuard returnGuard{indent};
        returnStmt->getReturnExpr()->accept(*this);
    }
}

void ASTDumper::visit(ASTWhileStmt* whileStmt){
    dumpNode(whileStmt);

    IndentGuard whileGuard{indent};
    whileStmt->getConditionExpr()->accept(*this);
    whileStmt->getStmt()->accept(*this);
}

void ASTDumper::visit(ASTDoWhileStmt* dowhileStmt){
    dumpNode(dowhileStmt);

    IndentGuard dowhileGuard{indent};
    dowhileStmt->getConditionExpr()->accept(*this);
    dowhileStmt->getStmt()->accept(*this);
}

void ASTDumper::visit(ASTSwitchStmt* switchStmt){
    dumpNode(switchStmt);

    IndentGuard switchGuard{indent};
    switchStmt->getVariableIdExpr()->accept(*this);
    for(const auto& caseStmt : switchStmt->getCaseStmts()){
        caseStmt->accept(*this);
    }
    if(switchStmt->hasDefaultStmt()){
        switchStmt->getDefaultStmt()->accept(*this);
    }
}

void ASTDumper::visit(ASTCaseStmt* caseStmt){
    dumpNode(caseStmt);

    IndentGuard caseGuard{indent};
    caseStmt->getLiteralExpr()->accept(*this);
    caseStmt->getSwitchBlockStmt()->accept(*this);
    if(caseStmt->hasBreakStmt()){
        dumpNode("BREAK");
    }
}

void ASTDumper::visit(ASTDefaultStmt* defaultStmt){
    dumpNode(defaultStmt);

    IndentGuard defaultGuard{indent};
    defaultStmt->getSwitchBlockStmt()->accept(*this);
}

void ASTDumper::visit(ASTSwitchBlockStmt* switchBlockStmt){
    dumpNode(switchBlockStmt);

    IndentGuard switchBlockGuard{indent};
    for(const auto& stmt : switchBlockStmt->getStmts()){
        stmt->accept(*this);
    }
}

void ASTDumper::visit(ASTBinaryExpr* binaryExpr){
    dumpNode(binaryExpr);

    IndentGuard binaryExprGuard{indent};
    binaryExpr->getLeftOperandExpr()->accept(*this);
    binaryExpr->getRightOperandExpr()->accept(*this);
}

void ASTDumper::visit(ASTFunctionCallExpr* callExpr){
    dumpNode(callExpr);

    IndentGuard callGuard{indent};
    for(const auto& arg : callExpr->getArguments()){
        arg->accept(*this);
    }
}

void ASTDumper::visit(ASTIdExpr* idExpr){
    dumpNode(idExpr);
}

void ASTDumper::visit(ASTLiteralExpr* literalExpr){
    dumpNode(literalExpr);
}

void ASTDumper::dumpIndent(){
    out << std::string(indent * 2, ' ');
}

void ASTDumper::dumpNode(const ASTNode* node){
    dumpIndent();
    out << "|-> " << astNodeTypeToString.at(node->getNodeType()) << " | '" << node->getToken().value << "'\n";
}

void ASTDumper::dumpNode(std::string_view nodeLabel){
    dumpIndent();
    out << "|-> " << nodeLabel << "\n";
}