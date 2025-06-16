#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>

#include "../../common/intermediate-representation-tree/IRTree.hpp"
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
        void generateCode(const IRTree* _root);

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
        void writeCode(const IRTree* _root);

        // function
        void initFunctions(const IRTree* _root);
        void generateFunction(const IRTree* _function);
        void generateParameter(const IRTree* _parameters);

        // construct
        void generateConstruct(const IRTree* _construct);

        // variable
        void generateVariable(const IRTree* _variable);

        // statement
        void generateStatement(const IRTree* _statement);
        void generatePrintfStatement(const IRTree* _printf);
        void generateIfStatement(const IRTree* _if);
        void generateWhileStatement(const IRTree* _while);
        void generateForStatement(const IRTree* _for);
        void generateDoWhileStatement(const IRTree* _dowhile);
        void generateCompoundStatement(const IRTree* _compound);
        void generateAssignmentStatement(const IRTree* _assignment);
        void generateReturnStatement(const IRTree* _return);
        void generateSwitchStatement(const IRTree* _switch);

        // expression
        void generateNumericalExpression(const IRTree* _numexp);
        void generateRelationalExpression(const IRTree* _relexp);
        const std::string& generateID(const IRTree* _id) const;
        std::string generateLiteral(const IRTree* _literal) const;
        void generateFunctionCall(const IRTree* _functionCall);
        void generateArgument(const IRTree* _arguments);
        void clearArguments(const IRTree* _arguments);

};

#endif
