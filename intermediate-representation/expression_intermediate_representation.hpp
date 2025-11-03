#ifndef EXPRESSION_INTERMEDIATE_REPRESENTATION_HPP
#define EXPRESSION_INTERMEDIATE_REPRESENTATION_HPP

#include <memory>

#include "../common/abstract-syntax-tree/ASTExpression.hpp"
#include "../common/abstract-syntax-tree/ASTBinaryExpression.hpp"
#include "../common/abstract-syntax-tree/ASTId.hpp"
#include "../common/abstract-syntax-tree/ASTLiteral.hpp"
#include "../common/abstract-syntax-tree/ASTFunctionCall.hpp"

#include "../common/intermediate-representation-tree/IRExpression.hpp"
#include "../common/intermediate-representation-tree/IRLiteral.hpp"
#include "../common/intermediate-representation-tree/IRBinaryExpression.hpp"
#include "../common/intermediate-representation-tree/IRId.hpp"
#include "../common/intermediate-representation-tree/IRFunctionCall.hpp"

#include "../optimization/constant_folding.hpp"

class ExpressionIntermediateRepresentation {
public:
    ExpressionIntermediateRepresentation();

    std::unique_ptr<IRExpression> numericalExpression(const ASTExpression* _numexp);
    std::unique_ptr<IRBinaryExpression> relationalExpression(const ASTExpression* _relexp);
    std::unique_ptr<IRId> id(const ASTId* _id) const;
    std::unique_ptr<IRLiteral> literal(const ASTLiteral* _literal) const;

    std::unique_ptr<IRTemporary> initiateTemporaries(const ASTExpression* _numexp);

private:
    std::unique_ptr<IRFunctionCall> functionCall(const ASTFunctionCall* _functionCall);
    void argument(IRFunctionCall* _irFunctionCall, const ASTFunctionCall* _functionCall);

    // assigning function calls to temporary variables, preventing register corruption
    size_t countTemporaries(const ASTExpression* _numexp) const;
    std::string generateTemporaries();
    void assignFunctionCalls(IRTemporary* _temporaryRoot, const ASTExpression* _numexp, size_t& idx);
    std::unique_ptr<IRId> replaceFunctionCall(const ASTFunctionCall* _functionCall);


    // helper function to get operand value as T
    template<typename T>
    T getOperandValue(const IRLiteral* operand) const {
        if (std::is_same<T, int>::value) {
            return static_cast<T>(std::stoi(operand->getValue()));
        } else {
            return static_cast<T>(std::stoul(operand->getValue()));
        }
    }
    
    // reducing the depth of the tree if both operands are literals
    template<typename T>
    Optimization::ConstantFolding::MergeResult<std::unique_ptr<IRExpression>> mergeLiterals(const IRLiteral* leftOperand, const IRLiteral* rightOperand, const ASTBinaryExpression* binExp) const {
        T lval = getOperandValue<T>(leftOperand);
        T rval = getOperandValue<T>(rightOperand);
        Optimization::ConstantFolding::MergeResult<T> res{ 
            Optimization::ConstantFolding::mergeValues<T>(
                lval, rval, binExp->getOperator(), binExp->getToken().line, binExp->getToken().column
            ) 
        };

        Types type{ std::is_same<T, int>::value ? Types::INT : Types::UNSIGNED };
        std::string suffix{ type == Types::INT ? "" : "u" };

        Optimization::ConstantFolding::MergeResult<std::unique_ptr<IRExpression>> foldedExpr {
            .result = std::make_unique<IRLiteral>(IRNodeType::LITERAL, std::to_string(res.result) + suffix, type),
            .error = res.error
        };

        return foldedExpr;
    }
};

#endif