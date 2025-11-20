#ifndef EXPRESSION_CODE_GENERATOR_HPP
#define EXPRESSION_CODE_GENERATOR_HPP

#include "../../common/intermediate-representation-tree/IRExpression.hpp"
#include "../../common/intermediate-representation-tree/IRId.hpp"
#include "../../common/intermediate-representation-tree/IRLiteral.hpp"
#include "../../common/intermediate-representation-tree/IRFunctionCall.hpp"
#include "../../common/intermediate-representation-tree/IRTemporary.hpp"

/** 
 * @class ExpressionCodeGenerator
 * @brief code generator specialized for expressions
*/
class ExpressionCodeGenerator {
public:
    /** 
     * @brief Creates the instance of the expression code generator
    */
    ExpressionCodeGenerator();

    /** 
     * @brief generates the asm code for the numerical expression
     * @param _numexp - const pointer to the irt numerical expression
     * @returns void
    */
    void generateNumericalExpression(const IRExpression* _numexp);

    /** 
     * @brief generates the asm code for the relational expression
     * @param _relexp - const pointer to the irt relational expression (binary expression only at the moment)
     * @returns void
    */
    void generateRelationalExpression(const IRExpression* _relexp);

    /** 
     * @brief gets the address of the variable on the stack
     * @param _id - const pointer to the irt id
     * @returns address of the variable on the stack
    */
    const std::string& generateID(const IRId* _id) const;

    /** 
     * @brief gets the literal
     * @param _literal - const pointer to irt literal
     * @returns literal
    */
    std::string generateLiteral(const IRLiteral* _literal) const;

    /** 
     * @brief generates the asm code for assigning values to temporary variables
     * @param _temporary - const irt pointer to the irt temporary
     * @returns void
    */
    void generateTemporaries(const IRTemporary* _temporary);

private:
    /** 
     * @brief generates the asm code for the function call
     * @param _functionCall - const pointer to the irt function call
     * @returns void
    */
    void generateFunctionCall(const IRFunctionCall* _functionCall);

    /** 
     * @brief generates the asm code for the arguments of the function call
     * @param _functionCall - const pointer to the irt function call
     * @returns void
    */
    void generateArgument(const IRFunctionCall* _functionCall);

    /** 
     * @brief generates the asm code for removing arguments from the stack after function call is done
     * @param _arguments - number of arguments
     * @returns void
    */
    void clearArguments(size_t _arguments);

};

#endif