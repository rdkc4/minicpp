#include "../statement_analyzer.hpp"
#include "../function_analyzer.hpp"

#include <format>
#include <utility>

StatementAnalyzer::StatementAnalyzer(ScopeManager& scopeManager) : globalScopeManager{ scopeManager }, expressionAnalyzer{ scopeManager } {}

void StatementAnalyzer::checkStmt(const ASTStmt* stmt){
    switch(stmt->getNodeType()){
        case ASTNodeType::VARIABLE:
            checkVariableDeclStmt(static_cast<const ASTVariableDeclStmt*>(stmt));
            break;
        case ASTNodeType::IF_STATEMENT:
            checkIfStmt(static_cast<const ASTIfStmt*>(stmt));
            break;
        case ASTNodeType::WHILE_STATEMENT:
            checkWhileStmt(static_cast<const ASTWhileStmt*>(stmt));
            break;
        case ASTNodeType::FOR_STATEMENT:
            checkForStmt(static_cast<const ASTForStmt*>(stmt));
            break;
        case ASTNodeType::ASSIGNMENT_STATEMENT:
            checkAssignStmt(static_cast<const ASTAssignStmt*>(stmt));
            break;
        case ASTNodeType::COMPOUND_STATEMENT:
            checkCompoundStmt(static_cast<const ASTCompoundStmt*>(stmt));
            break;
        case ASTNodeType::RETURN_STATEMENT:
            checkReturnStmt(static_cast<const ASTReturnStmt*>(stmt));
            break;
        case ASTNodeType::DO_WHILE_STATEMENT:
            checkDoWhileStmt(static_cast<const ASTDoWhileStmt*>(stmt));
            break;
        case ASTNodeType::SWITCH_STATEMENT:
            checkSwitchStmt(static_cast<const ASTSwitchStmt*>(stmt));
            break;
        case ASTNodeType::FUNCTION_CALL_STATEMENT:
            checkFunctionCallStmt(static_cast<const ASTFunctionCallStmt*>(stmt));
            break;
        default:
            std::unreachable();
    }
}

AnalyzerThreadContext& StatementAnalyzer::getContext() noexcept {
    return FunctionAnalyzer::getContext();
}

void StatementAnalyzer::checkVariableDeclStmt(const ASTVariableDeclStmt* variableDecl){
    // variable type check
    AnalyzerThreadContext& analyzerContext = getContext();
    Type type{ variableDecl->getType() };
    if(type == Type::VOID || type == Type::NO_TYPE){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                variableDecl->getToken().line, variableDecl->getToken().column, typeToString.at(type), variableDecl->getToken().value)
        );
    }
    else if(type == Type::AUTO && !variableDecl->hasAssign()){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction failed '{} {}'", 
                variableDecl->getToken().line, variableDecl->getToken().column, typeToString.at(type), variableDecl->getToken().value)
        );
    }

    // variable redefinition check
    if(!analyzerContext.scopeManager->pushSymbol(Symbol{variableDecl->getToken().value, Kind::VAR, type})){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> variable redefined '{}'", 
                variableDecl->getToken().line, variableDecl->getToken().column, variableDecl->getToken().value)
        );
    }

    // direct initialization
    if(variableDecl->getAssign() != nullptr){
        expressionAnalyzer.checkNumericalExpr(variableDecl->getAssign());

        Type rtype{variableDecl->getAssign()->getType() };

        // variable initialization type check
        if(rtype != type && type != Type::AUTO && rtype != Type::NO_TYPE){ // rtype == no_type => error was caught in expression, no need to write it again
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid assignment statement - type mismatch: expected '{}', got '{}'", 
                    variableDecl->getToken().line, variableDecl->getToken().column, typeToString.at(type), typeToString.at(rtype))
            );
        }

        if(type == Type::AUTO){
            analyzerContext.scopeManager->getSymbol(variableDecl->getToken().value).setType(rtype);
        }
    }
}

void StatementAnalyzer::checkIfStmt(const ASTIfStmt* ifStmt){
    for(const auto& condition : ifStmt->getConditions()){
        expressionAnalyzer.checkNumericalExpr(condition.get());
    }
    // else statement is included
    for(const auto& stmt : ifStmt->getStatements()){
        checkStmt(stmt.get());
    }
}

