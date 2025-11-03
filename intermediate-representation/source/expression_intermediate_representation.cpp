#include "../expression_intermediate_representation.hpp"
#include "../function_intermediate_representation.hpp"

#include <cassert>

ExpressionIntermediateRepresentation::ExpressionIntermediateRepresentation() = default; 

// NUMEXP - reduced to (id, literal, function call) or arithmetic operation (add, sub, mul, div)
std::unique_ptr<IRExpression> ExpressionIntermediateRepresentation::numericalExpression(const ASTExpression* _numexp){
    if(_numexp->getNodeType() == ASTNodeType::ID){
        return id(static_cast<const ASTId*>(_numexp));
    }
    else if(_numexp->getNodeType() == ASTNodeType::LITERAL){
        return literal(static_cast<const ASTLiteral*>(_numexp));
    }
    else if(_numexp->getNodeType() == ASTNodeType::FUNCTION_CALL){
        return replaceFunctionCall(static_cast<const ASTFunctionCall*>(_numexp));
    }
    else{
        const ASTBinaryExpression* binExp = static_cast<const ASTBinaryExpression*>(_numexp);
        std::unique_ptr<IRExpression> leftOperand{ numericalExpression(binExp->getLeftOperand()) };
        std::unique_ptr<IRExpression> rightOperand{ numericalExpression(binExp->getRightOperand()) };

        if(leftOperand->getNodeType() == IRNodeType::LITERAL && rightOperand->getNodeType() == IRNodeType::LITERAL){
            if(leftOperand->getType() == Types::INT){
                auto res = mergeLiterals<int>(static_cast<const IRLiteral*>(leftOperand.get()), 
                    static_cast<const IRLiteral*>(rightOperand.get()), binExp
                );
                if(!res.error.empty()){
                    auto& irContext = FunctionIntermediateRepresentation::getContext();
                    irContext.errors.push_back(res.error);
                }
                return std::move(res.result);
            }
            else if(leftOperand->getType() == Types::UNSIGNED){
                auto res = mergeLiterals<unsigned>(static_cast<const IRLiteral*>(leftOperand.get()), 
                    static_cast<const IRLiteral*>(rightOperand.get()), binExp
                );
                if(!res.error.empty()){
                    auto& irContext = FunctionIntermediateRepresentation::getContext();
                    irContext.errors.push_back(res.error);
                }
                return std::move(res.result);
            }
        }
        
        std::string val{ _numexp->getToken().value };
        Types type{ leftOperand->getType() };
        IRNodeType iNodeType{ operatorToIRNodeType.at(binExp->getOperator()).getOperation(type) };

        std::unique_ptr<IRBinaryExpression> _irBinExp = std::make_unique<IRBinaryExpression>(iNodeType, type);
        _irBinExp->setBinaryExpression(std::move(leftOperand), std::move(rightOperand), binExp->getOperator());

        return _irBinExp;
    }
}

std::unique_ptr<IRBinaryExpression> ExpressionIntermediateRepresentation::relationalExpression(const ASTExpression* _relexp) {
    const ASTBinaryExpression* _astBinExp = static_cast<const ASTBinaryExpression*>(_relexp);
    
    IRNodeType irNodeType{ operatorToIRNodeType.at(_astBinExp->getOperator()).getOperation(_relexp->getType()) };
    std::unique_ptr<IRBinaryExpression> _irBinExp = std::make_unique<IRBinaryExpression>(irNodeType, _astBinExp->getType());

    _irBinExp->setBinaryExpression(numericalExpression(_astBinExp->getLeftOperand()), numericalExpression(_astBinExp->getRightOperand()), _astBinExp->getOperator());
    return _irBinExp;
}

std::unique_ptr<IRId> ExpressionIntermediateRepresentation::id(const ASTId* _id) const {
    return std::make_unique<IRId>(IRNodeType::ID, _id->getToken().value, _id->getType());
}

