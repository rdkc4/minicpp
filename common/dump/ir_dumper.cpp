#include "ir_dumper.hpp"

#include <format>

#include "../defs/defs.hpp"

IRDumper::IRDumper(std::ostream& out) : out{ out }, indent{ 0 } {}

void IRDumper::visit(IRProgram* program){
    dumpNode(program, std::format(" | libs: {}", program->getLinkedLibs()));

    ++indent;
    for(const auto& function : program->getFunctions()){
        function->accept(*this);
    }
    --indent;
}

void IRDumper::visit(IRFunction* function){
    dumpNode(function, std::format(
        " | {} {} | req mem: {}", 
        typeToString.at(function->getType()), function->getFunctionName(), function->getRequiredMemory()
    ));

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

    ++indent;
    if(variableDecl->hasTemporaries()){
        variableDecl->getTemporaries()->accept(*this);
    }

    if(variableDecl->hasAssign()){
        variableDecl->getAssign()->accept(*this);
    }
    --indent;
}

void IRDumper::visit(IRAssignStmt* assignStmt){
    dumpNode(assignStmt);

    ++indent;
    assignStmt->getVariable()->accept(*this);
    if(assignStmt->hasTemporaries()){
        assignStmt->getTemporaries()->accept(*this);
    }
    assignStmt->getExp()->accept(*this);
    --indent;
}

void IRDumper::visit(IRCompoundStmt* compoundStmt){
    dumpNode(compoundStmt);

    ++indent;
    for(const auto& stmt : compoundStmt->getStatements()){
        stmt->accept(*this);
    }
    --indent;
}

void IRDumper::visit(IRForStmt* forStmt){
    dumpNode(forStmt);

    ++indent;
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
    --indent;
}

void IRDumper::visit(IRFunctionCallStmt* callStmt){
    dumpNode(callStmt);

    ++indent;
    callStmt->getFunctionCall()->accept(*this);
    --indent;
}

void IRDumper::visit(IRIfStmt* ifStmt){
    dumpNode(ifStmt);

    const auto& conditions = ifStmt->getConditions();
    const auto& statements = ifStmt->getStatements();
    const auto& temporaries = ifStmt->getTemporaries();

    ++indent;
    for(size_t i = 0; i < conditions.size(); ++i){
        if(temporaries.at(i)){
            temporaries.at(i)->accept(*this);
        }
        conditions.at(i)->accept(*this);
        statements.at(i)->accept(*this);
    }
    if(ifStmt->hasElse()){
        statements.back()->accept(*this);
    }
    --indent;
}

void IRDumper::visit(IRReturnStmt* returnStmt){
    dumpNode(returnStmt);

    ++indent;
    if(returnStmt->hasTemporaries()){
        returnStmt->getTemporaries()->accept(*this);
    }
    if(returnStmt->returns()){
        returnStmt->getExp()->accept(*this);
    }
    --indent;
}

void IRDumper::visit(IRWhileStmt* whileStmt){
    dumpNode(whileStmt);

    ++indent;
    if(whileStmt->hasTemporaries()){
        whileStmt->getTemporaries()->accept(*this);
    }
    whileStmt->getCondition()->accept(*this);
    whileStmt->getStatement()->accept(*this);
    --indent;
}

void IRDumper::visit(IRDoWhileStmt* dowhileStmt){
    dumpNode(dowhileStmt);

    ++indent;
    if(dowhileStmt->hasTemporaries()){
        dowhileStmt->getTemporaries()->accept(*this);
    }
    dowhileStmt->getCondition()->accept(*this);
    dowhileStmt->getStatement()->accept(*this);
    --indent;
}

void IRDumper::visit(IRSwitchStmt* switchStmt){
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

void IRDumper::visit(IRCaseStmt* caseStmt){
    dumpNode(caseStmt);

    ++indent;
    caseStmt->getLiteral()->accept(*this);
    caseStmt->getSwitchBlock()->accept(*this);
    if(caseStmt->hasBreak()){
        dumpNode("BREAK");
    }
    --indent;
}

void IRDumper::visit(IRDefaultStmt* defaultStmt){
    dumpNode(defaultStmt);

    ++indent;
    defaultStmt->getSwitchBlock()->accept(*this);
    --indent;
}

void IRDumper::visit(IRSwitchBlockStmt* switchBlockStmt){
    dumpNode(switchBlockStmt);

    ++indent;
    for(const auto& stmt : switchBlockStmt->getStatements()){
        stmt->accept(*this);
    }
    --indent;
}

void IRDumper::visit(IRBinaryExpr* binaryExpr){
    dumpNode(binaryExpr, std::format(" | {}", operatorToString.at(binaryExpr->getOperator())));

    ++indent;
    binaryExpr->getLeftOperand()->accept(*this);
    binaryExpr->getRightOperand()->accept(*this);
    --indent;
}

void IRDumper::visit(IRFunctionCallExpr* callExpr){
    dumpNode(callExpr, std::format(" | {}", callExpr->getCallName()));

    const auto& arguments = callExpr->getArguments();
    const auto& temporaries = callExpr->getTemporaries();

    ++indent;
    for(size_t i = 0; i < arguments.size(); ++i){
        if(temporaries.at(i)){
            temporaries.at(i)->accept(*this);
        }
        arguments.at(i)->accept(*this);
    }
    --indent;
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

    ++indent;
    for(size_t i = 0; i < exprs.size(); ++i){
        dumpNode(names.at(i));
        ++indent;
        exprs.at(i)->accept(*this);
        --indent;
    }
    --indent;
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