#include "ast_dumper.hpp"

#include "indent_guard.hpp"

ASTDumper::ASTDumper(std::ostream& out) : out{ out }, indent{ 0 } {}

void ASTDumper::visit(ASTProgram* program){
    dumpNode(program);

    IndentGuard programGuard{indent};

    dumpNode("DIRECTIVES");
    {
        IndentGuard dirGuard{indent};
        for(const auto& dir : program->getDirectives()){
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

    if(variableDecl->hasAssign()){
        IndentGuard variableGuard{indent};
        variableDecl->getAssign()->accept(*this);
    }
}

void ASTDumper::visit(ASTAssignStmt* assignStmt){
    dumpNode(assignStmt);
    
    IndentGuard assignGuard{indent};
    assignStmt->getVariable()->accept(*this);
    assignStmt->getExp()->accept(*this);
}

void ASTDumper::visit(ASTCompoundStmt* compoundStmt){
    dumpNode(compoundStmt);

    IndentGuard compoundGuard{indent};
    for(const auto& stmt : compoundStmt->getStatements()){
        stmt->accept(*this);
    }
}

void ASTDumper::visit(ASTForStmt* forStmt){
    dumpNode(forStmt);

    IndentGuard forGuard{indent};
    if(forStmt->hasInitializer()){
        forStmt->getInitializer()->accept(*this);
    }
    if(forStmt->hasCondition()){
        forStmt->getCondition()->accept(*this);
    }
    if(forStmt->hasIncrementer()){
        forStmt->getIncrementer()->accept(*this);
    }
    forStmt->getStatement()->accept(*this);
}

void ASTDumper::visit(ASTFunctionCallStmt* callStmt){
    dumpNode(callStmt);

    IndentGuard callGuard{indent};
    callStmt->getFunctionCall()->accept(*this);
}

void ASTDumper::visit(ASTIfStmt* ifStmt){
    dumpNode(ifStmt);

    IndentGuard ifGuard{indent};
    const auto& conditions = ifStmt->getConditions();
    const auto& statements = ifStmt->getStatements();

    for(size_t i = 0; i < conditions.size(); ++i){
        conditions[i]->accept(*this);
        statements[i]->accept(*this);
    }
    if(ifStmt->hasElse()){
        statements.back()->accept(*this);
    }
}

void ASTDumper::visit(ASTReturnStmt* returnStmt){
    dumpNode(returnStmt);

    if(returnStmt->returns()){
        IndentGuard returnGuard{indent};
        returnStmt->getExp()->accept(*this);
    }
}

void ASTDumper::visit(ASTWhileStmt* whileStmt){
    dumpNode(whileStmt);

    IndentGuard whileGuard{indent};
    whileStmt->getCondition()->accept(*this);
    whileStmt->getStatement()->accept(*this);
}

void ASTDumper::visit(ASTDoWhileStmt* dowhileStmt){
    dumpNode(dowhileStmt);

    IndentGuard dowhileGuard{indent};
    dowhileStmt->getCondition()->accept(*this);
    dowhileStmt->getStatement()->accept(*this);
}

void ASTDumper::visit(ASTSwitchStmt* switchStmt){
    dumpNode(switchStmt);

    IndentGuard switchGuard{indent};
    switchStmt->getVariable()->accept(*this);
    for(const auto& caseStmt : switchStmt->getCases()){
        caseStmt->accept(*this);
    }
    if(switchStmt->hasDefault()){
        switchStmt->getDefault()->accept(*this);
    }
}

void ASTDumper::visit(ASTCaseStmt* caseStmt){
    dumpNode(caseStmt);

    IndentGuard caseGuard{indent};
    caseStmt->getLiteral()->accept(*this);
    caseStmt->getSwitchBlock()->accept(*this);
    if(caseStmt->hasBreak()){
        dumpNode("BREAK");
    }
}

void ASTDumper::visit(ASTDefaultStmt* defaultStmt){
    dumpNode(defaultStmt);

    IndentGuard defaultGuard{indent};
    defaultStmt->getSwitchBlock()->accept(*this);
}

void ASTDumper::visit(ASTSwitchBlockStmt* switchBlockStmt){
    dumpNode(switchBlockStmt);

    IndentGuard switchBlockGuard{indent};
    for(const auto& stmt : switchBlockStmt->getStatements()){
        stmt->accept(*this);
    }
}

void ASTDumper::visit(ASTBinaryExpr* binaryExpr){
    dumpNode(binaryExpr);

    IndentGuard binaryExprGuard{indent};
    binaryExpr->getLeftOperand()->accept(*this);
    binaryExpr->getRightOperand()->accept(*this);
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