std::unique_ptr<IRLiteral> ExpressionIntermediateRepresentation::literal(const ASTLiteral* _literal) const {
    return std::make_unique<IRLiteral>(IRNodeType::LITERAL, _literal->getToken().value, _literal->getType());
}

std::unique_ptr<IRFunctionCall> ExpressionIntermediateRepresentation::functionCall(const ASTFunctionCall* _functionCall){
    std::unique_ptr<IRFunctionCall> _irFunctionCall = 
        std::make_unique<IRFunctionCall>(IRNodeType::CALL, _functionCall->getToken().value, _functionCall->getType());
    argument(_irFunctionCall.get(), _functionCall);

    return _irFunctionCall;
}

void ExpressionIntermediateRepresentation::argument(IRFunctionCall* _irFunctionCall, const ASTFunctionCall* _functionCall){
    for(const auto& argument : _functionCall->getArguments()){
        auto temps{ initiateTemporaries(argument.get()) };
        _irFunctionCall->addArgument(numericalExpression(argument.get()), std::move(temps));
    }
}

// counting the number of function calls that should be replaced by temporary variables
size_t ExpressionIntermediateRepresentation::countTemporaries(const ASTExpression* _numexp) const {
    if(_numexp->getNodeType() == ASTNodeType::FUNCTION_CALL){
        return 1;
    }
    else if(_numexp->getNodeType() == ASTNodeType::LITERAL || _numexp->getNodeType() == ASTNodeType::ID || _numexp->getNodeType() == ASTNodeType::VARIABLE){
        return 0;
    }
    else{
        const ASTBinaryExpression* binExp = static_cast<const ASTBinaryExpression*>(_numexp); 
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
void ExpressionIntermediateRepresentation::assignFunctionCalls(IRTemporary* _temporaryRoot, const ASTExpression* _numexp, size_t& idx){
    if(_numexp->getNodeType() == ASTNodeType::FUNCTION_CALL){
        const ASTFunctionCall* funcCall = static_cast<const ASTFunctionCall*>(_numexp);

        _temporaryRoot->assignTempAtN(functionCall(funcCall), _numexp->getType(), idx++);
    }
    else if(_numexp->getNodeType() == ASTNodeType::LITERAL || _numexp->getNodeType() == ASTNodeType::VARIABLE || _numexp->getNodeType() == ASTNodeType::ID){
        return;
    }
    else{
        const ASTBinaryExpression* binExp = static_cast<const ASTBinaryExpression*>(_numexp);
        assignFunctionCalls(_temporaryRoot, binExp->getLeftOperand(), idx);
        assignFunctionCalls(_temporaryRoot, binExp->getRightOperand(), idx);
    }
}

// replacing function calls with temporary variables in numerical expression
std::unique_ptr<IRId> ExpressionIntermediateRepresentation::replaceFunctionCall(const ASTFunctionCall* _functionCall){
    auto& irContext = FunctionIntermediateRepresentation::getContext(); 
    assert(!irContext.temporaryNames.empty());
    std::string name = irContext.temporaryNames.top();
    irContext.temporaryNames.pop();
    return std::make_unique<IRId>(IRNodeType::ID, name, _functionCall->getType());
}

std::unique_ptr<IRTemporary> ExpressionIntermediateRepresentation::initiateTemporaries(const ASTExpression* _numexp){
    size_t tmpCount = countTemporaries(_numexp);
    if(tmpCount > 0){
        std::unique_ptr<IRTemporary> temporaryRoot = std::make_unique<IRTemporary>(IRNodeType::TEMPORARY);
        size_t firstTemporaryIndex = 0;
        for(size_t i = 0; i < tmpCount; ++i){
            temporaryRoot->addTemporary(generateTemporaries());
        }
        assignFunctionCalls(temporaryRoot.get(), _numexp, firstTemporaryIndex);
        return temporaryRoot;
    }
    return nullptr;
}