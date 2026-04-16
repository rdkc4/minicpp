#ifndef INTERMEDIATE_REPRESENTATION_FIXTURE_HPP
#define INTERMEDIATE_REPRESENTATION_FIXTURE_HPP

#include "../analyzer-test/analyzer_fixture.hpp"
#include "intermediate_representation_test.hpp"
#include <memory>

class IntermediateRepresentationFixture : public AnalyzerFixture {
protected:
    std::unique_ptr<IR::node::IRProgram> irProgram;
    std::unique_ptr<IntermediateRepresentationTest> intermediateRepresentation;

    void initIR() {
        initAnalyzer();
        intermediateRepresentation = std::make_unique<IntermediateRepresentationTest>(tp);
        irProgram = intermediateRepresentation->transformProgram(program.get());
    }
};

class StatementIntermediateRepresentationFixture : public StatementAnalyzerFixture {
protected:
    std::unique_ptr<IR::node::IRStmt> irStmt;
    std::unique_ptr<StatementIntermediateRepresentationTest> intermediateRepresentation;

    void initIR(){
        initAnalyzer();
        intermediateRepresentation = std::make_unique<StatementIntermediateRepresentationTest>();
        irStmt = intermediateRepresentation->transformStmt(stmt.get());
    }
};

class ExpressionIntermediateRepresentationFixture : public ExpressionAnalyzerFixture {
protected:
    std::unique_ptr<IR::node::IRExpr> irExpr;
    std::unique_ptr<ExpressionIntermediateRepresentationTest> intermediateRepresentation;

    void initIR(){
        initAnalyzer();
        intermediateRepresentation = std::make_unique<ExpressionIntermediateRepresentationTest>();
        irExpr = intermediateRepresentation->transformExpr(expr.get());
    }
};

#endif