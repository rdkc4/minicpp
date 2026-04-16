#ifndef INTERMEDIATE_REPRESENTATION_TEST_HPP
#define INTERMEDIATE_REPRESENTATION_TEST_HPP

#include <cassert>

#include "../../intermediate-representation/intermediate_representation.hpp"
#include "../../intermediate-representation/function_intermediate_representation.hpp"

class IntermediateRepresentationTest : public IR::IntermediateRepresentation {
    public:
        IntermediateRepresentationTest(ThreadPool& threadPool) : IntermediateRepresentation{ threadPool } {}

        const std::vector<std::string>& getErrors(const std::string& func) const noexcept {
            assert(exceptions.find(func) != exceptions.end());
            return exceptions.at(func);
        }
};

class FunctionIntermediateRepresentationTest : public IR::FunctionIntermediateRepresentation {

};

class StatementIntermediateRepresentationTest : public IR::StatementIntermediateRepresentation {
public:
    StatementIntermediateRepresentationTest(IR::defs::ctx::IRFunctionContext& ctx)
        : IR::StatementIntermediateRepresentation{ ctx } {}
};

class ExpressionIntermediateRepresentationTest : public IR::ExpressionIntermediateRepresentation {
public:
    ExpressionIntermediateRepresentationTest(IR::defs::ctx::IRFunctionContext& ctx)
        : IR::ExpressionIntermediateRepresentation{ ctx } {}
};

#endif