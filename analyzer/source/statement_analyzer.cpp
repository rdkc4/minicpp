#include "../statement_analyzer.hpp"
#include "../function_analyzer.hpp"

#include <format>
#include <utility>

StatementAnalyzer::StatementAnalyzer(ScopeManager& scopeManager) : globalScopeManager{ scopeManager }, expressionAnalyzer{ scopeManager } {}

void StatementAnalyzer::checkStatement(const ASTStmt* _statement){
    switch(_statement->getNodeType()){
        case ASTNodeType::VARIABLE:
            checkVariable(static_cast<const ASTVariableDeclStmt*>(_statement));
            break;
        case ASTNodeType::IF_STATEMENT:
            checkIfStatement(static_cast<const ASTIfStmt*>(_statement));
            break;
        case ASTNodeType::WHILE_STATEMENT:
            checkWhileStatement(static_cast<const ASTWhileStmt*>(_statement));
            break;
        case ASTNodeType::FOR_STATEMENT:
            checkForStatement(static_cast<const ASTForStmt*>(_statement));
            break;
        case ASTNodeType::ASSIGNMENT_STATEMENT:
            checkAssignmentStatement(static_cast<const ASTAssignStmt*>(_statement));
            break;
        case ASTNodeType::COMPOUND_STATEMENT:
            checkCompoundStatement(static_cast<const ASTCompoundStmt*>(_statement));
            break;
        case ASTNodeType::RETURN_STATEMENT:
            checkReturnStatement(static_cast<const ASTReturnStmt*>(_statement));
            break;
        case ASTNodeType::DO_WHILE_STATEMENT:
            checkDoWhileStatement(static_cast<const ASTDoWhileStmt*>(_statement));
            break;
        case ASTNodeType::SWITCH_STATEMENT:
            checkSwitchStatement(static_cast<const ASTSwitchStmt*>(_statement));
            break;
        case ASTNodeType::FUNCTION_CALL_STATEMENT:
            checkFunctionCallStatement(static_cast<const ASTFunctionCallStmt*>(_statement));
            break;
        default:
            std::unreachable();
    }
}

AnalyzerThreadContext& StatementAnalyzer::getContext() noexcept {
    return FunctionAnalyzer::getContext();
}

void StatementAnalyzer::checkVariable(const ASTVariableDeclStmt* _variable){
    // variable type check
    AnalyzerThreadContext& analyzerContext = getContext();
    Type type{ _variable->getType() };
    if(type == Type::VOID || type == Type::NO_TYPE){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                _variable->getToken().line, _variable->getToken().column, typeToString.at(type), _variable->getToken().value)
        );
    }
    else if(type == Type::AUTO && !_variable->hasAssign()){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction failed '{} {}'", 
                _variable->getToken().line, _variable->getToken().column, typeToString.at(type), _variable->getToken().value)
        );
    }

    // variable redefinition check
    if(!analyzerContext.scopeManager->pushSymbol(Symbol{_variable->getToken().value, Kind::VAR, type})){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> variable redefined '{}'", 
                _variable->getToken().line, _variable->getToken().column, _variable->getToken().value)
        );
    }

    // direct initialization
    if(_variable->getAssign() != nullptr){
        expressionAnalyzer.checkNumericalExpression(_variable->getAssign());

        Type rtype{_variable->getAssign()->getType() };

        // variable initialization type check
        if(rtype != type && type != Type::AUTO && rtype != Type::NO_TYPE){ // rtype == no_type => error was caught in expression, no need to write it again
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid assignment statement - type mismatch: expected '{}', got '{}'", 
                    _variable->getToken().line, _variable->getToken().column, typeToString.at(type), typeToString.at(rtype))
            );
        }

        if(type == Type::AUTO){
            analyzerContext.scopeManager->getSymbol(_variable->getToken().value).setType(rtype);
        }
    }
}

void StatementAnalyzer::checkIfStatement(const ASTIfStmt* _if){
    for(const auto& _condition : _if->getConditions()){
        expressionAnalyzer.checkNumericalExpression(_condition.get());
    }
    // else statement is included
    for(const auto& _statement : _if->getStatements()){
        checkStatement(_statement.get());
    }
}

