#include "ast_dumper.hpp"

#include "indent_guard.hpp"

syntax::ast::ASTDumper::ASTDumper(std::ostream& out) : out{ out }, indent{ 0 } {}

void syntax::ast::ASTDumper::visit(syntax::ast::ASTProgram* program){
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

void syntax::ast::ASTDumper::visit(syntax::ast::ASTIncludeDir* lib){
    dumpNode(lib);
}

void syntax::ast::ASTDumper::visit(syntax::ast::ASTFunction* function){
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

void syntax::ast::ASTDumper::visit(syntax::ast::ASTParameter* parameter){
    dumpNode(parameter);
}

void syntax::ast::ASTDumper::visit(syntax::ast::ASTVariableDeclStmt* variableDecl){
    dumpNode(variableDecl);

    if(variableDecl->hasAssignExpr()){
        IndentGuard variableGuard{indent};
        variableDecl->getAssignExpr()->accept(*this);
    }
}

void syntax::ast::ASTDumper::visit(syntax::ast::ASTAssignStmt* assignStmt){
    dumpNode(assignStmt);
    
    IndentGuard assignGuard{indent};
    assignStmt->getVariableIdExpr()->accept(*this);
    assignStmt->getAssignedExpr()->accept(*this);
}

void syntax::ast::ASTDumper::visit(syntax::ast::ASTCompoundStmt* compoundStmt){
    dumpNode(compoundStmt);

    IndentGuard compoundGuard{indent};
    for(const auto& stmt : compoundStmt->getStmts()){
        stmt->accept(*this);
    }
}

void syntax::ast::ASTDumper::visit(syntax::ast::ASTForStmt* forStmt){
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

void syntax::ast::ASTDumper::visit(syntax::ast::ASTFunctionCallStmt* callStmt){
    dumpNode(callStmt);

    IndentGuard callGuard{indent};
    callStmt->getFunctionCallExpr()->accept(*this);
}

void syntax::ast::ASTDumper::visit(syntax::ast::ASTIfStmt* ifStmt){
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

void syntax::ast::ASTDumper::visit(syntax::ast::ASTReturnStmt* returnStmt){
    dumpNode(returnStmt);

    if(returnStmt->hasReturnExpr()){
        IndentGuard returnGuard{indent};
        returnStmt->getReturnExpr()->accept(*this);
    }
}

void syntax::ast::ASTDumper::visit(syntax::ast::ASTWhileStmt* whileStmt){
    dumpNode(whileStmt);

    IndentGuard whileGuard{indent};
    whileStmt->getConditionExpr()->accept(*this);
    whileStmt->getStmt()->accept(*this);
}

void syntax::ast::ASTDumper::visit(syntax::ast::ASTDoWhileStmt* dowhileStmt){
    dumpNode(dowhileStmt);

    IndentGuard dowhileGuard{indent};
    dowhileStmt->getConditionExpr()->accept(*this);
    dowhileStmt->getStmt()->accept(*this);
}

void syntax::ast::ASTDumper::visit(syntax::ast::ASTSwitchStmt* switchStmt){
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

void syntax::ast::ASTDumper::visit(syntax::ast::ASTCaseStmt* caseStmt){
    dumpNode(caseStmt);

    IndentGuard caseGuard{indent};
    caseStmt->getLiteralExpr()->accept(*this);
    caseStmt->getSwitchBlockStmt()->accept(*this);
    if(caseStmt->hasBreakStmt()){
        dumpNode("BREAK");
    }
}

void syntax::ast::ASTDumper::visit(syntax::ast::ASTDefaultStmt* defaultStmt){
    dumpNode(defaultStmt);

    IndentGuard defaultGuard{indent};
    defaultStmt->getSwitchBlockStmt()->accept(*this);
}

void syntax::ast::ASTDumper::visit(syntax::ast::ASTSwitchBlockStmt* switchBlockStmt){
    dumpNode(switchBlockStmt);

    IndentGuard switchBlockGuard{indent};
    for(const auto& stmt : switchBlockStmt->getStmts()){
        stmt->accept(*this);
    }
}

void syntax::ast::ASTDumper::visit(syntax::ast::ASTBinaryExpr* binaryExpr){
    dumpNode(binaryExpr);

    IndentGuard binaryExprGuard{indent};
    binaryExpr->getLeftOperandExpr()->accept(*this);
    binaryExpr->getRightOperandExpr()->accept(*this);
}

void syntax::ast::ASTDumper::visit(syntax::ast::ASTFunctionCallExpr* callExpr){
    dumpNode(callExpr);

    IndentGuard callGuard{indent};
    for(const auto& arg : callExpr->getArguments()){
        arg->accept(*this);
    }
}

void syntax::ast::ASTDumper::visit(syntax::ast::ASTIdExpr* idExpr){
    dumpNode(idExpr);
}

void syntax::ast::ASTDumper::visit(syntax::ast::ASTLiteralExpr* literalExpr){
    dumpNode(literalExpr);
}

void syntax::ast::ASTDumper::dumpIndent(){
    out << std::string(indent * 2, ' ');
}

void syntax::ast::ASTDumper::dumpNode(const syntax::ast::ASTNode* node){
    dumpIndent();
    out << "|-> " 
        << syntax::ast::astNodeTypeToStr(node->getNodeType()) 
        << " | '" << node->getToken().value 
        << "'\n";
}

void syntax::ast::ASTDumper::dumpNode(std::string_view nodeLabel){
    dumpIndent();
    out << "|-> " << nodeLabel << "\n";
}