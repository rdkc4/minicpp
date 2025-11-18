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

/**
 * @class FunctionIntermediateRepresentation
 * @brief turns ast function into irt function
*/
class FunctionIntermediateRepresentation {
public:
    FunctionIntermediateRepresentation(std::unordered_map<std::string, std::vector<std::string>>& exceptions);

    /**
     * @brief getter for the context of the function processed by the current thread
     * @returns ir context of the current thread
    */
    static IRThreadContext& getContext() noexcept;

    /**
     * @brief turns ast function into irt function
     * @param _function - const pointer to the ast function
     * @returns pointer to the irt function
    */
    std::unique_ptr<IRFunction> function(const ASTFunction* _function);

private:
    static thread_local IRThreadContext irContext;
    
    StatementIntermediateRepresentation stmtIR;
    std::unordered_map<std::string,std::vector<std::string>>& exceptions;
    std::mutex exceptionMtx;

    /**
     * @brief turns ast parameters into irt parameters
     * @param _irFunction - pointer to the irt function
     * @param _parameters - reference to a const vector of pointers to ast parameters
     * @returns void
    */
    void parameter(IRFunction* _irFunction, const std::vector<std::unique_ptr<ASTParameter>>& _parameters);

    /**
     * @brief turns body of the ast function into the body of the irt function
     * @param _irFunction - pointer to the irt function
     * @param _body - reference to a const vector of pointers to the statements of the ast body
    */
    void body(IRFunction* _irFunction, const std::vector<std::unique_ptr<ASTStatement>>& _body);
};

#endif
