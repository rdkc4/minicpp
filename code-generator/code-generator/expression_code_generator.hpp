#ifndef EXPRESSION_CODE_GENERATOR_HPP
#define EXPRESSION_CODE_GENERATOR_HPP

#include "../../common/intermediate-representation-tree/IRExpression.hpp"
#include "../../common/intermediate-representation-tree/IRId.hpp"
#include "../../common/intermediate-representation-tree/IRLiteral.hpp"
#include "../../common/intermediate-representation-tree/IRFunctionCall.hpp"
#include "../../common/intermediate-representation-tree/IRTemporary.hpp"

class ExpressionCodeGenerator {
public:
    void generateNumericalExpression(const IRExpression* _numexp);
    void generateRelationalExpression(const IRExpression* _relexp);
    const std::string& generateID(const IRId* _id) const;
    std::string generateLiteral(const IRLiteral* _literal) const;

    void generateTemporaries(const IRTemporary* _temporary);

private:
    void generateFunctionCall(const IRFunctionCall* _functionCall);
    void generateArgument(const IRFunctionCall* _arguments);
    void clearArguments(size_t _arguments);

};

#endif