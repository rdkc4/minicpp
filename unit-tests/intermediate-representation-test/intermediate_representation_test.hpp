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

        std::unique_ptr<IRTree> testFunction(const ASTree* _function){
            return function(_function);
        }

        std::unique_ptr<IRTree> testCompoundStatement(const ASTree* _compound){
            return compoundStatement(_compound);
        }

        std::unique_ptr<IRTree> testAssignmentStatement(const ASTree* _assignment){
            return assignmentStatement(_assignment);
        }

        std::unique_ptr<IRTree> testSwitchStatement(const ASTree* _switch){
            return switchStatement(_switch);
        }
        
        std::unique_ptr<IRTree> testNumericalExpression(const ASTree* _numexp){
            return numericalExpression(_numexp);
        }

};


#endif