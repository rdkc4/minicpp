#include "analyzer.hpp"

#include <format>
#include <unordered_set>
#include <filesystem>
#include <cassert>
#include <latch>

#include "../common/preprocessing/preprocessing_libraries.hpp"
#include "return_checker.hpp"

Analyzer::Analyzer(ScopeManager& scopeManager, ThreadPool& threadPool)
    : threadPool{ threadPool }, globalScopeManager{scopeManager} {}

thread_local AnalyzerThreadContext Analyzer::analyzerContext;

void Analyzer::visit(ASTProgram* program){
    semanticErrors[globalError] = {};

    // global scope
    globalScopeManager.pushScope();

    for(const auto& dir : program->getDirs()){
        dir->accept(*this);
    }

    for(const auto& function : program->getFunctions()){
        checkFunctionSignature(function.get());
    }

    if(hasSemanticErrors(program)) return;
    
    std::latch doneLatch{ static_cast<std::ptrdiff_t>(program->getFunctionCount()) };
    for(const auto& function : program->getFunctions()){
        threadPool.enqueue([this, function=function.get(), &doneLatch] {
            function->accept(*this);
            doneLatch.count_down();
        });
    }
    doneLatch.wait();

    // check if main exists
    if(!globalScopeManager.lookupSymbol("main", {Kind::FUN})){
        semanticErrors[globalError].push_back("'main' function not found");
    }

    globalScopeManager.popScope();
}

void Analyzer::visit(ASTIncludeDir* includeDir){
    if(!std::filesystem::exists(Preprocessing::Libs::generateLibSourcePath(includeDir->getLibName()))){
        semanticErrors.at(globalError).push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> unknown library '{}'", 
                includeDir->getToken().line, includeDir->getToken().column, includeDir->getLibName())
        );
    }
}

void Analyzer::checkFunctionSignature(const ASTFunction* function){
    Type returnType{ function->getType() };
    const std::string& funcName = function->getToken().value;
    semanticErrors[funcName] = {};

    // function redefinition check
    if(!globalScopeManager.pushSymbol(Symbol{funcName, Kind::FUN, returnType})){
        semanticErrors[globalError].push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> function redefined '{} {}'", 
                function->getToken().line, function->getToken().column, typeToString.at(returnType), function->getToken().value)
        );
        return;
    }

    // function type check
    if(returnType == Type::NO_TYPE){
        semanticErrors[funcName].push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                function->getToken().line, function->getToken().column, typeToString.at(returnType), function->getToken().value)
        );
    }
    else if(returnType == Type::AUTO){
        semanticErrors[funcName].push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction cannot be performed on function '{} {}'", 
                function->getToken().line, function->getToken().column, typeToString.at(returnType), function->getToken().value)
        );
    }

    SymbolTable symTab;
    ScopeManager signatureScopeManager{symTab};
    analyzerContext.init(funcName, &signatureScopeManager);

    analyzerContext.scopeManager->pushScope();
    const auto& parameters = function->getParameters();

    // pointer to parameters for easier function call type checking
    globalScopeManager.getSymbol(funcName).setParameters(&parameters);
    
    // parameter check for main
    if(funcName == "main" && parameters.size() > 0){
        semanticErrors[funcName].push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> function 'main' cannot have any parameters", 
                parameters[0]->getToken().line, parameters[0]->getToken().column
            )
        );
    }

    for(const auto& parameter : parameters){
        parameter->accept(*this);
    }
    analyzerContext.scopeManager->popScope();

    for(const auto& err : analyzerContext.semanticErrors){
        semanticErrors[funcName].push_back(err);
    }

    analyzerContext.reset();
}

