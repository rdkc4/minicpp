#include "../expression_analyzer.hpp"
#include "../function_analyzer.hpp"

#include <format>
#include <string>
#include <vector>
#include <memory>

#include "../../common/abstract-syntax-tree/ast_binary_expr.hpp"

ExpressionAnalyzer::ExpressionAnalyzer(ScopeManager& scopeManager) : globalScopeManager{ scopeManager } {}

void ExpressionAnalyzer::checkNumericalExpression(ASTExpr* _numexp){
    // setting type to numexp node for easier type checking
    _numexp->setType(checkNumericalExpressionType(_numexp));
}

Type ExpressionAnalyzer::checkNumericalExpressionType(ASTExpr* _numexp){
    if(_numexp->getNodeType() == ASTNodeType::ID){
        checkID(static_cast<ASTIdExpr*>(_numexp));
        return _numexp->getType();
    }
    else if(_numexp->getNodeType() == ASTNodeType::LITERAL){
        checkLiteral(static_cast<const ASTLiteralExpr*>(_numexp));
        return _numexp->getType();
    }
    else if(_numexp->getNodeType() == ASTNodeType::FUNCTION_CALL){
        checkFunctionCall(static_cast<ASTFunctionCallExpr*>(_numexp));
        return _numexp->getType();
    } // no unary operators yet
    else{
        // numerical expression type check
        ASTBinaryExpr* binExp = static_cast<ASTBinaryExpr*>(_numexp);
        Type ltype{ checkNumericalExpressionType(binExp->getLeftOperand()) };
        Type rtype{ checkNumericalExpressionType(binExp->getRightOperand()) };

        // if either subexpression is invalid, root expression shall be invalid too, no point checking for type mismatch
        if(ltype == Type::NO_TYPE || rtype == Type::NO_TYPE) return Type::NO_TYPE;

        auto& analyzerContext = FunctionAnalyzer::getContext();
        if(ltype != rtype){
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid expression - type mismatch: expected '{}', got '{}'", 
                    _numexp->getToken().line, _numexp->getToken().column, typeToString.at(ltype), typeToString.at(rtype))
            );
            return Type::NO_TYPE;
        }
        return ltype;
    }
}

void ExpressionAnalyzer::checkRelationalExpression(ASTExpr* _relexp){
    ASTBinaryExpr* _binExp = static_cast<ASTBinaryExpr*>(_relexp);
    checkNumericalExpression(_binExp->getLeftOperand());
    checkNumericalExpression(_binExp->getRightOperand());
    
    auto& analyzerContext = FunctionAnalyzer::getContext();
    auto ltype = _binExp->getLeftOperand()->getType();
    auto rtype = _binExp->getRightOperand()->getType();

    if(ltype != rtype && ltype != Type::NO_TYPE && rtype != Type::NO_TYPE){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid assignment statement - type mismatch: expected '{}', got '{}'", 
                _relexp->getToken().line, _relexp->getToken().column, typeToString.at(ltype), typeToString.at(rtype))
        );
    }
    _binExp->setType(ltype);
}

bool ExpressionAnalyzer::checkID(ASTIdExpr* _id){
    std::string name{ _id->getToken().value };
    auto& analyzerContext = FunctionAnalyzer::getContext();
    
    // check if id exists
    if(!analyzerContext.scopeManager->lookupSymbol(name, {Kind::VAR, Kind::PAR})){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> undefined variable '{}'", 
                _id->getToken().line, _id->getToken().column, name)
        );
        _id->setType(Type::NO_TYPE);
        return false;
    }
    _id->setType(analyzerContext.scopeManager->getSymbol(name).getType());
    return true;
}

void ExpressionAnalyzer::checkLiteral(const ASTLiteralExpr* _literal) const {
    auto& analyzerContext = FunctionAnalyzer::getContext();
    std::string name{ _literal->getToken().value };
    // invalid literal check
    if((_literal->getType() == Type::UNSIGNED && (name.back() != 'u' || name.front() == '-')) || (_literal->getType() == Type::INT && name.back() == 'u')){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid literal '{}'", 
                _literal->getToken().line, _literal->getToken().column, name)
        );
    }
}

void ExpressionAnalyzer::checkFunctionCall(ASTFunctionCallExpr* _functionCall){
    // function existence
    auto& analyzerContext = FunctionAnalyzer::getContext();
    if(!globalScopeManager.lookupSymbol(_functionCall->getToken().value, {Kind::FUN})){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> undefined function '{}'", 
                _functionCall->getToken().line, _functionCall->getToken().column, _functionCall->getToken().value)
        );
        // function doesn't exist, no point checking arguments
        return;
    }
    
    // main function can't be called
    if(_functionCall->getToken().value == "main"){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> 'main' is not callable function", 
                _functionCall->getToken().line, _functionCall->getToken().column)
        );
        return;
    }

    // comparation of given parameter count with expected parameter count
    size_t expectedParams{ globalScopeManager.getSymbol(_functionCall->getToken().value).getParameters()->size() };
    if(_functionCall->getArgumentCount() != expectedParams){
        analyzerContext.semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid function call '{}': provided arguments '{}', expected '{}'", 
                _functionCall->getToken().line, _functionCall->getToken().column, _functionCall->getToken().value, _functionCall->getArgumentCount(), expectedParams)
        );
        _functionCall->setType(globalScopeManager.getSymbol(_functionCall->getToken().value).getType());
        // number of provided arguments differs from expected, no type checking
        return;
    }
    _functionCall->setType(globalScopeManager.getSymbol(_functionCall->getToken().value).getType());
    checkArgument(_functionCall);
}

void ExpressionAnalyzer::checkArgument(const ASTFunctionCallExpr* _functionCall){
    const std::vector<std::unique_ptr<ASTParameter>>* functionParameters{ globalScopeManager.getSymbol(_functionCall->getToken().value).getParameters() };
    const std::vector<std::unique_ptr<ASTExpr>>& _arguments{ _functionCall->getArguments() };

    for(size_t i = 0; i < _functionCall->getArgumentCount(); ++i){
        checkNumericalExpression(_arguments[i].get());

        // type check of corresponding parameters in function and function call
        Type ltype{ _arguments[i]->getType() };
        Type rtype{ (*functionParameters)[i]->getType() };

        // no point checking for types if argument is invalid
        if(ltype == Type::NO_TYPE) return;

        auto& analyzerContext = FunctionAnalyzer::getContext();
        if(ltype != rtype){
            analyzerContext.semanticErrors.push_back(
                std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid argument - type mismatch: expected '{}', got '{} {}'",
                    _arguments[i]->getToken().line, _arguments[i]->getToken().column, typeToString.at(rtype), typeToString.at(ltype), _arguments[i]->getToken().value)
            );
        }
    }
}