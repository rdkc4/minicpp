#ifndef INTERMEDIATE_REPRESENTATION_HPP
#define INTERMEDIATE_REPRESENTATION_HPP

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

#include "../common/intermediate-representation-tree/IRProgram.hpp"

#include "../common/abstract-syntax-tree/ASTProgram.hpp"
#include "function_intermediate_representation.hpp"

/*
    Turning abstract syntax tree into intermediate representation tree
*/
class IntermediateRepresentation{

    public:

        IntermediateRepresentation();

        // simplifying ast for easier code generation
        // returns root of the intermediate representation tree
        [[nodiscard]] std::unique_ptr<IRProgram> formIR(const ASTProgram* _program);

    protected:
        FunctionIntermediateRepresentation funcIR;
        std::unordered_map<std::string,std::vector<std::string>> exceptions;

        void checkErrors(const IRProgram* _program) const;
};

#endif
