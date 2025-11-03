#ifndef INTERMEDIATE_REPRESENTATION_TEST_HPP
#define INTERMEDIATE_REPRESENTATION_TEST_HPP

#include <cassert>
#include <memory>

#include "../../intermediate-representation/intermediate_representation.hpp"

class IntermediateRepresentationTest : public IntermediateRepresentation {
    public:
        const std::vector<std::string>& getErrors(const std::string& func) const noexcept {
            assert(exceptions.find(func) != exceptions.end());
            return exceptions.at(func);
        }
};

class FunctionIntermediateRepresentationTest : public FunctionIntermediateRepresentation {
    public:
        std::unique_ptr<IRFunction> testFunction(const ASTFunction* _function) {
            return function(_function);
        }

};

class StatementIntermediateRepresentationTest : public StatementIntermediateRepresentation {
    public:
        std::unique_ptr<IRCompoundSt> testCompoundStatement(const ASTCompoundSt* _compound){
            return compoundStatement(_compound);
        }

        std::unique_ptr<IRAssignSt> testAssignmentStatement(const ASTAssignSt* _assign){
            return assignmentStatement(_assign);
        }

        std::unique_ptr<IRSwitchSt> testSwitchStatement(const ASTSwitchSt* _switch){
            return switchStatement(_switch);
        }
};

class ExpressionIntermediateRepresentationTest : public ExpressionIntermediateRepresentation {
    public:
        std::unique_ptr<IRExpression> testNumericalExpression(ASTExpression* _expression){
            return numericalExpression(_expression);
        }
};

#endif