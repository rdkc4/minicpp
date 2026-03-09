#include "ir_dumper.hpp"

#include <format>

#include "../defs/defs.hpp"
#include "indent_guard.hpp"

IRDumper::IRDumper(std::ostream& out) : out{ out }, indent{ 0 } {}

void IRDumper::visit(IRProgram* program){
    dumpNode(program, std::format(" | libs: {}", program->getLinkedLibs()));

    IndentGuard programGuard{indent};
    for(const auto& function : program->getFunctions()){
        function->accept(*this);
    }
}

void IRDumper::visit(IRFunction* function){
    dumpNode(function, std::format(
        " | {} {} | req mem: {}", 
        typeToString.at(function->getType()), function->getFunctionName(), function->getRequiredMemory()
    ));

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

void IRDumper::visit(IRParameter* parameter){
    dumpNode(parameter, std::format(
        " | {} {}", 
        typeToString.at(parameter->getType()), parameter->getParName()
    ));
}

void IRDumper::visit(IRVariableDeclStmt* variableDecl){
    dumpNode(variableDecl, std::format(
        " | {} {} {}", 
        typeToString.at(variableDecl->getType()), variableDecl->getVarName(), variableDecl->getValue()
    ));

    IndentGuard variableGuard{indent};
    if(variableDecl->hasTemporaries()){
        variableDecl->getTemporaries()->accept(*this);
    }
    if(variableDecl->hasAssign()){
        variableDecl->getAssign()->accept(*this);
    }
}

void IRDumper::visit(IRAssignStmt* assignStmt){
    dumpNode(assignStmt);

    IndentGuard assignGuard{indent};
    assignStmt->getVariable()->accept(*this);
    if(assignStmt->hasTemporaries()){
        assignStmt->getTemporaries()->accept(*this);
    }
    assignStmt->getExp()->accept(*this);
}

void IRDumper::visit(IRCompoundStmt* compoundStmt){
    dumpNode(compoundStmt);

    IndentGuard compoundGuard{indent};
    for(const auto& stmt : compoundStmt->getStatements()){
        stmt->accept(*this);
    }
}

void IRDumper::visit(IRForStmt* forStmt){
    dumpNode(forStmt);

    IndentGuard forGuard{indent};
    if(forStmt->hasTemporaries()){
        forStmt->getTemporaries()->accept(*this);
    }
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

void IRDumper::visit(IRFunctionCallStmt* callStmt){
    dumpNode(callStmt);

    IndentGuard callGuard{indent};
    callStmt->getFunctionCall()->accept(*this);
}

void IRDumper::visit(IRIfStmt* ifStmt){
    dumpNode(ifStmt);

    const auto& conditions = ifStmt->getConditions();
    const auto& statements = ifStmt->getStatements();
    const auto& temporaries = ifStmt->getTemporaries();

    IndentGuard ifGuard{indent};
    for(size_t i = 0; i < conditions.size(); ++i){
        if(temporaries[i]){
            temporaries[i]->accept(*this);
        }
        conditions[i]->accept(*this);
        statements[i]->accept(*this);
    }
    if(ifStmt->hasElse()){
        statements.back()->accept(*this);
    }
}

void IRDumper::visit(IRReturnStmt* returnStmt){
    dumpNode(returnStmt);

    IndentGuard returnGuard{indent};
    if(returnStmt->hasTemporaries()){
        returnStmt->getTemporaries()->accept(*this);
    }
    if(returnStmt->returns()){
        returnStmt->getExp()->accept(*this);
    }
}

void IRDumper::visit(IRWhileStmt* whileStmt){
    dumpNode(whileStmt);

    IndentGuard whileGuard{indent};
    if(whileStmt->hasTemporaries()){
        whileStmt->getTemporaries()->accept(*this);
    }
    whileStmt->getCondition()->accept(*this);
    whileStmt->getStatement()->accept(*this);
}

void IRDumper::visit(IRDoWhileStmt* dowhileStmt){
    dumpNode(dowhileStmt);

    IndentGuard dowhileGuard{indent};
    if(dowhileStmt->hasTemporaries()){
        dowhileStmt->getTemporaries()->accept(*this);
    }
    dowhileStmt->getCondition()->accept(*this);
    dowhileStmt->getStatement()->accept(*this);
}

void IRDumper::visit(IRSwitchStmt* switchStmt){
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

void IRDumper::visit(IRCaseStmt* caseStmt){
    dumpNode(caseStmt);

    IndentGuard caseGuard{indent};
    caseStmt->getLiteral()->accept(*this);
    caseStmt->getSwitchBlock()->accept(*this);
    if(caseStmt->hasBreak()){
        dumpNode("BREAK");
    }
}

void IRDumper::visit(IRDefaultStmt* defaultStmt){
    dumpNode(defaultStmt);

    IndentGuard defaultGuard{indent};
    defaultStmt->getSwitchBlock()->accept(*this);
}

void IRDumper::visit(IRSwitchBlockStmt* switchBlockStmt){
    dumpNode(switchBlockStmt);

    IndentGuard switchBlockGuard{indent};
    for(const auto& stmt : switchBlockStmt->getStatements()){
        stmt->accept(*this);
    }
}

void IRDumper::visit(IRBinaryExpr* binaryExpr){
    dumpNode(binaryExpr, std::format(" | {}", operatorToString.at(binaryExpr->getOperator())));

    IndentGuard binaryExprGuard{indent};
    binaryExpr->getLeftOperand()->accept(*this);
    binaryExpr->getRightOperand()->accept(*this);
}

void IRDumper::visit(IRFunctionCallExpr* callExpr){
    dumpNode(callExpr, std::format(" | {}", callExpr->getCallName()));

    const auto& arguments = callExpr->getArguments();
    const auto& temporaries = callExpr->getTemporaries();

    IndentGuard callGuard{indent};
    for(size_t i = 0; i < arguments.size(); ++i){
        if(temporaries[i]){
            temporaries[i]->accept(*this);
        }
        arguments[i]->accept(*this);
    }
}

void IRDumper::visit(IRIdExpr* idExpr){
    dumpNode(idExpr, std::format(
        " | {} {}", 
        typeToString.at(idExpr->getType()), idExpr->getIdName()
    ));
}

void IRDumper::visit(IRLiteralExpr* literalExpr){
    dumpNode(literalExpr, std::format(
        " | {} {}",
        typeToString.at(literalExpr->getType()), literalExpr->getValue()
    ));
}

void IRDumper::visit(IRTemporaryExpr* tempExpr){
    dumpNode(tempExpr);

    const auto& names = tempExpr->getTemporaryNames();
    const auto& exprs = tempExpr->getTemporaries();

    IndentGuard tempGuard{indent};
    for(size_t i = 0; i < exprs.size(); ++i){
        dumpNode(names[i]);
        IndentGuard exprGuard{indent};
        exprs[i]->accept(*this);
    }
}

void IRDumper::dumpIndent(){
    out << std::string(indent * 2, ' ');
}

void IRDumper::dumpNode(const IRNode* node, std::string_view details){
    dumpIndent();
    out << "|-> " << irNodeToString.at(node->getNodeType()) << details << "\n";
}

void IRDumper::dumpNode(std::string_view nodeLabel){
    dumpIndent();
    out << "|-> " << nodeLabel << "\n";
}