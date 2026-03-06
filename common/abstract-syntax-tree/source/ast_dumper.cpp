#include "../ast_dumper.hpp"

ASTDumper::ASTDumper(std::ostream& out) : out{ out } {}

void ASTDumper::visit(ASTProgram* program){
    dumpNode(program);

    ++indent;

    dumpNode("DIRECTIVES");
    ++indent;
    for(const auto& dir : program->getDirectives()){
        dir->accept(*this);
    }
    --indent;

    dumpNode("FUNCTIONS");
    ++indent;
    for(const auto& func : program->getFunctions()){
        func->accept(*this);
    }
    --indent;

    --indent;
}

void ASTDumper::visit(ASTIncludeDir* lib){
    dumpNode(lib);
}

void ASTDumper::visit(ASTFunction* function){
    dumpNode(function);

    ++indent;

    dumpNode("PARAMETERS");
    ++indent;
    for(const auto& param : function->getParameters()){
        param->accept(*this);
    }
    --indent;

    if(!function->isPredefined()){
        dumpNode("BODY");
        ++indent;
        for(const auto& stmt : function->getBody()){
            stmt->accept(*this);
        }
        --indent;
    }

    --indent;
}

void ASTDumper::visit(ASTParameter* parameter){
    dumpNode(parameter);
}

void ASTDumper::visit(ASTVariableDeclStmt* variableDecl){
    dumpNode(variableDecl);

    if(variableDecl->hasAssign()){
        ++indent;
        variableDecl->getAssign()->accept(*this);
        --indent;
    }
}

void ASTDumper::visit(ASTAssignStmt* assignStmt){
    dumpNode(assignStmt);
    
    ++indent;
    assignStmt->getVariable()->accept(*this);
    assignStmt->getExp()->accept(*this);
    --indent;
}

void ASTDumper::visit(ASTCompoundStmt* compoundStmt){
    dumpNode(compoundStmt);

    ++indent;
    for(const auto& stmt : compoundStmt->getStatements()){
        stmt->accept(*this);
    }
    --indent;
}

void ASTDumper::visit(ASTForStmt* forStmt){
    dumpNode(forStmt);

    ++indent;
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
    --indent;
}

void ASTDumper::visit(ASTFunctionCallStmt* callStmt){
    dumpNode(callStmt);

    ++indent;
    callStmt->getFunctionCall()->accept(*this);
    --indent;
}

void ASTDumper::visit(ASTIfStmt* ifStmt){
    dumpNode(ifStmt);

    ++indent;
    auto& conditions = ifStmt->getConditions();
    auto& statements = ifStmt->getStatements();

    for(size_t i = 0; i < conditions.size(); ++i){
        conditions.at(i)->accept(*this);
        statements.at(i)->accept(*this);
    }
    if(ifStmt->hasElse()){
        statements.back()->accept(*this);
    }

    --indent;
}

void ASTDumper::visit(ASTReturnStmt* returnStmt){
    dumpNode(returnStmt);

    if(returnStmt->returns()){
        ++indent;
        returnStmt->getExp()->accept(*this);
        --indent;
    }
}

void ASTDumper::visit(ASTWhileStmt* whileStmt){
    dumpNode(whileStmt);

    ++indent;
    whileStmt->getCondition()->accept(*this);
    whileStmt->getStatement()->accept(*this);
    --indent;
}

void ASTDumper::visit(ASTDoWhileStmt* dowhileStmt){
    dumpNode(dowhileStmt);

    ++indent;
    dowhileStmt->getCondition()->accept(*this);
    dowhileStmt->getStatement()->accept(*this);
    --indent;
}

void ASTDumper::visit(ASTSwitchStmt* switchStmt){
    dumpNode(switchStmt);

    ++indent;
    switchStmt->getVariable()->accept(*this);
    for(const auto& caseStmt : switchStmt->getCases()){
        caseStmt->accept(*this);
    }
    if(switchStmt->hasDefault()){
        switchStmt->getDefault()->accept(*this);
    }
    --indent;
}

void ASTDumper::visit(ASTCaseStmt* caseStmt){
    dumpNode(caseStmt);

    ++indent;
    caseStmt->getLiteral()->accept(*this);
    caseStmt->getSwitchBlock()->accept(*this);
    --indent;
}

void ASTDumper::visit(ASTDefaultStmt* defaultStmt){
    dumpNode(defaultStmt);

    ++indent;
    defaultStmt->getSwitchBlock()->accept(*this);
    --indent;
}

void ASTDumper::visit(ASTSwitchBlockStmt* switchBlockStmt){
    dumpNode(switchBlockStmt);

    ++indent;
    for(const auto& stmt : switchBlockStmt->getStatements()){
        stmt->accept(*this);
    }
    --indent;
}

void ASTDumper::visit(ASTBinaryExpr* binaryExpr){
    dumpNode(binaryExpr);

    ++indent;
    binaryExpr->getLeftOperand()->accept(*this);
    binaryExpr->getRightOperand()->accept(*this);
    --indent;
}

void ASTDumper::visit(ASTFunctionCallExpr* callExpr){
    dumpNode(callExpr);

    ++indent;
    for(const auto& arg : callExpr->getArguments()){
        arg->accept(*this);
    }
    --indent;
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