void StatementAnalyzer::checkWhileStmt(const ASTWhileStmt* whileStmt){
    expressionAnalyzer.checkNumericalExpr(whileStmt->getCondition());
    checkStmt(whileStmt->getStatement());
}

// initializer, condition and incrementer are optional
void StatementAnalyzer::checkForStmt(const ASTForStmt* forStmt){
    if(forStmt->hasInitializer()){
        checkAssignStmt(forStmt->getInitializer());
    }
    if(forStmt->hasCondition()){
        expressionAnalyzer.checkNumericalExpr(forStmt->getCondition());
    }
    if(forStmt->hasIncrementer()){
        checkAssignStmt(forStmt->getIncrementer());
    }

    checkStmt(forStmt->getStatement());
}

void StatementAnalyzer::checkDoWhileStmt(const ASTDoWhileStmt* dowhileStmt){
    checkStmt(dowhileStmt->getStatement());
    expressionAnalyzer.checkNumericalExpr(dowhileStmt->getCondition());
}

void StatementAnalyzer::checkCompoundStmt(const ASTCompoundStmt* compoundStmt){
    // compound scope
    AnalyzerThreadContext& analyzerContext = getContext();
    analyzerContext.scopeManager->pushScope();
    for(const auto& stmt : compoundStmt->getStatements()){
        checkStmt(stmt.get());
    }
    analyzerContext.scopeManager->popScope();
}

void StatementAnalyzer::checkAssignStmt(const ASTAssignStmt* assignStmt){
    ASTIdExpr* variableExpr{ assignStmt->getVariable() };
    ASTExpr* valueExpr{ assignStmt->getExp() };

    // if variable is not defined, rvalue won't be analyzed
    if(!expressionAnalyzer.checkIDExpr(variableExpr)) return;

    expressionAnalyzer.checkNumericalExpr(valueExpr);
    Type rtype{ valueExpr->getType() };
    Type ltype{ variableExpr->getType() };

    // if numexp contains undefined element it will report the undefined variable, no point checking for type mismatch
    if(rtype == Type::NO_TYPE) return;

    AnalyzerThreadContext& analyzerContext = getContext();

    // assignment type check
    if(rtype != ltype && ltype != Type::AUTO){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid assignment statement - type mismatch: expected '{}', got '{}'", 
                assignStmt->getToken().line, assignStmt->getToken().column, typeToString.at(ltype), typeToString.at(rtype))
        );
    }

    if(ltype == Type::AUTO){
        analyzerContext.scopeManager->getSymbol(variableExpr->getToken().value).setType(rtype);
    }
}

void StatementAnalyzer::checkReturnStmt(const ASTReturnStmt* returnStmt){
    Type returnType{ returnStmt->returns() ? expressionAnalyzer.checkNumericalExprType(returnStmt->getExp()) : Type::VOID };
    
    // if numexp inside of a return statement contains undef var, it will report it, no point checking for type mismatch
    if(returnType == Type::NO_TYPE) return;
    
    AnalyzerThreadContext& analyzerContext = getContext();

    // return type check
    Type expectedReturnType{ globalScopeManager.getSymbol(analyzerContext.functionName).getType() };
    if(returnType != expectedReturnType){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid return statement - type mismatch: '{} {}' returns '{}'", 
                returnStmt->getToken().line, returnStmt->getToken().column, typeToString.at(expectedReturnType), analyzerContext.functionName, typeToString.at(returnType))
        );
    }
}

void StatementAnalyzer::checkFunctionCallStmt(const ASTFunctionCallStmt* callStmt){
    expressionAnalyzer.checkFunctionCallExpr(callStmt->getFunctionCall());
}

void StatementAnalyzer::checkSwitchStmt(const ASTSwitchStmt* switchStmt){
    // if id is not defined, switch is ignored, all cases will be ignored, including their statements 
    if(!expressionAnalyzer.checkIDExpr(switchStmt->getVariable())) return;
    
    AnalyzerThreadContext& analyzerContext = getContext();
    // case check
    if(switchStmt->getVariable()->getType() == Type::INT){
        checkCaseStmts<int>(switchStmt);
    }
    else if(switchStmt->getVariable()->getType() == Type::UNSIGNED){
        checkCaseStmts<unsigned>(switchStmt);
    }
    else{
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{}'", 
                switchStmt->getToken().line, switchStmt->getToken().column, switchStmt->getVariable()->getToken().value)
        );
    }
}