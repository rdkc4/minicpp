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
};

class FunctionIntermediateRepresentationTest : public FunctionIntermediateRepresentation {

};

class StatementIntermediateRepresentationTest : public StatementIntermediateRepresentation {

};

class ExpressionIntermediateRepresentationTest : public ExpressionIntermediateRepresentation {

};

#endif