void StatementAnalyzer::checkWhileStatement(const ASTWhileStmt* _while){
    expressionAnalyzer.checkNumericalExpression(_while->getCondition());
    checkStatement(_while->getStatement());
}

// initializer, condition and incrementer are optional
void StatementAnalyzer::checkForStatement(const ASTForStmt* _for){
    if(_for->hasInitializer()){
        checkAssignmentStatement(_for->getInitializer());
    }
    if(_for->hasCondition()){
        expressionAnalyzer.checkNumericalExpression(_for->getCondition());
    }
    if(_for->hasIncrementer()){
        checkAssignmentStatement(_for->getIncrementer());
    }

    checkStatement(_for->getStatement());
}

void StatementAnalyzer::checkDoWhileStatement(const ASTDoWhileStmt* _dowhile){
    checkStatement(_dowhile->getStatement());
    expressionAnalyzer.checkNumericalExpression(_dowhile->getCondition());
}

void StatementAnalyzer::checkCompoundStatement(const ASTCompoundStmt* _compound){
    // compound scope
    AnalyzerThreadContext& analyzerContext = getContext();
    analyzerContext.scopeManager->pushScope();
    for(const auto& _statement : _compound->getStatements()){
        checkStatement(_statement.get());
    }
    analyzerContext.scopeManager->popScope();
}

void StatementAnalyzer::checkAssignmentStatement(const ASTAssignStmt* _assignment){
    ASTIdExpr* _variable{ _assignment->getVariable() };
    ASTExpr* _value{ _assignment->getExp() };

    // if variable is not defined, rvalue won't be analyzed
    if(!expressionAnalyzer.checkID(_variable)) return;

    expressionAnalyzer.checkNumericalExpression(_value);
    Type rtype{ _value->getType() };
    Type ltype{ _variable->getType() };

    // if numexp contains undefined element it will report the undefined variable, no point checking for type mismatch
    if(rtype == Type::NO_TYPE) return;

    AnalyzerThreadContext& analyzerContext = getContext();

    // assignment type check
    if(rtype != ltype && ltype != Type::AUTO){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid assignment statement - type mismatch: expected '{}', got '{}'", 
                _assignment->getToken().line, _assignment->getToken().column, typeToString.at(ltype), typeToString.at(rtype))
        );
    }

    if(ltype == Type::AUTO){
        analyzerContext.scopeManager->getSymbol(_variable->getToken().value).setType(rtype);
    }
}

void StatementAnalyzer::checkReturnStatement(const ASTReturnStmt* _return){
    Type returnType{ _return->returns() ? expressionAnalyzer.checkNumericalExpressionType(_return->getExp()) : Type::VOID };
    
    // if numexp inside of a return statement contains undef var, it will report it, no point checking for type mismatch
    if(returnType == Type::NO_TYPE) return;
    
    AnalyzerThreadContext& analyzerContext = getContext();

    // return type check
    Type expectedReturnType{ globalScopeManager.getSymbol(analyzerContext.functionName).getType() };
    if(returnType != expectedReturnType){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid return statement - type mismatch: '{} {}' returns '{}'", 
                _return->getToken().line, _return->getToken().column, typeToString.at(expectedReturnType), analyzerContext.functionName, typeToString.at(returnType))
        );
    }
}

void StatementAnalyzer::checkFunctionCallStatement(const ASTFunctionCallStmt* _call){
    expressionAnalyzer.checkFunctionCall(_call->getFunctionCall());
}

void StatementAnalyzer::checkSwitchStatement(const ASTSwitchStmt* _switch){
    // if id is not defined, switch is ignored, all cases will be ignored, including their statements 
    if(!expressionAnalyzer.checkID(_switch->getVariable())) return;
    
    AnalyzerThreadContext& analyzerContext = getContext();
    // case check
    if(_switch->getVariable()->getType() == Type::INT){
        checkSwitchStatementCases<int>(_switch);
    }
    else if(_switch->getVariable()->getType() == Type::UNSIGNED){
        checkSwitchStatementCases<unsigned>(_switch);
    }
    else{
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{}'", 
                _switch->getToken().line, _switch->getToken().column, _switch->getVariable()->getToken().value)
        );
    }
}