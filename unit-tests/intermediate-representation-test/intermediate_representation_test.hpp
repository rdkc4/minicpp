#ifndef INTERMEDIATE_REPRESENTATION_TEST_HPP
#define INTERMEDIATE_REPRESENTATION_TEST_HPP

#include <cassert>

#include "../../intermediate-representation/intermediate_representation.hpp"

class IntermediateRepresentationTest : public IntermediateRepresentation {
    public:
        const std::vector<std::string>& getErrors(const std::string& func) const noexcept {
            assert(exceptions.find(func) != exceptions.end());
            return exceptions.at(func);
        }

        IRThreadContext& getIRContext() const noexcept {
            return irContext;
        }

        std::unique_ptr<IRFunction> testFunction(const ASTFunction* _function){
            return function(_function);
        }

        std::unique_ptr<IRCompoundSt> testCompoundStatement(const ASTCompoundSt* _compound){
            return compoundStatement(_compound);
        }

        std::unique_ptr<IRAssignSt> testAssignmentStatement(const ASTAssignSt* _assignment){
            return assignmentStatement(_assignment);
        }

        std::unique_ptr<IRSwitchSt> testSwitchStatement(const ASTSwitchSt* _switch){
            return switchStatement(_switch);
        }
        
        std::unique_ptr<IRExpression> testNumericalExpression(const ASTExpression* _numexp){
            return numericalExpression(_numexp);
        }

};


#endif