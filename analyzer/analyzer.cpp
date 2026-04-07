#include "analyzer.hpp"

#include <unordered_set>
#include <filesystem>
#include <latch>

#include "../common/preprocessing/preprocessing_libs.hpp"
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
        threadPool.enqueue(
            [this, function=function.get(), &doneLatch] -> void {
                function->accept(*this);
                doneLatch.count_down();
            }
        );
    }
    doneLatch.wait();

    // check if main exists
    if(!globalScopeManager.lookupSymbol("main", {Kind::FUN})){
        semanticErrors[globalError].emplace_back("'main' function not found");
    }

    globalScopeManager.popScope();
}

void Analyzer::visit(ASTIncludeDir* includeDir){
    if(!std::filesystem::exists(Preprocessing::Libs::generateLibSourcePath(includeDir->getLibName()))){
        const auto& token{ includeDir->getToken() };
        reportError(
            token, 
            std::format(
                "unknown library '{}'", 
                includeDir->getLibName()
            ),
            globalError
        );
    }
}

void Analyzer::checkFunctionSignature(const ASTFunction* function){
    Type returnType{ function->getType() };
    const auto& funcToken{ function->getToken() };
    semanticErrors[funcToken.value] = {};

    // function redefinition check
    if(!globalScopeManager.pushSymbol(Symbol{funcToken.value, Kind::FUN, returnType})){
        reportError(
            funcToken, 
            std::format(
                "function redefined '{} {}'", 
                typeToString.at(returnType), funcToken.value
            ),
            globalError
        );
        return;
    }

    // function type check
    if(returnType == Type::NO_TYPE){
        reportError(
            funcToken, 
            std::format(
                "invalid type '{} {}'", 
                typeToString.at(returnType), funcToken.value
            ),
            funcToken.value
        );
    }
    else if(returnType == Type::AUTO){
        reportError(
            funcToken, 
            std::format(
                "type deduction cannot be performed on function '{} {}'", 
                typeToString.at(returnType), funcToken.value
            ),
            funcToken.value
        );
    }

    SymbolTable symTab;
    ScopeManager signatureScopeManager{symTab};
    analyzerContext.init(funcToken.value, &signatureScopeManager);

    analyzerContext.scopeManager->pushScope();
    const auto& parameters{ function->getParameters() };

    // pointer to parameters for easier function call type checking
    globalScopeManager.getSymbol(funcToken.value).setParameters(&parameters);
    
    // parameter check for main
    if(funcToken.value == "main" && parameters.size() > 0){
        const auto& paramToken{ parameters[0]->getToken() };
        reportError(
            paramToken, 
            "function 'main' cannot have any parameters",
            funcToken.value
        );
    }

    for(const auto& parameter : parameters){
        parameter->accept(*this);
    }
    analyzerContext.scopeManager->popScope();

    for(const auto& err : analyzerContext.semanticErrors){
        semanticErrors[funcToken.value].push_back(err);
    }

    analyzerContext.reset();
}

void Analyzer::visit(ASTFunction* function){
    const auto& funcToken{ function->getToken() };
    const auto funcReturnType{ function->getType() };

    SymbolTable symTab;
    ScopeManager functionScopeManager{symTab};

    // initializing thread context
    analyzerContext.init(funcToken.value, &functionScopeManager);

    // function scope
    analyzerContext.scopeManager->pushScope();
    defineParameters(function);
    if(!function->isPredefined()){
        for(const auto& stmt : function->getBody()){
            stmt->accept(*this);
        }

        // function return type check
        if(funcReturnType != Type::VOID){
            ReturnChecker returnChecker;
            function->accept(returnChecker);

            if(!function->alwaysReturnsValue()){
                reportError(
                    funcToken, 
                    std::format(
                        "function '{} {}' not all paths return value", 
                        typeToString.at(funcReturnType), funcToken.value
                    )
                );
            }
        }
    }
    analyzerContext.scopeManager->popScope();

    {
        std::lock_guard<std::mutex> lock(errorMtx);
        semanticErrors[funcToken.value] = std::move(analyzerContext.semanticErrors);
    }
    analyzerContext.reset();
}

void Analyzer::defineParameters(const ASTFunction* function){
    for(const auto& parameter : function->getParameters()){
        analyzerContext.scopeManager->pushSymbol(
            Symbol{parameter->getToken().value, Kind::PAR, parameter->getType()}
        );
    }
}