void Analyzer::visit(ASTFunction* function){
    const std::string functionName = function->getToken().value;

    SymbolTable symTab;
    ScopeManager functionScopeManager{symTab};

    // initializing thread context
    analyzerContext.init(functionName, &functionScopeManager);

    // function scope
    analyzerContext.scopeManager->pushScope();
    defineParameters(function);
    if(!function->isPredefined()){
        for(const auto& stmt : function->getBody()){
            stmt->accept(*this);
        }

        // function return type check
        if(function->getType() != Type::VOID){
            ReturnChecker returnChecker;
            function->accept(returnChecker);

            if(!function->alwaysReturnsValue()){
                analyzerContext.semanticErrors.push_back(
                    std::format("Line {}, Column {}: SEMANTIC ERROR -> function '{} {}' not all paths return value", 
                        function->getToken().line, function->getToken().column, typeToString.at(function->getType()), function->getToken().value)
                );
            }
        }
    }
    analyzerContext.scopeManager->popScope();

    {
        std::lock_guard<std::mutex> lock(errorMtx);
        assert(semanticErrors[functionName].empty());
        semanticErrors[functionName] = std::move(analyzerContext.semanticErrors);
    }
    analyzerContext.reset();
}

void Analyzer::defineParameters(const ASTFunction* function){
    for(const auto& parameter : function->getParameters()){
        analyzerContext.scopeManager->pushSymbol(Symbol{parameter->getToken().value, Kind::PAR, parameter->getType()});
    }
}

void Analyzer::visit(ASTParameter* parameter){
    Type type{ parameter->getType() };
    if(type == Type::VOID || type == Type::NO_TYPE){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                parameter->getToken().line, parameter->getToken().column, typeToString.at(type), parameter->getToken().value)
        );
    }
    else if(type == Type::AUTO){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction cannot be performed on parameters '{} {}'", 
                parameter->getToken().line, parameter->getToken().column, typeToString.at(type), parameter->getToken().value)
        );
    }

    // parameter redefinition check
    if(!analyzerContext.scopeManager->pushSymbol(Symbol{parameter->getToken().value, Kind::PAR, parameter->getType()})){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> variable redefined '{}'", 
                parameter->getToken().line, parameter->getToken().column, parameter->getToken().value)
        );
    }
}

