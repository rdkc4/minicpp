#include "../statement_analyzer.hpp"
#include "../function_analyzer.hpp"

#include <format>

StatementAnalyzer::StatementAnalyzer(ScopeManager& scopeManager) : globalScopeManager{ scopeManager }, expressionAnalyzer{ scopeManager } {}

void StatementAnalyzer::checkStatement(const ASTStatement* _statement){
    AnalyzerThreadContext& analyzerContext = getContext();
    switch(_statement->getNodeType()){
        case ASTNodeType::VARIABLE:
            checkVariable(static_cast<const ASTVariable*>(_statement));
            break;
        case ASTNodeType::PRINTF:
            checkPrintfStatement(static_cast<const ASTPrintfSt*>(_statement));
            break;
        case ASTNodeType::IF_STATEMENT:
            checkIfStatement(static_cast<const ASTIfSt*>(_statement));
            break;
        case ASTNodeType::WHILE_STATEMENT:
            checkWhileStatement(static_cast<const ASTWhileSt*>(_statement));
            break;
        case ASTNodeType::FOR_STATEMENT:
            checkForStatement(static_cast<const ASTForSt*>(_statement));
            break;
        case ASTNodeType::ASSIGNMENT_STATEMENT:
            checkAssignmentStatement(static_cast<const ASTAssignSt*>(_statement));
            break;
        case ASTNodeType::COMPOUND_STATEMENT:
            checkCompoundStatement(static_cast<const ASTCompoundSt*>(_statement));
            break;
        case ASTNodeType::RETURN_STATEMENT:
            checkReturnStatement(static_cast<const ASTReturnSt*>(_statement));
            break;
        case ASTNodeType::DO_WHILE_STATEMENT:
            checkDoWhileStatement(static_cast<const ASTDoWhileSt*>(_statement));
            break;
        case ASTNodeType::SWITCH_STATEMENT:
            checkSwitchStatement(static_cast<const ASTSwitchSt*>(_statement));
            break;
        default:
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SYNTAX ERROR -> expected 'STATEMENT' got '{}'",
                    _statement->getToken().line, _statement->getToken().column, astNodeTypeToString.at(_statement->getNodeType()))
            );
    }
}

AnalyzerThreadContext& StatementAnalyzer::getContext() noexcept {
    return FunctionAnalyzer::getContext();
}

void StatementAnalyzer::checkVariable(const ASTVariable* _variable){
    // variable type check
    AnalyzerThreadContext& analyzerContext = getContext();
    Types type{ _variable->getType() };
    if(type == Types::VOID || type == Types::NO_TYPE){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                _variable->getToken().line, _variable->getToken().column, typeToString.at(type), _variable->getToken().value)
        );
    }
    else if(type == Types::AUTO && !_variable->hasAssign()){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction failed '{} {}'", 
                _variable->getToken().line, _variable->getToken().column, typeToString.at(type), _variable->getToken().value)
        );
    }

    // variable redefinition check
    if(!analyzerContext.scopeManager->pushSymbol(Symbol{_variable->getToken().value, Kinds::VAR, type})){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> variable redefined '{}'", 
                _variable->getToken().line, _variable->getToken().column, _variable->getToken().value)
        );
    }

    // direct initialization
    if(_variable->getAssign() != nullptr){
        expressionAnalyzer.checkNumericalExpression(_variable->getAssign());

        Types rtype{_variable->getAssign()->getType() };

        // variable initialization type check
        if(rtype != type && type != Types::AUTO && rtype != Types::NO_TYPE){ // rtype == no_type => error was caught in expression, no need to write it again
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid assignment statement - type mismatch: expected '{}', got '{}'", 
                    _variable->getToken().line, _variable->getToken().column, typeToString.at(type), typeToString.at(rtype))
            );
        }

        if(type == Types::AUTO){
            analyzerContext.scopeManager->getSymbol(_variable->getToken().value).setType(rtype);
        }
    }
}

void StatementAnalyzer::checkPrintfStatement(const ASTPrintfSt* _printf){
    expressionAnalyzer.checkNumericalExpression(_printf->getExp());
}