void Analyzer::visit(ASTParameter* parameter){
    Type paramType{ parameter->getType() };
    const auto& paramToken{ parameter->getToken() };

    if(paramType == Type::VOID || paramType == Type::NO_TYPE){
        reportError(
            paramToken, 
            std::format(
                "invalid type '{} {}'", 
                typeToString.at(paramType), paramToken.value
            )
        );
    }
    else if(paramType == Type::AUTO){
        reportError(
            paramToken, 
            std::format(
                "type deduction cannot be performed on parameters '{} {}'", 
                typeToString.at(paramType), paramToken.value
            )
        );
    }

    // parameter redefinition check
    if(!analyzerContext.scopeManager->pushSymbol(Symbol{paramToken.value, Kind::PAR, paramType})){
        reportError(
            paramToken, 
            std::format(
                "parameter redefined '{} {}'", 
                typeToString.at(paramType), paramToken.value
            )
        );
    }
}

void Analyzer::visit(ASTVariableDeclStmt* variableDecl){
    // variable type check
    Type variableType{ variableDecl->getType() };
    const auto& variableToken{ variableDecl->getToken() };
    if(variableType == Type::VOID || variableType == Type::NO_TYPE){
        reportError(
            variableToken, 
            std::format(
                "invalid type '{} {}'", 
                typeToString.at(variableType), variableToken.value
            )
        );
    }
    else if(variableType == Type::AUTO && !variableDecl->hasAssignExpr()){
        reportError(
            variableToken, 
            std::format(
                "type deduction failed '{} {}'", 
                typeToString.at(variableType), variableToken.value
            )
        );
    }

    // variable redefinition check
    if(!analyzerContext.scopeManager->pushSymbol(Symbol{variableToken.value, Kind::VAR, variableType})){
        reportError(
            variableToken, 
            std::format(
                "variable redefined '{} {}'", 
                typeToString.at(variableType), variableToken.value
            )
        );
    }

    // direct initialization
    if(variableDecl->hasAssignExpr()){
        auto* assignExpr{ variableDecl->getAssignExpr() };
        assignExpr->accept(*this);

        Type rtype{ assignExpr->getType() };

        // variable initialization type check
        if(rtype != variableType && variableType != Type::AUTO && rtype != Type::NO_TYPE){ // rtype == no_type => error was caught in expression, no need to write it again
            reportError(
                variableToken, 
                std::format(
                    "invalid assignment statement - type mismatch: expected '{}', got '{}'", 
                    typeToString.at(variableType), typeToString.at(rtype)
                )
            );
        }

        if(variableType == Type::AUTO){
            analyzerContext.scopeManager->getSymbol(variableToken.value).setType(rtype);
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
        const auto& assignStmtToken{ assignStmt->getToken() };
        reportError(
            assignStmtToken, 
            std::format(
                "invalid assignment statement - type mismatch: expected '{}', got '{}'", 
                typeToString.at(ltype), typeToString.at(rtype)
            )
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
    const auto& conditions{ ifStmt->getConditionExprs() };
    const auto& stmts{ ifStmt->getStmts() };

    const size_t size{ conditions.size() };
    for(size_t i{0}; i < size; ++i){
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
        auto* returnExpr{ returnStmt->getReturnExpr() };
        returnExpr->accept(*this);
        returnType = returnExpr->getType();
    }

    // if numexp inside of a return statement contains undef var, it will report it, no point checking for type mismatch
    if(returnType == Type::NO_TYPE) return;

    // return type check
    Type expectedReturnType{ globalScopeManager.getSymbol(analyzerContext.functionName).getType() };
    if(returnType != expectedReturnType){
        const auto& returnToken{ returnStmt->getToken() };
        reportError(
            returnToken, 
            std::format(
                "invalid return statement - type mismatch: '{} {}' returns '{}'", 
                typeToString.at(expectedReturnType), analyzerContext.functionName, typeToString.at(returnType)
            )
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
    auto* variableIdExpr{ switchStmt->getVariableIdExpr() };
    variableIdExpr->accept(*this);

    auto variableIdExprType{ variableIdExpr->getType() };
    if(variableIdExprType == Type::NO_TYPE) return;
    
    std::unordered_set<std::string> caseSet;
    for(const auto& caseStmt : switchStmt->getCaseStmts()){
        caseStmt->accept(*this);

        const auto& caseToken{ caseStmt->getToken() };

        const auto* literalExpr{ caseStmt->getLiteralExpr() };
        const auto& literalExprToken{ literalExpr->getToken() };
        auto literalType{ literalExpr->getType() };

        // type mismatch
        if(literalType != variableIdExprType){
            reportError(
                caseToken, 
                std::format(
                    "invalid case - type mismatch: expected '{}', got '{}'", 
                    typeToString.at(variableIdExprType), typeToString.at(literalType)
                )
            );
        }

        // duplicates
        if(caseSet.find(literalExprToken.value) != caseSet.end()){
            reportError(
                caseToken, 
                std::format(
                    "duplicate case '{}'", 
                    literalExprToken.value
                )
            );
        }
        caseSet.insert(literalExprToken.value);
    }

    if(switchStmt->hasDefaultStmt()){
        switchStmt->getDefaultStmt()->accept(*this);
    }

}

void Analyzer::visit(ASTCaseStmt* caseStmt){
    auto* literalExpr{ caseStmt->getLiteralExpr() };
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
    auto* leftOperand{ binaryExpr->getLeftOperandExpr() };
    auto* rightOperand{ binaryExpr->getRightOperandExpr() };

    leftOperand->accept(*this);
    rightOperand->accept(*this);

    auto ltype{ leftOperand->getType() };
    auto rtype{ rightOperand->getType() };

    if(ltype != rtype && ltype != Type::NO_TYPE && rtype != Type::NO_TYPE){
        const auto& binaryExprToken{ binaryExpr->getToken() };
        reportError(
            binaryExprToken, 
            std::format(
                "binary expression - type mismatch: expected '{}', got '{}'", 
                typeToString.at(ltype), typeToString.at(rtype)
            )
        );
    }
    binaryExpr->setType(ltype);
}

void Analyzer::visit(ASTFunctionCallExpr* callExpr){
    const auto& callExprToken{  callExpr->getToken() };

    const auto* callExprSymbol{ 
        globalScopeManager.lookupSymbol(callExprToken.value, {Kind::FUN}) 
    };

    if(!callExprSymbol){
        reportError(
            callExprToken, 
            std::format(
                "undefined function '{}'", 
                callExprToken.value
            )
        );
        // function doesn't exist, no point checking arguments
        return;
    }

    // main function can't be called
    if(callExprToken.value == "main"){
        reportError(
            callExprToken, 
            "'main' is not callable function"
        );
        return;
    }

    const auto* callExprParams{ callExprSymbol->getParameters() };
    callExpr->setType(callExprSymbol->getType());

    // comparison of given parameter count with expected parameter count
    size_t providedParams{ callExpr->getArgumentCount() };
    size_t expectedParams{ callExprParams->size() };
    if(providedParams != expectedParams){
        reportError(
            callExprToken, 
            std::format(
                "invalid function call '{}': provided arguments '{}', expected '{}'", 
                callExprToken.value, providedParams, expectedParams
            )
        );
        return; // number of provided arguments differs from expected, no type checking
    }
    
    const auto& arguments{ callExpr->getArguments() };
    for(size_t i{0}; i < providedParams; ++i){
        const auto& arg{ arguments[i] };
        arg->accept(*this);

        // type check of corresponding parameters in function and function call
        Type ltype{ arg->getType() };
        Type rtype{ (*callExprParams)[i]->getType() };

        // no point checking for types if argument is invalid
        if(ltype == Type::NO_TYPE) continue;

        if(ltype != rtype){
            const auto& argToken{ arg->getToken() };
            reportError(
                argToken, 
                std::format(
                    "invalid argument {} - type mismatch: expected '{}', got '{} {}'", 
                    i, typeToString.at(rtype), typeToString.at(ltype), argToken.value
                )
            );
        }
    }
}

void Analyzer::visit(ASTIdExpr* idExpr){
    const auto& idExprToken{ idExpr->getToken() };
    
    // check if id exists
    const auto* idExprSymbol{ analyzerContext.scopeManager->lookupSymbol(idExprToken.value, {Kind::VAR, Kind::PAR}) };
    if(!idExprSymbol){
        reportError(
            idExprToken, 
            std::format(
                "undefined variable '{}'", 
                idExprToken.value
            )
        );
        idExpr->setType(Type::NO_TYPE);
        return;
    }
    idExpr->setType(idExprSymbol->getType());
}

void Analyzer::visit(ASTLiteralExpr* literalExpr){
    const auto& literalExprToken{ literalExpr->getToken() };

    // invalid literal check
    if(isInvalidLiteral(literalExpr->getType(), literalExprToken.value)){
        reportError(
            literalExprToken, 
            std::format(
                "invalid literal '{}'", 
                literalExprToken.value
            )
        );
    }
}

bool Analyzer::isInvalidLiteral(Type type, const std::string& value) const {
    if (value.empty()) [[unlikely]] return true;

    const char last{ value.back() };
    const char first{ value.front() };

    if (type == Type::UNSIGNED) {
        return last != 'u' || first == '-';
    }

    if (type == Type::INT) {
        return last == 'u';
    }

    return false;
}


bool Analyzer::hasSemanticErrors(const ASTProgram* program) const noexcept {
    for(const auto& function : program->getFunctions()){
        const std::string& funcName{ function->getToken().value };
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
    size_t errLen{ errors.str().length() };

    for(const auto& function : program->getFunctions()){
        const std::string& funcName{ function->getToken().value };
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

    std::string strErrors{ errors.str() }; 

    return strErrors.length() != errLen ? strErrors : "";
}