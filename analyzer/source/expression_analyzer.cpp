#include "../expression_analyzer.hpp"
#include "../function_analyzer.hpp"

#include <format>
#include <string>
#include <vector>
#include <memory>

#include "../../common/abstract-syntax-tree/ast_binary_expr.hpp"

ExpressionAnalyzer::ExpressionAnalyzer(ScopeManager& scopeManager) : globalScopeManager{ scopeManager } {}

void ExpressionAnalyzer::checkNumericalExpr(ASTExpr* numericalExpr){
    // setting type to numexp node for easier type checking
    numericalExpr->setType(checkNumericalExprType(numericalExpr));
}

Type ExpressionAnalyzer::checkNumericalExprType(ASTExpr* numericalExpr){
    if(numericalExpr->getNodeType() == ASTNodeType::ID){
        checkIDExpr(static_cast<ASTIdExpr*>(numericalExpr));
        return numericalExpr->getType();
    }
    else if(numericalExpr->getNodeType() == ASTNodeType::LITERAL){
        checkLiteralExpr(static_cast<const ASTLiteralExpr*>(numericalExpr));
        return numericalExpr->getType();
    }
    else if(numericalExpr->getNodeType() == ASTNodeType::FUNCTION_CALL){
        checkFunctionCallExpr(static_cast<ASTFunctionCallExpr*>(numericalExpr));
        return numericalExpr->getType();
    } // no unary operators yet
    else{
        // numerical expression type check
        ASTBinaryExpr* binExp = static_cast<ASTBinaryExpr*>(numericalExpr);
        Type ltype{ checkNumericalExprType(binExp->getLeftOperandExpr()) };
        Type rtype{ checkNumericalExprType(binExp->getRightOperandExpr()) };

        // if either subexpression is invalid, root expression shall be invalid too, no point checking for type mismatch
        if(ltype == Type::NO_TYPE || rtype == Type::NO_TYPE) return Type::NO_TYPE;

        auto& analyzerContext = FunctionAnalyzer::getContext();
        if(ltype != rtype){
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid expression - type mismatch: expected '{}', got '{}'", 
                    numericalExpr->getToken().line, numericalExpr->getToken().column, typeToString.at(ltype), typeToString.at(rtype))
            );
            return Type::NO_TYPE;
        }
        return ltype;
    }
}

void ExpressionAnalyzer::checkRelationalExpr(ASTExpr* relationalExpr){
    ASTBinaryExpr* binaryExpr = static_cast<ASTBinaryExpr*>(relationalExpr);
    checkNumericalExpr(binaryExpr->getLeftOperandExpr());
    checkNumericalExpr(binaryExpr->getRightOperandExpr());
    
    auto& analyzerContext = FunctionAnalyzer::getContext();
    auto ltype = binaryExpr->getLeftOperandExpr()->getType();
    auto rtype = binaryExpr->getRightOperandExpr()->getType();

    if(ltype != rtype && ltype != Type::NO_TYPE && rtype != Type::NO_TYPE){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid assignment statement - type mismatch: expected '{}', got '{}'", 
                relationalExpr->getToken().line, relationalExpr->getToken().column, typeToString.at(ltype), typeToString.at(rtype))
        );
    }
    binaryExpr->setType(ltype);
}

bool ExpressionAnalyzer::checkIDExpr(ASTIdExpr* idExpr){
    std::string name{ idExpr->getToken().value };
    auto& analyzerContext = FunctionAnalyzer::getContext();
    
    // check if id exists
    if(!analyzerContext.scopeManager->lookupSymbol(name, {Kind::VAR, Kind::PAR})){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> undefined variable '{}'", 
                idExpr->getToken().line, idExpr->getToken().column, name)
        );
        idExpr->setType(Type::NO_TYPE);
        return false;
    }
    idExpr->setType(analyzerContext.scopeManager->getSymbol(name).getType());
    return true;
}

void ExpressionAnalyzer::checkLiteralExpr(const ASTLiteralExpr* literalExpr) const {
    auto& analyzerContext = FunctionAnalyzer::getContext();
    std::string name{ literalExpr->getToken().value };
    // invalid literal check
    if((literalExpr->getType() == Type::UNSIGNED && (name.back() != 'u' || name.front() == '-')) || (literalExpr->getType() == Type::INT && name.back() == 'u')){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid literal '{}'", 
                literalExpr->getToken().line, literalExpr->getToken().column, name)
        );
    }
}

void ExpressionAnalyzer::checkFunctionCallExpr(ASTFunctionCallExpr* callExpr){
    // function existence
    auto& analyzerContext = FunctionAnalyzer::getContext();
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
    checkArguments(callExpr);
}

void ExpressionAnalyzer::checkArguments(const ASTFunctionCallExpr* callExpr){
    const std::vector<std::unique_ptr<ASTParameter>>* functionParameters{ globalScopeManager.getSymbol(callExpr->getToken().value).getParameters() };
    const std::vector<std::unique_ptr<ASTExpr>>& arguments{ callExpr->getArguments() };

    for(size_t i = 0; i < callExpr->getArgumentCount(); ++i){
        checkNumericalExpr(arguments[i].get());

        // type check of corresponding parameters in function and function call
        Type ltype{ arguments[i]->getType() };
        Type rtype{ (*functionParameters)[i]->getType() };

        // no point checking for types if argument is invalid
        if(ltype == Type::NO_TYPE) return;

        auto& analyzerContext = FunctionAnalyzer::getContext();
        if(ltype != rtype){
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid argument - type mismatch: expected '{}', got '{} {}'",
                    arguments[i]->getToken().line, arguments[i]->getToken().column, typeToString.at(rtype), typeToString.at(ltype), arguments[i]->getToken().value)
            );
        }
    }
}