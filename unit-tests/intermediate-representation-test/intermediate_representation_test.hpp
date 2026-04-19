#ifndef INTERMEDIATE_REPRESENTATION_TEST_HPP
#define INTERMEDIATE_REPRESENTATION_TEST_HPP

#include <cassert>

#include "../../intermediate-representation/intermediate_representation.hpp"
#include "../../intermediate-representation/function_intermediate_representation.hpp"

class IntermediateRepresentationTest : public ir::IntermediateRepresentation {
    public:
        IntermediateRepresentationTest(util::concurrency::ThreadPool& threadPool) 
            : ir::IntermediateRepresentation{ threadPool } {}

        const std::vector<std::string>& getErrors(const std::string& func) const noexcept {
            assert(exceptions.find(func) != exceptions.end());
            return exceptions.at(func);
        }
};

class FunctionIntermediateRepresentationTest : public ir::FunctionIntermediateRepresentation {

};

class StatementIntermediateRepresentationTest : public ir::StatementIntermediateRepresentation {
public:
    StatementIntermediateRepresentationTest(ir::IRFunctionContext& ctx)
        : ir::StatementIntermediateRepresentation{ ctx } {}
};

class ExpressionIntermediateRepresentationTest : public ir::ExpressionIntermediateRepresentation {
public:
    ExpressionIntermediateRepresentationTest(ir::IRFunctionContext& ctx)
        : ir::ExpressionIntermediateRepresentation{ ctx } {}
};

#endif