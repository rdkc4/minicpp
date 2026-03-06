#include "../expression_intermediate_representation.hpp"
#include "../function_intermediate_representation.hpp"

#include <cassert>

ExpressionIntermediateRepresentation::ExpressionIntermediateRepresentation() = default; 

// NUMEXP - reduced to (id, literal, function call) or arithmetic operation (add, sub, mul, div)
std::unique_ptr<IRExpr> ExpressionIntermediateRepresentation::numericalExpression(const ASTExpr* _numexp){
    if(_numexp->getNodeType() == ASTNodeType::ID){
        return id(static_cast<const ASTIdExpr*>(_numexp));
    }
    else if(_numexp->getNodeType() == ASTNodeType::LITERAL){
        return literal(static_cast<const ASTLiteralExpr*>(_numexp));
    }
    else if(_numexp->getNodeType() == ASTNodeType::FUNCTION_CALL){
        return replaceFunctionCall(static_cast<const ASTFunctionCallExpr*>(_numexp));
    }
    else{
        const ASTBinaryExpr* binExp = static_cast<const ASTBinaryExpr*>(_numexp);
        std::unique_ptr<IRExpr> leftOperand{ numericalExpression(binExp->getLeftOperand()) };
        std::unique_ptr<IRExpr> rightOperand{ numericalExpression(binExp->getRightOperand()) };

        if(leftOperand->getNodeType() == IRNodeType::LITERAL && rightOperand->getNodeType() == IRNodeType::LITERAL){
            if(leftOperand->getType() == Type::INT){
                auto res = mergeLiterals<int>(static_cast<const IRLiteralExpr*>(leftOperand.get()), 
                    static_cast<const IRLiteralExpr*>(rightOperand.get()), binExp
                );
                if(!res.error.empty()){
                    auto& irContext = FunctionIntermediateRepresentation::getContext();
                    irContext.errors.push_back(res.error);
                }
                return std::move(res.result);
            }
            else if(leftOperand->getType() == Type::UNSIGNED){
                auto res = mergeLiterals<unsigned>(static_cast<const IRLiteralExpr*>(leftOperand.get()), 
                    static_cast<const IRLiteralExpr*>(rightOperand.get()), binExp
                );
                if(!res.error.empty()){
                    auto& irContext = FunctionIntermediateRepresentation::getContext();
                    irContext.errors.push_back(res.error);
                }
                return std::move(res.result);
            }
        }
        
        std::string val{ _numexp->getToken().value };
        Type type{ leftOperand->getType() };
        IRNodeType iNodeType{ operatorToIRNodeType.at(binExp->getOperator()).getOperation(type) };

        std::unique_ptr<IRBinaryExpr> _irBinExp = std::make_unique<IRBinaryExpr>(iNodeType, type);
        _irBinExp->setBinaryExpression(std::move(leftOperand), std::move(rightOperand), binExp->getOperator());

        return _irBinExp;
    }
}

std::unique_ptr<IRBinaryExpr> ExpressionIntermediateRepresentation::relationalExpression(const ASTExpr* _relexp) {
    const ASTBinaryExpr* _astBinExp = static_cast<const ASTBinaryExpr*>(_relexp);
    
    IRNodeType irNodeType{ operatorToIRNodeType.at(_astBinExp->getOperator()).getOperation(_relexp->getType()) };
    std::unique_ptr<IRBinaryExpr> _irBinExp = std::make_unique<IRBinaryExpr>(irNodeType, _astBinExp->getType());

    _irBinExp->setBinaryExpression(numericalExpression(_astBinExp->getLeftOperand()), numericalExpression(_astBinExp->getRightOperand()), _astBinExp->getOperator());
    return _irBinExp;
}

std::unique_ptr<IRIdExpr> ExpressionIntermediateRepresentation::id(const ASTIdExpr* _id) const {
    return std::make_unique<IRIdExpr>(IRNodeType::ID, _id->getToken().value, _id->getType());
}

std::unique_ptr<IRLiteralExpr> ExpressionIntermediateRepresentation::literal(const ASTLiteralExpr* _literal) const {
    return std::make_unique<IRLiteralExpr>(IRNodeType::LITERAL, _literal->getToken().value, _literal->getType());
}

