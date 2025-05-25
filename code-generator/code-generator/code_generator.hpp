#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <string>
#include <unordered_map>

#include "../../common/intermediate-representation-tree/IRTree.hpp"
#include "../asm-generator/asm_generator.hpp"

class CodeGenerator{

    public:

        CodeGenerator(std::string& file);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // public entry point to code generation
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void generateCode(const IRTree* root);

    private:

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // _asm - component for asm code generation
        // active function - function being generated at the moment
        // variable mapping - mapping variable name to its address relative to rbp
        // label number - number for next label
        // gp free register position - position of the first free register
        // variable number - number for next variable -> -variableNum*8(%rbp)
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        AsmGenerator _asm;
        std::string activeFunction;
        std::unordered_map<std::string, std::string> variableMap;
        size_t labelNum;
        size_t gpFreeRegPos;
        size_t variableNum;
        static constexpr size_t regSize{8};

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // register usage handlers
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void takeGpReg() noexcept;

        void freeGpReg() noexcept;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // ensuring label number is unique for every non-related
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        [[nodiscard]] size_t getNextLabelNum() noexcept;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // function
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void generateFunction(const IRTree* node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // variables
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void generateParameter(const IRTree* node);

        void generateConstruct(const IRTree* node);

        void generateVariable(const IRTree* node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // statements
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void generateStatement(const IRTree* node);

        void generateIfStatement(const IRTree* node);

        void generateWhileStatement(const IRTree* node);

        void generateForStatement(const IRTree* node);

        void generateDoWhileStatement(const IRTree* node);

        void generateCompoundStatement(const IRTree* node);

        void generateAssignmentStatement(const IRTree* node);

        void generateReturnStatement(const IRTree* node);

        void generateSwitchStatement(const IRTree* node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // expressions
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void generateNumericalExpression(const IRTree* node);

        void generateRelationalExpression(const IRTree* node);

        const std::string& generateID(const IRTree* node) const;

        std::string generateLiteral(const IRTree* node) const;

        void generateFunctionCall(const IRTree* node);

        void generateArgument(const IRTree* node);

        void clearArguments(const IRTree* node);

};

#endif
