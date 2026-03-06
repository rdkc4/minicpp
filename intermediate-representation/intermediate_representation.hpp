#ifndef INTERMEDIATE_REPRESENTATION_HPP
#define INTERMEDIATE_REPRESENTATION_HPP

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

#include "../common/intermediate-representation-tree/ir_program.hpp"

#include "../common/abstract-syntax-tree/ast_program.hpp"
#include "directive_intermediate_representation.hpp"
#include "function_intermediate_representation.hpp"

/**
 * @class IntermediateRepresentation
 * @brief turns abstract syntax tree into intermediate representation tree
*/
class IntermediateRepresentation {
public:
    /** 
     * @brief Creates the instance of the intermediate representation
    */
    IntermediateRepresentation();

    /**
     * @brief transforms ast program into irt program
     * @param _program - const pointer to the root of the ast program
     * @returns pointer to the root of irt program
    */
    [[nodiscard]] std::unique_ptr<IRProgram> formIR(const ASTProgram* _program);

    /**
     * @brief checks if any errors are caught
     * @param _program - const pointer to the root of the irt
     * @returns true if any error is caught, false otherwise
    */
    bool hasErrors(const IRProgram* _program) const noexcept;

    /**
     * @brief getter for the ir errors
     * @param _program - pointer to the root of the irt program
     * @returns errors merged into a string
    */
    std::string getErrors(const IRProgram* _program) const noexcept;

private:
    /// intermediate representation specialized for functions
    FunctionIntermediateRepresentation funcIR;

    /// intermediate representation specialized for directives
    DirectiveIntermediateRepresentation dirIR;

protected:
    /// maps function name to its exceptions
    std::unordered_map<std::string,std::vector<std::string>> exceptions;
};

#endif
