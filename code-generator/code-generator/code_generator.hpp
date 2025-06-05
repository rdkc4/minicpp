#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <string>
#include <unordered_map>

#include "../../common/intermediate-representation-tree/IRTree.hpp"
#include "../asm-generator/asm_generator.hpp"

/*
    Generating asm code from intermediate representation tree
    ASM CODE GENERATION - x86_64 (linux)
    $ as -o file_name.o file_name.s
    $ ld -o file_name file_name.o
    $ ./file_name
    $ echo $? (to check return value)
*/
class CodeGenerator{

    public:

        CodeGenerator(std::string& file);

        // start code generation
        void generateCode(const IRTree* root);

    private:
        // component for asm code generation
        AsmGenerator _asm;
        // function being generated at the moment
        std::string activeFunction;
        // mapping variable name to its address (relative to rbp)
        std::unordered_map<std::string, std::string> variableMap;
        // number for next label
        size_t labelNum;
        // position of the first free gener purpose register (if there is no free gp registers, stack is used) 
        size_t gpFreeRegPos;
        // number for next variable (-variableNum*8(%rbp))
        size_t variableNum;

        static constexpr size_t regSize{8};

        // register usage handlers
        void takeGpReg() noexcept;
        void freeGpReg() noexcept;

        // ensuring label number is unique for every non-related
        [[nodiscard]] size_t getNextLabelNum() noexcept;

        // function
        void generateFunction(const IRTree* node);
        void generateParameter(const IRTree* node);

        // construct
        void generateConstruct(const IRTree* node);

        // variable
        void generateVariable(const IRTree* node);

        // statement
        void generateStatement(const IRTree* node);
        void generateIfStatement(const IRTree* node);
        void generateWhileStatement(const IRTree* node);
        void generateForStatement(const IRTree* node);
        void generateDoWhileStatement(const IRTree* node);
        void generateCompoundStatement(const IRTree* node);
        void generateAssignmentStatement(const IRTree* node);
        void generateReturnStatement(const IRTree* node);
        void generateSwitchStatement(const IRTree* node);

        // expression
        void generateNumericalExpression(const IRTree* node);
        void generateRelationalExpression(const IRTree* node);
        const std::string& generateID(const IRTree* node) const;
        std::string generateLiteral(const IRTree* node) const;
        void generateFunctionCall(const IRTree* node);
        void generateArgument(const IRTree* node);
        void clearArguments(const IRTree* node);

};

#endif