void StatementAnalyzer::checkIfStatement(const ASTIfSt* _if){
    for(const auto& _condition : _if->getConditions()){
        expressionAnalyzer.checkNumericalExpression(_condition.get());
    }
    // else statement is included
    for(const auto& _statement : _if->getStatements()){
        checkStatement(_statement.get());
    }
}

void StatementAnalyzer::checkWhileStatement(const ASTWhileSt* _while){
    expressionAnalyzer.checkNumericalExpression(_while->getCondition());
    checkStatement(_while->getStatement());
}

// initializer, condition and incrementer are optional
void StatementAnalyzer::checkForStatement(const ASTForSt* _for){
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

void StatementAnalyzer::checkDoWhileStatement(const ASTDoWhileSt* _dowhile){
    checkStatement(_dowhile->getStatement());
    expressionAnalyzer.checkNumericalExpression(_dowhile->getCondition());
}

void StatementAnalyzer::checkCompoundStatement(const ASTCompoundSt* _compound){
    // compound scope
    AnalyzerThreadContext& analyzerContext = getContext();
    analyzerContext.scopeManager->pushScope();
    for(const auto& _statement : _compound->getStatements()){
        checkStatement(_statement.get());
    }
    analyzerContext.scopeManager->popScope();
}

void StatementAnalyzer::checkAssignmentStatement(const ASTAssignSt* _assignment){
    ASTId* _variable{ _assignment->getVariable() };
    ASTExpression* _value{ _assignment->getExp() };

    // if variable is not defined, rvalue won't be analyzed
    if(!expressionAnalyzer.checkID(_variable)) return;

    expressionAnalyzer.checkNumericalExpression(_value);
    Types rtype{ _value->getType() };
    Types ltype{ _variable->getType() };

    // if numexp contains undefined element it will report the undefined variable, no point checking for type mismatch
    if(rtype == Types::NO_TYPE) return;

    AnalyzerThreadContext& analyzerContext = getContext();

    // assignment type check
    if(rtype != ltype && ltype != Types::AUTO){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid assignment statement - type mismatch: expected '{}', got '{}'", 
                _assignment->getToken().line, _assignment->getToken().column, typeToString.at(ltype), typeToString.at(rtype))
        );
    }

    if(ltype == Types::AUTO){
        analyzerContext.scopeManager->getSymbol(_variable->getToken().value).setType(rtype);
    }
}

void StatementAnalyzer::checkReturnStatement(const ASTReturnSt* _return){
    Types returnType{ _return->returns() ? expressionAnalyzer.checkNumericalExpressionType(_return->getExp()) : Types::VOID };
    
    // if numexp inside of a return statement contains undef var, it will report it, no point checking for type mismatch
    if(returnType == Types::NO_TYPE) return;
    
    AnalyzerThreadContext& analyzerContext = getContext();

    // return type check
    Types expectedReturnType{ globalScopeManager.getSymbol(analyzerContext.functionName).getType() };
    if(returnType != expectedReturnType){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid return statement - type mismatch: '{} {}' returns '{}'", 
                _return->getToken().line, _return->getToken().column, typeToString.at(expectedReturnType), analyzerContext.functionName, typeToString.at(returnType))
        );
    }
}

void StatementAnalyzer::checkSwitchStatement(const ASTSwitchSt* _switch){
    // if id is not defined, switch is ignored, all cases will be ignored, including their statements 
    if(!expressionAnalyzer.checkID(_switch->getVariable())) return;
    
    AnalyzerThreadContext& analyzerContext = getContext();
    // case check
    if(_switch->getVariable()->getType() == Types::INT){
        checkSwitchStatementCases<int>(_switch);
    }
    else if(_switch->getVariable()->getType() == Types::UNSIGNED){
        checkSwitchStatementCases<unsigned>(_switch);
    }
    else{
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{}'", 
                _switch->getToken().line, _switch->getToken().column, _switch->getVariable()->getToken().value)
        );
    }
}