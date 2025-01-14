#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <fstream>
#include <memory>
#include <unordered_map>

#include "../common/intermediate-representation-tree/IRTree.hpp"

class CodeGenerator{

    public:

        CodeGenerator(std::string& output);
        ~CodeGenerator();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // public entry point to code generation
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void generateCode(std::shared_ptr<IRTree> root);

    private:

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // generatedCode - output file that contains asm (.s) code
        // active function - function being generated at the moment
        // variable mapping - mapping variable name to its address relative to rbp
        // label number - number for next label
        // gp free register position - position of the first free register
        // variable number - number for next variable -> -variableNum*8(%rbp)
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::ofstream generatedCode;
        std::string activeFunction;
        std::unordered_map<std::string, std::string> variableMap;
        size_t labelNum;
        size_t gpFreeRegPos;
        size_t variableNum;

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
        void generateFunction(std::shared_ptr<IRTree> node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // variables
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void generateParameter(std::shared_ptr<IRTree> node);

        void generateConstruct(std::shared_ptr<IRTree> node);

        void generateVariable(std::shared_ptr<IRTree> node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // statements
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void generateStatement(std::shared_ptr<IRTree> node);

        void generateIfStatement(std::shared_ptr<IRTree> node);

        void generateWhileStatement(std::shared_ptr<IRTree> node);

        void generateForStatement(std::shared_ptr<IRTree> node);

        void generateDoWhileStatement(std::shared_ptr<IRTree> node);

        void generateCompoundStatement(std::shared_ptr<IRTree> node);

        void generateAssignmentStatement(std::shared_ptr<IRTree> node);

        void generateReturnStatement(std::shared_ptr<IRTree> node);

        void generateSwitchStatement(std::shared_ptr<IRTree> node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // expressions
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void generateNumericalExpression(std::shared_ptr<IRTree> node);

        void generateRelationalExpression(std::shared_ptr<IRTree> node);

        std::string generateID(std::shared_ptr<IRTree> node);

        std::string generateLiteral(std::shared_ptr<IRTree> node);

        void generateFunctionCall(std::shared_ptr<IRTree> node);

        void generateArgument(std::shared_ptr<IRTree> node);

        void clearArguments(std::shared_ptr<IRTree> node);

};

#endif
