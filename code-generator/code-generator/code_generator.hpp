#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>

#include "../../common/intermediate-representation-tree/IRProgram.hpp"
#include "../../common/intermediate-representation-tree/IRFunction.hpp"
#include "../../common/intermediate-representation-tree/IRParameter.hpp"
#include "../../common/intermediate-representation-tree/IRStatement.hpp"
#include "../../common/intermediate-representation-tree/IRVariable.hpp"
#include "../../common/intermediate-representation-tree/IRPrintfSt.hpp"
#include "../../common/intermediate-representation-tree/IRSwitchSt.hpp"
#include "../../common/intermediate-representation-tree/IRForSt.hpp"
#include "../../common/intermediate-representation-tree/IRWhileSt.hpp"
#include "../../common/intermediate-representation-tree/IRDoWhileSt.hpp"
#include "../../common/intermediate-representation-tree/IRCompoundSt.hpp"
#include "../../common/intermediate-representation-tree/IRIfSt.hpp"
#include "../../common/intermediate-representation-tree/IRAssignSt.hpp"
#include "../../common/intermediate-representation-tree/IRReturnSt.hpp"
#include "../../common/intermediate-representation-tree/IRExpression.hpp"
#include "../../common/intermediate-representation-tree/IRFunctionCall.hpp"
#include "../../common/intermediate-representation-tree/IRId.hpp"
#include "../../common/intermediate-representation-tree/IRLiteral.hpp"
#include "../../common/intermediate-representation-tree/IRTemporary.hpp"
#include "../asm-generator/asm_generator.hpp"
#include "../defs/code_generator_defs.hpp"

/*
    Generating asm code from intermediate representation tree
    ASM CODE GENERATION - x86_64 (linux)
    // running asm code
    $ as -o output.o output.s
    $ ld -o output output.o
    $ ./output
    $ echo $? (to check return value)
*/
class CodeGenerator{

    public:

        CodeGenerator(const std::string& filePath);

        // start code generation
        void generateCode(const IRProgram* _root);

    private:
        // component for asm code generation
        AsmGenerator _asm;
        // number for next label
        // since functions are generated parallely, labelNums won't be in any particular order
        std::atomic<size_t> labelNum;
        // if printf is called generate a function for printing
        std::atomic<bool> prints;
        // output file path (.s)
        const std::string outputPath;
        
        mutable std::mutex asmMtx;
        mutable std::unordered_map<std::string, std::vector<std::string>> asmCode;

        static thread_local CodeGeneratorThreadContext codeGenContext;

        static constexpr size_t regSize{8};

        // register usage handlers
        void takeGpReg(size_t& gpFreeRegPos) noexcept;
        void freeGpReg(size_t& gpFreeRegPos) noexcept;

        // ensuring label number is unique for every non-related
        [[nodiscard]] size_t getNextLabelNum() noexcept;

        // writing generated code to file
        void writeCode(const IRProgram* _root);

        // function
        void initFunctions(const IRProgram* _root);
        void generateFunction(const IRFunction* _function);
        void generateParameter(const std::vector<std::unique_ptr<IRParameter>>& _parameters);

        // variable
        void generateVariable(const IRVariable* _variable);

        // statement
        void generateStatement(const IRStatement* _statement);
        void generatePrintfStatement(const IRPrintfSt* _printf);
        void generateIfStatement(const IRIfSt* _if);
        void generateWhileStatement(const IRWhileSt* _while);
        void generateForStatement(const IRForSt* _for);
        void generateDoWhileStatement(const IRDoWhileSt* _dowhile);
        void generateCompoundStatement(const IRCompoundSt* _compound);
        void generateAssignmentStatement(const IRAssignSt* _assignment);
        void generateReturnStatement(const IRReturnSt* _return);
        void generateSwitchStatement(const IRSwitchSt* _switch);

        // expression
        void generateNumericalExpression(const IRExpression* _numexp);
        void generateRelationalExpression(const IRExpression* _relexp);
        const std::string& generateID(const IRId* _id) const;
        std::string generateLiteral(const IRLiteral* _literal) const;
        void generateFunctionCall(const IRFunctionCall* _functionCall);
        void generateArgument(const IRFunctionCall* _arguments);
        void clearArguments(size_t _arguments);

        // temporaries
        void generateTemporaries(const IRTemporary* _temporary);

};

#endif