void Analyzer::visit(ASTVariableDeclStmt* variableDecl){
    // variable type check
    Type type{ variableDecl->getType() };
    if(type == Type::VOID || type == Type::NO_TYPE){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                variableDecl->getToken().line, variableDecl->getToken().column, typeToString.at(type), variableDecl->getToken().value)
        );
    }
    else if(type == Type::AUTO && !variableDecl->hasAssignExpr()){
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
    if(variableDecl->getAssignExpr() != nullptr){
        variableDecl->getAssignExpr()->accept(*this);

        Type rtype{variableDecl->getAssignExpr()->getType() };

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

void Analyzer::visit(ASTAssignStmt* assignStmt){
    ASTIdExpr* variableExpr{ assignStmt->getVariableIdExpr() };
    ASTExpr* valueExpr{ assignStmt->getAssignedExpr() };

    variableExpr->accept(*this);
    valueExpr->accept(*this);

    if(variableExpr->getType() == Type::NO_TYPE || valueExpr->getType() == Type::NO_TYPE) return;

    Type rtype{ valueExpr->getType() };
    Type ltype{ variableExpr->getType() };

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

void Analyzer::visit(ASTCompoundStmt* compoundStmt){
    analyzerContext.scopeManager->pushScope();
    for(const auto& stmt : compoundStmt->getStmts()){
        stmt->accept(*this);
    }
    analyzerContext.scopeManager->popScope();
}

void Analyzer::visit(ASTForStmt* forStmt){
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

void Analyzer::visit(ASTFunctionCallStmt* callStmt){
    callStmt->getFunctionCallExpr()->accept(*this);
}

void Analyzer::visit(ASTIfStmt* ifStmt){
    const auto& conditions = ifStmt->getConditionExprs();
    const auto& stmts = ifStmt->getStmts();

    const size_t size = conditions.size();
    for(size_t i = 0; i < size; ++i){
        conditions[i]->accept(*this);
        stmts[i]->accept(*this);
    }

    if(ifStmt->hasElseStmt()){
        stmts.back()->accept(*this);
    }
}

void Analyzer::visit(ASTReturnStmt* returnStmt){
    Type returnType{Type::VOID};

    if(returnStmt->hasReturnExpr()){
        returnStmt->getReturnExpr()->accept(*this);
        returnType = returnStmt->getReturnExpr()->getType();
    }

    // if numexp inside of a return statement contains undef var, it will report it, no point checking for type mismatch
    if(returnType == Type::NO_TYPE) return;

    // return type check
    Type expectedReturnType{ globalScopeManager.getSymbol(analyzerContext.functionName).getType() };
    if(returnType != expectedReturnType){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid return statement - type mismatch: '{} {}' returns '{}'", 
                returnStmt->getToken().line, returnStmt->getToken().column, typeToString.at(expectedReturnType), analyzerContext.functionName, typeToString.at(returnType))
        );
    }
}

void Analyzer::visit(ASTWhileStmt* whileStmt){
    whileStmt->getConditionExpr()->accept(*this);
    whileStmt->getStmt()->accept(*this);
}

void Analyzer::visit(ASTDoWhileStmt* dowhileStmt){
    dowhileStmt->getStmt()->accept(*this);
    dowhileStmt->getConditionExpr()->accept(*this);
}

void Analyzer::visit(ASTSwitchStmt* switchStmt){
    switchStmt->getVariableIdExpr()->accept(*this);

    if(switchStmt->getVariableIdExpr()->getType() == Type::NO_TYPE) return;
    
    auto expectedType = switchStmt->getVariableIdExpr()->getType();
    std::unordered_set<std::string> caseSet;
    for(const auto& caseStmt : switchStmt->getCaseStmts()){
        caseStmt->accept(*this);
        const auto* literalExpr = caseStmt->getLiteralExpr();

        // type mismatch
        if(literalExpr->getType() != expectedType){
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid case - type mismatch: expected '{}', got '{}'", 
                    caseStmt->getToken().line, caseStmt->getToken().column, typeToString.at(expectedType), typeToString.at(literalExpr->getType()))
            );
        }
        // duplicates
        if(caseSet.contains(literalExpr->getToken().value)){
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> duplicate case '{}'", 
                    caseStmt->getToken().line, caseStmt->getToken().column, caseStmt->getLiteralExpr()->getToken().value)
            );
        }
        caseSet.insert(literalExpr->getToken().value);
    }

    if(switchStmt->hasDefaultStmt()){
        switchStmt->getDefaultStmt()->accept(*this);
    }

}

void Analyzer::visit(ASTCaseStmt* caseStmt){
    auto* literalExpr = caseStmt->getLiteralExpr();
    literalExpr->accept(*this);
    if(literalExpr->getType() == Type::NO_TYPE) return;

    caseStmt->getSwitchBlockStmt()->accept(*this);
}

void Analyzer::visit(ASTDefaultStmt* defaultStmt){
    defaultStmt->getSwitchBlockStmt()->accept(*this);
}

void Analyzer::visit(ASTSwitchBlockStmt* switchBlockStmt){
    for(const auto& stmt : switchBlockStmt->getStmts()){
        stmt->accept(*this);
    }
}

void Analyzer::visit(ASTBinaryExpr* binaryExpr){
    binaryExpr->getLeftOperandExpr()->accept(*this);
    binaryExpr->getRightOperandExpr()->accept(*this);

    auto ltype = binaryExpr->getLeftOperandExpr()->getType();
    auto rtype = binaryExpr->getRightOperandExpr()->getType();

    if(ltype != rtype && ltype != Type::NO_TYPE && rtype != Type::NO_TYPE){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid assignment statement - type mismatch: expected '{}', got '{}'", 
                binaryExpr->getToken().line, binaryExpr->getToken().column, typeToString.at(ltype), typeToString.at(rtype))
        );
    }
    binaryExpr->setType(ltype);
}

