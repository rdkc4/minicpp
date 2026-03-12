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
    ExpressionIntermediateRepresentation() = default;

    /**
     * @brief turns ast numerical expression into irt numerical expression
     * @param astNumericalExpr - const pointer to the ast numerical expression
     * @returns pointer to the irt numerical expression
    */
    std::unique_ptr<IRExpr> transformNumericalExpr(const ASTExpr* astNumericalExpr);

    /**
     * @brief turns ast binary expression into irt binary expression
     * @param astBinaryExpr - const pointer to the ast binary expression
     * @returns pointer to the irt expression
    */
    std::unique_ptr<IRExpr> transformBinaryExpr(const ASTBinaryExpr* astBinaryExpr);

    /**
     * @brief turns ast relational expression into irt relational expression
     * @param astRelationalExpr - const pointer to the ast relational expression
     * @returns pointer to the irt relational expression
     * @note at the moment only binary expression
    */
    std::unique_ptr<IRBinaryExpr> transformRelationalExpr(const ASTExpr* astRelationalExpr);

    /**
     * @brief turns ast id into irt id
     * @param astIdExpr - const pointer to the ast id
     * @returns pointer to the irt id
    */
    std::unique_ptr<IRIdExpr> transformIdExpr(const ASTIdExpr* astIdExpr) const;

    /**
     * @brief turns ast literal into irt literal
     * @param astLiteralExpr - const pointer to the ast literal
     * @returns pointer to the irt literal
    */
    std::unique_ptr<IRLiteralExpr> transformLiteralExpr(const ASTLiteralExpr* astLiteralExpr) const;

    /**
     * @brief generates temporary variables for runction calls of the numerical expression
     * @param astExpr - const pointer to the ast numerical expression
     * @returns pointer to the temporary variables
    */
    std::unique_ptr<IRTemporaryExpr> initiateTemporaries(const ASTExpr* astExpr);

    /**
     * @brief turns ast function call into irt function call
     * @param astCallExpr - const pointer to the ast function call
     * @returns pointer to the irt function call
    */
    std::unique_ptr<IRFunctionCallExpr> transformFunctionCallExpr(const ASTFunctionCallExpr* astCallExpr);

    /**
     * @brief turns arguments of the ast function call into arguments of the irt function call
     * @param irCallExpr - pointer to the irt function call
     * @param astCallExpr - const pointer to the ast function call
    */
    void transformArguments(IRFunctionCallExpr* irCallExpr, const ASTFunctionCallExpr* astCallExpr);

    /**
     * @brief counting the number of required temporaries
     * @param astExpr - const pointer to the ast expression
     * @returns number of required temporaries
    */
    size_t countTemporaries(const ASTExpr* astExpr) const;

    /**
     * @brief generates the name for the temporary variable
     * @returns name of the temporary variable
    */
    std::string generateTemporaries();

    /**
     * @brief assigns function calls to the temporaries
     * @param temporaryRoot - pointer to the parent node of the temporaries
     * @param astExpr - const pointer to the ast expression
     * @param idx - index of the temporary being assigned
    */
    void assignTemporaries(IRTemporaryExpr* temporaryRoot, const ASTExpr* astExpr, size_t& idx);

    /**
     * @brief replaces function calls with temporaries
     * @param astCallExpr - const pointer to the ast function call
     * @returns pointer to the irt id of the temporary variable
    */
    std::unique_ptr<IRIdExpr> replaceFunctionCallExpr(const ASTFunctionCallExpr* astCallExpr);

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
            .result = std::make_unique<IRLiteralExpr>(std::to_string(res.result) + suffix, type),
            .error = res.error
        };

        return foldedExpr;
    }
};

#endif
