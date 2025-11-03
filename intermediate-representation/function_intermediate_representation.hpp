#ifndef FUNCTION_INTERMEDIATE_REPRESENTATION_HPP
#define FUNCTION_INTERMEDIATE_REPRESENTATION_HPP

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

#include "../common/abstract-syntax-tree/ASTFunction.hpp"
#include "../common/intermediate-representation-tree/IRFunction.hpp"
#include "defs/ir_defs.hpp"
#include "statement_intermediate_representation.hpp"

class FunctionIntermediateRepresentation {
public:
    FunctionIntermediateRepresentation(std::unordered_map<std::string, std::vector<std::string>>& exceptions);
    static IRThreadContext& getContext() noexcept;

    std::unique_ptr<IRFunction> function(const ASTFunction* _function);

private:
    static thread_local IRThreadContext irContext;
    
    StatementIntermediateRepresentation stmtIR;
    std::unordered_map<std::string,std::vector<std::string>>& exceptions;
    std::mutex exceptionMtx;

    void parameter(IRFunction* _irFunction, const std::vector<std::unique_ptr<ASTParameter>>& _parameters);
    void body(IRFunction* _irFunction, const std::vector<std::unique_ptr<ASTStatement>>& _body);
};

#endif