void Analyzer::visit(ASTFunctionCallExpr* callExpr){
    if(!globalScopeManager.lookupSymbol(callExpr->getToken().value, {Kind::FUN})){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> undefined function '{}'", 
                callExpr->getToken().line, callExpr->getToken().column, callExpr->getToken().value)
        );
        // function doesn't exist, no point checking arguments
        return;
    }
    
    // main function can't be called
    if(callExpr->getToken().value == "main"){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> 'main' is not callable function", 
                callExpr->getToken().line, callExpr->getToken().column)
        );
        return;
    }

    // comparation of given parameter count with expected parameter count
    size_t expectedParams{ globalScopeManager.getSymbol(callExpr->getToken().value).getParameters()->size() };
    if(callExpr->getArgumentCount() != expectedParams){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid function call '{}': provided arguments '{}', expected '{}'", 
                callExpr->getToken().line, callExpr->getToken().column, callExpr->getToken().value, callExpr->getArgumentCount(), expectedParams)
        );
        callExpr->setType(globalScopeManager.getSymbol(callExpr->getToken().value).getType());
        // number of provided arguments differs from expected, no type checking
        return;
    }
    callExpr->setType(globalScopeManager.getSymbol(callExpr->getToken().value).getType());
    
    const auto* functionParameters{ globalScopeManager.getSymbol(callExpr->getToken().value).getParameters() };
    const auto& arguments{ callExpr->getArguments() };

    for(size_t i = 0; i < callExpr->getArgumentCount(); ++i){
        arguments[i]->accept(*this);

        // type check of corresponding parameters in function and function call
        Type ltype{ arguments[i]->getType() };
        Type rtype{ (*functionParameters)[i]->getType() };

        // no point checking for types if argument is invalid
        if(ltype == Type::NO_TYPE) return;

        if(ltype != rtype){
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid argument - type mismatch: expected '{}', got '{} {}'",
                    arguments[i]->getToken().line, arguments[i]->getToken().column, typeToString.at(rtype), typeToString.at(ltype), arguments[i]->getToken().value)
            );
        }
    }
}

void Analyzer::visit(ASTIdExpr* idExpr){
    std::string name{ idExpr->getToken().value };
    
    // check if id exists
    if(!analyzerContext.scopeManager->lookupSymbol(name, {Kind::VAR, Kind::PAR})){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> undefined variable '{}'", 
                idExpr->getToken().line, idExpr->getToken().column, name)
        );
        idExpr->setType(Type::NO_TYPE);
        return;
    }
    idExpr->setType(analyzerContext.scopeManager->getSymbol(name).getType());
}

void Analyzer::visit(ASTLiteralExpr* literalExpr){
    std::string name{ literalExpr->getToken().value };
    // invalid literal check
    if((literalExpr->getType() == Type::UNSIGNED && (name.back() != 'u' || name.front() == '-')) || 
        (literalExpr->getType() == Type::INT && name.back() == 'u')
    ){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid literal '{}'", 
                literalExpr->getToken().line, literalExpr->getToken().column, name)
        );
    }
}


bool Analyzer::hasSemanticErrors(const ASTProgram* program) const noexcept {
    for(const auto& _function : program->getFunctions()){
        const std::string& funcName = _function->getToken().value;
        if(!semanticErrors.at(funcName).empty()){
            return true;
        }   
    }
    
    if(!semanticErrors.at(globalError).empty()){
        return true;
    }

    return false;
}

std::string Analyzer::getSemanticErrors(const ASTProgram* program) const noexcept {
    if(semanticErrors.empty()){
        return "";
    }

    std::stringstream errors{"Semantic check failed:\n"};
    size_t errLen = errors.str().length();

    for(const auto& function : program->getFunctions()){
        const std::string& funcName = function->getToken().value;
        if(!semanticErrors.at(funcName).empty()){
            for(const auto& error : semanticErrors.at(funcName)){
                errors << error << "\n";
            }
        }
    }
    if(!semanticErrors.at(globalError).empty()){
        for(const auto& error : semanticErrors.at(globalError)){
            errors << error << "\n";
        }
    }

    std::string strErrors = errors.str(); 

    return strErrors.length() != errLen ? strErrors : "";
}