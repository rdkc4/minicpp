#include "ir_dumper.hpp"

#include <format>

#include "../defs/defs.hpp"
#include "indent_guard.hpp"

IR::dump::IRDumper::IRDumper(std::ostream& out) : out{ out }, indent{ 0 } {}

void IR::dump::IRDumper::visit(IR::node::IRProgram* program){
    dumpNode(program);

    dumpLibs(program);

    IndentGuard programGuard{indent};
    for(const auto& function : program->getFunctions()){
        function->accept(*this);
    }
}

void IR::dump::IRDumper::visit(IR::node::IRFunction* function){
    dumpNode(function, std::format(
        " | {} {} | req mem: {}", 
        typeToStr(function->getType()), function->getFunctionName(), function->getRequiredMemory()
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

void IR::dump::IRDumper::visit(IR::node::IRParameter* parameter){
    dumpNode(parameter, std::format(
        " | {} {}", 
        typeToStr(parameter->getType()), parameter->getParameterName()
    ));
}

void IR::dump::IRDumper::visit(IR::node::IRVariableDeclStmt* variableDecl){
    dumpNode(variableDecl, std::format(
        " | {} {} {}", 
        typeToStr(variableDecl->getType()), variableDecl->getVarName(), variableDecl->getValue()
    ));

    IndentGuard variableGuard{indent};
    if(variableDecl->hasTemporaryExpr()){
        variableDecl->getTemporaryExpr()->accept(*this);
    }
    if(variableDecl->hasAssignExpr()){
        variableDecl->getAssignExpr()->accept(*this);
    }
}

void IR::dump::IRDumper::visit(IR::node::IRAssignStmt* assignStmt){
    dumpNode(assignStmt);

    IndentGuard assignGuard{indent};
    assignStmt->getVariableIdExpr()->accept(*this);
    if(assignStmt->hasTemporaryExpr()){
        assignStmt->getTemporaryExpr()->accept(*this);
    }
    assignStmt->getAssignedExpr()->accept(*this);
}

void IR::dump::IRDumper::visit(IR::node::IRCompoundStmt* compoundStmt){
    dumpNode(compoundStmt);

    IndentGuard compoundGuard{indent};
    for(const auto& stmt : compoundStmt->getStmts()){
        stmt->accept(*this);
    }
}

void IR::dump::IRDumper::visit(IR::node::IRForStmt* forStmt){
    dumpNode(forStmt);

    IndentGuard forGuard{indent};
    if(forStmt->hasTemporaryExpr()){
        forStmt->getTemporaryExpr()->accept(*this);
    }
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

void IR::dump::IRDumper::visit(IR::node::IRFunctionCallStmt* callStmt){
    dumpNode(callStmt);

    IndentGuard callGuard{indent};
    callStmt->getFunctionCallExpr()->accept(*this);
}

void IR::dump::IRDumper::visit(IR::node::IRIfStmt* ifStmt){
    dumpNode(ifStmt);

    const auto& conditions{ ifStmt->getConditionExprs() };
    const auto& statements{ ifStmt->getStmts() };
    const auto& temporaries{ ifStmt->getTemporaryExprs() };

    IndentGuard ifGuard{indent};
    for(size_t i{0}; i < conditions.size(); ++i){
        if(temporaries[i]){
            temporaries[i]->accept(*this);
        }
        conditions[i]->accept(*this);
        statements[i]->accept(*this);
    }
    if(ifStmt->hasElseStmt()){
        statements.back()->accept(*this);
    }
}

void IR::dump::IRDumper::visit(IR::node::IRReturnStmt* returnStmt){
    dumpNode(returnStmt);

    IndentGuard returnGuard{indent};
    if(returnStmt->hasTemporaryExpr()){
        returnStmt->getTemporaryExpr()->accept(*this);
    }
    if(returnStmt->hasReturnValue()){
        returnStmt->getReturnExpr()->accept(*this);
    }
}

void IR::dump::IRDumper::visit(IR::node::IRWhileStmt* whileStmt){
    dumpNode(whileStmt);

    IndentGuard whileGuard{indent};
    if(whileStmt->hasTemporaryExpr()){
        whileStmt->getTemporaryExpr()->accept(*this);
    }
    whileStmt->getConditionExpr()->accept(*this);
    whileStmt->getStmt()->accept(*this);
}

void IR::dump::IRDumper::visit(IR::node::IRDoWhileStmt* dowhileStmt){
    dumpNode(dowhileStmt);

    IndentGuard dowhileGuard{indent};
    if(dowhileStmt->hasTemporaryExpr()){
        dowhileStmt->getTemporaryExpr()->accept(*this);
    }
    dowhileStmt->getConditionExpr()->accept(*this);
    dowhileStmt->getStmt()->accept(*this);
}

void IR::dump::IRDumper::visit(IR::node::IRSwitchStmt* switchStmt){
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

void IR::dump::IRDumper::visit(IR::node::IRCaseStmt* caseStmt){
    dumpNode(caseStmt);

    IndentGuard caseGuard{indent};
    caseStmt->getLiteralExpr()->accept(*this);
    caseStmt->getSwitchBlockStmt()->accept(*this);
    if(caseStmt->hasBreakStmt()){
        dumpNode("BREAK");
    }
}

void IR::dump::IRDumper::visit(IR::node::IRDefaultStmt* defaultStmt){
    dumpNode(defaultStmt);

    IndentGuard defaultGuard{indent};
    defaultStmt->getSwitchBlockStmt()->accept(*this);
}

void IR::dump::IRDumper::visit(IR::node::IRSwitchBlockStmt* switchBlockStmt){
    dumpNode(switchBlockStmt);

    IndentGuard switchBlockGuard{indent};
    for(const auto& stmt : switchBlockStmt->getStmts()){
        stmt->accept(*this);
    }
}

void IR::dump::IRDumper::visit(IR::node::IRBinaryExpr* binaryExpr){
    dumpNode(binaryExpr, std::format(" | {}", operatorToStr(binaryExpr->getOperator())));

    IndentGuard binaryExprGuard{indent};
    binaryExpr->getLeftOperandExpr()->accept(*this);
    binaryExpr->getRightOperandExpr()->accept(*this);
}

void IR::dump::IRDumper::visit(IR::node::IRFunctionCallExpr* callExpr){
    dumpNode(callExpr, std::format(" | {}", callExpr->getCallName()));

    const auto& arguments{ callExpr->getArguments() };
    const auto& temporaries{ callExpr->getTemporaryExprs() };

    IndentGuard callGuard{indent};
    for(size_t i{0}; i < arguments.size(); ++i){
        if(temporaries[i]){
            temporaries[i]->accept(*this);
        }
        arguments[i]->accept(*this);
    }
}

void IR::dump::IRDumper::visit(IR::node::IRIdExpr* idExpr){
    dumpNode(idExpr, std::format(
        " | {} {}", 
        typeToStr(idExpr->getType()), idExpr->getIdName()
    ));
}

void IR::dump::IRDumper::visit(IR::node::IRLiteralExpr* literalExpr){
    dumpNode(literalExpr, std::format(
        " | {} {}",
        typeToStr(literalExpr->getType()), literalExpr->getValue()
    ));
}

void IR::dump::IRDumper::visit(IR::node::IRTemporaryExpr* tempExpr){
    dumpNode(tempExpr);

    const auto& names{ tempExpr->getTemporaryNames() };
    const auto& exprs{ tempExpr->getTemporaryExprs() };

    IndentGuard tempGuard{indent};
    for(size_t i{0}; i < exprs.size(); ++i){
        dumpNode(names[i]);
        IndentGuard exprGuard{indent};
        exprs[i]->accept(*this);
    }
}

void IR::dump::IRDumper::dumpIndent(){
    out << std::string(indent * 2, ' ');
}

void IR::dump::IRDumper::dumpNode(const IR::node::IRNode* node, std::string_view details){
    dumpIndent();
    out << "|-> " << IR::defs::irNodeToStr(node->getNodeType()) << details << "\n";
}

void IR::dump::IRDumper::dumpNode(std::string_view nodeLabel){
    dumpIndent();
    out << "|-> " << nodeLabel << "\n";
}

void IR::dump::IRDumper::dumpLibs(const IR::node::IRProgram* program){
    IndentGuard libsGuard{indent};
    dumpNode("LIBS");

    IndentGuard libGuard{indent};
    for(const auto& lib : program->getLinkedLibs()){
        dumpNode(lib);
    }
}