#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <fstream>
#include <vector>
#include <memory>
#include <unordered_map>

#include "intermediate_representation.hpp"

class CodeGenerator{

    public:

        CodeGenerator();
        ~CodeGenerator();

        void generateCode(std::shared_ptr<IRTree> root);

    private:

        std::ofstream generatedCode;

        std::string activeFunction;

        std::unordered_map<std::string, std::string> variableMapping;

        size_t labelNum = 0;

        const std::vector<std::string> gpRegisters = {"%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"};
        size_t gpFreeRegPos = 0;

        void takeGpReg();

        void freeGpReg();

        size_t getNextLabelNum();

        void generateFunction(std::shared_ptr<IRTree> node);

        void generateParameter(std::shared_ptr<IRTree> node);

        void generateVariable(std::shared_ptr<IRTree> node);

        void generateStatement(std::shared_ptr<IRTree> node);

        void generateIfStatement(std::shared_ptr<IRTree> node);

        void generateWhileStatement(std::shared_ptr<IRTree> node);

        void generateForStatement(std::shared_ptr<IRTree> node);

        void generateDoWhileStatement(std::shared_ptr<IRTree> node);

        void generateCompoundStatement(std::shared_ptr<IRTree> node);

        void generateAssignmentStatement(std::shared_ptr<IRTree> node);

        void generateReturnStatement(std::shared_ptr<IRTree> node);

        void generateSwitchStatement(std::shared_ptr<IRTree> node);

        void generateNumericalExpression(std::shared_ptr<IRTree> node);

        void generateRelationalExpression(std::shared_ptr<IRTree> node);

        void generateID(std::shared_ptr<IRTree> node);

        void generateLiteral(std::shared_ptr<IRTree> node);

        void generateFunctionCall(std::shared_ptr<IRTree> node);

        void generateArgument(std::shared_ptr<IRTree> node);

        void clearArguments(std::shared_ptr<IRTree> node);

};

#endif