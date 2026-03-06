#ifndef EXPRESSION_INTERMEDIATE_REPRESENTATION_HPP
#define EXPRESSION_INTERMEDIATE_REPRESENTATION_HPP

#include <memory>

#include "../common/abstract-syntax-tree/ast_expr.hpp"
#include "../common/abstract-syntax-tree/ast_binary_expr.hpp"
#include "../common/abstract-syntax-tree/ast_id_expr.hpp"
#include "../common/abstract-syntax-tree/ast_literal_expr.hpp"
#include "../common/abstract-syntax-tree/ast_function_call_expr.hpp"

#include "../common/intermediate-representation-tree/ir_expr.hpp"
#include "../common/intermediate-representation-tree/ir_literal_expr.hpp"
#include "../common/intermediate-representation-tree/ir_binary_expr.hpp"
#include "../common/intermediate-representation-tree/ir_id_expr.hpp"
#include "../common/intermediate-representation-tree/ir_function_call_expr.hpp"

#include "../optimization/constant_folding.hpp"

/**
 * @class ExpressionIntermediateRepresentation
 * @brief turns ast expression into irt expression
*/
class ExpressionIntermediateRepresentation {
public:
    /** 
     * @brief Creates the instance of the expression intermediate representation
    */
    ExpressionIntermediateRepresentation();

    /**
     * @brief turns ast numerical expression into irt numerical expression
     * @param _numexp - const pointer to the ast numerical expression
     * @returns pointer to the irt numerical expression
    */
    std::unique_ptr<IRExpr> numericalExpression(const ASTExpr* _numexp);

    /**
     * @brief turns ast relational expression into irt relational expression
     * @param _relexp - const pointer to the ast relational expression
     * @returns pointer to the irt relational expression
     * @note at the moment only binary expression
    */
    std::unique_ptr<IRBinaryExpr> relationalExpression(const ASTExpr* _relexp);

    /**
     * @brief turns ast id into irt id
     * @param _id - const pointer to the ast id
     * @returns pointer to the irt id
    */
    std::unique_ptr<IRIdExpr> id(const ASTIdExpr* _id) const;

    /**
     * @brief turns ast literal into irt literal
     * @param _literal - const pointer to the ast literal
     * @returns pointer to the irt literal
    */
    std::unique_ptr<IRLiteralExpr> literal(const ASTLiteralExpr* _literal) const;

    /**
     * @brief generates temporary variables for runction calls of the numerical expression
     * @param _numexp - const pointer to the ast numerical expression
     * @returns pointer to the temporary variables
    */
    std::unique_ptr<IRTemporaryExpr> initiateTemporaries(const ASTExpr* _numexp);

    /**
     * @brief turns ast function call into irt function call
     * @param _functionCall - const pointer to the ast function call
     * @returns pointer to the irt function call
    */
    std::unique_ptr<IRFunctionCallExpr> functionCall(const ASTFunctionCallExpr* _functionCall);

    /**
     * @brief turns arguments of the ast function call into arguments of the irt function call
     * @param _irFunctionCall - pointer to the irt function call
     * @param _functionCall - const pointer to the ast function call
    */
    void argument(IRFunctionCallExpr* _irFunctionCall, const ASTFunctionCallExpr* _functionCall);

    /**
     * @brief counting the number of required temporaries
     * @param _numexp - const pointer to the ast numerical expression
     * @returns number of required temporaries
    */
    size_t countTemporaries(const ASTExpr* _numexp) const;

    /**
     * @brief generates the name for the temporary variable
     * @returns name of the temporary variable
    */
    std::string generateTemporaries();

    /**
     * @brief assigns function calls to the temporaries
     * @param _temporaryRoot - pointer to the parent node of the temporaries
     * @param _numexp - const pointer to the ast numerical expression
     * @param idx - index of the temporary being assigned
    */
    void assignFunctionCalls(IRTemporaryExpr* _temporaryRoot, const ASTExpr* _numexp, size_t& idx);

    /**
     * @brief replaces function calls with temporaries
     * @param _functionCall - const pointer to the ast function call
     * @returns pointer to the irt id of the temporary variable
    */
    std::unique_ptr<IRIdExpr> replaceFunctionCall(const ASTFunctionCallExpr* _functionCall);


    /**
     * @brief helper function to get operand value
     * @returns operand value as T
    */
    template<typename T>
    T getOperandValue(const IRLiteralExpr* operand) const {
        if (std::is_same<T, int>::value) {
            return static_cast<T>(std::stoi(operand->getValue()));
        } else {
            return static_cast<T>(std::stoul(operand->getValue()));
        }
    }
    
    /**
     * @brief merges literals of the expression
     * @note reduces the depth of the expression subtree when all children are literals
     * @param leftOperand - const pointer to the irt numerical expression
     * @param rightOperand - const pointer to the irt numerical expression
     * @param binExp - const pointer to the ast binary expression, contains operation
     * @returns result of the merge operation
    */
    template<typename T>
    Optimization::ConstantFolding::MergeResult<std::unique_ptr<IRExpr>> mergeLiterals(const IRLiteralExpr* leftOperand, const IRLiteralExpr* rightOperand, const ASTBinaryExpr* binExp) const {
        T lval = getOperandValue<T>(leftOperand);
        T rval = getOperandValue<T>(rightOperand);
        Optimization::ConstantFolding::MergeResult<T> res{ 
            Optimization::ConstantFolding::mergeValues<T>(
                lval, rval, binExp->getOperator(), binExp->getToken().line, binExp->getToken().column
            ) 
        };

        Type type{ std::is_same<T, int>::value ? Type::INT : Type::UNSIGNED };
        std::string suffix{ type == Type::INT ? "" : "u" };

        Optimization::ConstantFolding::MergeResult<std::unique_ptr<IRExpr>> foldedExpr {
            .result = std::make_unique<IRLiteralExpr>(IRNodeType::LITERAL, std::to_string(res.result) + suffix, type),
            .error = res.error
        };

        return foldedExpr;
    }
};

#endif
