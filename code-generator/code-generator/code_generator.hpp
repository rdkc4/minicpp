#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <string>
#include <unordered_map>

#include "../../common/intermediate-representation-tree/IRTree.hpp"
#include "../asm-generator/asm_generator.hpp"

class CodeGenerator{

    public:

        CodeGenerator(std::string& output);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // public entry point to code generation
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void generateCode(IRTree* root);

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
        void takeGpReg();

        void freeGpReg();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // ensuring label number is unique for every non-related
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        size_t getNextLabelNum();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // function
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void generateFunction(IRTree* node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // variables
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void generateParameter(IRTree* node);

        void generateConstruct(IRTree* node);

        void generateVariable(IRTree* node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // statements
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void generateStatement(IRTree* node);

        void generateIfStatement(IRTree* node);

        void generateWhileStatement(IRTree* node);

        void generateForStatement(IRTree* node);

        void generateDoWhileStatement(IRTree* node);

        void generateCompoundStatement(IRTree* node);

        void generateAssignmentStatement(IRTree* node);

        void generateReturnStatement(IRTree* node);

        void generateSwitchStatement(IRTree* node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // expressions
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void generateNumericalExpression(IRTree* node);

        void generateRelationalExpression(IRTree* node);

        const std::string generateID(IRTree* node);

        const std::string generateLiteral(IRTree* node);

        void generateFunctionCall(IRTree* node);

        void generateArgument(IRTree* node);

        void clearArguments(IRTree* node);

};

#endif