std::unique_ptr<IRFunctionCallExpr> ExpressionIntermediateRepresentation::functionCall(const ASTFunctionCallExpr* _functionCall){
    std::unique_ptr<IRFunctionCallExpr> _irFunctionCall = 
        std::make_unique<IRFunctionCallExpr>(IRNodeType::CALL, _functionCall->getToken().value, _functionCall->getType());
    argument(_irFunctionCall.get(), _functionCall);

    return _irFunctionCall;
}

void ExpressionIntermediateRepresentation::argument(IRFunctionCallExpr* _irFunctionCall, const ASTFunctionCallExpr* _functionCall){
    for(const auto& argument : _functionCall->getArguments()){
        auto temps{ initiateTemporaries(argument.get()) };
        _irFunctionCall->addArgument(numericalExpression(argument.get()), std::move(temps));
    }
}

// counting the number of function calls that should be replaced by temporary variables
size_t ExpressionIntermediateRepresentation::countTemporaries(const ASTExpr* _numexp) const {
    if(_numexp->getNodeType() == ASTNodeType::FUNCTION_CALL){
        return 1;
    }
    else if(_numexp->getNodeType() == ASTNodeType::LITERAL || _numexp->getNodeType() == ASTNodeType::ID || _numexp->getNodeType() == ASTNodeType::VARIABLE){
        return 0;
    }
    else{
        const ASTBinaryExpr* binExp = static_cast<const ASTBinaryExpr*>(_numexp); 
        return countTemporaries(binExp->getLeftOperand()) + countTemporaries(binExp->getRightOperand());
    }
}

// generating temporary variables
std::string ExpressionIntermediateRepresentation::generateTemporaries(){
    auto& irContext = FunctionIntermediateRepresentation::getContext();
    std::string name = std::format("_t{}", ++irContext.temporaries);
    irContext.temporaryNames.push(name);
    return name;
}

// assigning a returned value to temporary variables
void ExpressionIntermediateRepresentation::assignFunctionCalls(IRTemporaryExpr* _temporaryRoot, const ASTExpr* _numexp, size_t& idx){
    if(_numexp->getNodeType() == ASTNodeType::FUNCTION_CALL){
        const ASTFunctionCallExpr* funcCall = static_cast<const ASTFunctionCallExpr*>(_numexp);

        _temporaryRoot->assignTempAtN(functionCall(funcCall), _numexp->getType(), idx++);
    }
    else if(_numexp->getNodeType() == ASTNodeType::LITERAL || _numexp->getNodeType() == ASTNodeType::VARIABLE || _numexp->getNodeType() == ASTNodeType::ID){
        return;
    }
    else{
        const ASTBinaryExpr* binExp = static_cast<const ASTBinaryExpr*>(_numexp);
        assignFunctionCalls(_temporaryRoot, binExp->getLeftOperand(), idx);
        assignFunctionCalls(_temporaryRoot, binExp->getRightOperand(), idx);
    }
}

// replacing function calls with temporary variables in numerical expression
std::unique_ptr<IRIdExpr> ExpressionIntermediateRepresentation::replaceFunctionCall(const ASTFunctionCallExpr* _functionCall){
    auto& irContext = FunctionIntermediateRepresentation::getContext(); 
    assert(!irContext.temporaryNames.empty());
    std::string name = irContext.temporaryNames.top();
    irContext.temporaryNames.pop();
    return std::make_unique<IRIdExpr>(IRNodeType::ID, name, _functionCall->getType());
}

std::unique_ptr<IRTemporaryExpr> ExpressionIntermediateRepresentation::initiateTemporaries(const ASTExpr* _numexp){
    size_t tmpCount = countTemporaries(_numexp);
    if(tmpCount > 0){
        std::unique_ptr<IRTemporaryExpr> temporaryRoot = std::make_unique<IRTemporaryExpr>(IRNodeType::TEMPORARY);
        size_t firstTemporaryIndex = 0;
        for(size_t i = 0; i < tmpCount; ++i){
            temporaryRoot->addTemporary(generateTemporaries());
        }
        assignFunctionCalls(temporaryRoot.get(), _numexp, firstTemporaryIndex);
        return temporaryRoot;
    }
    return nullptr;
}