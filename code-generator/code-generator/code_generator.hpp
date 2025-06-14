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

        CodeGenerator(const std::string& file);

        // start code generation
        void generateCode(const IRTree* root);

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

        // function
        void initFunctions(const IRTree* node);
        void generateFunction(const IRTree* node);
        void generateParameter(const IRTree* node);

        // construct
        void generateConstruct(const IRTree* node);

        // variable
        void generateVariable(const IRTree* node);

        // statement
        void generateStatement(const IRTree* node);
        void generatePrintfStatement(const IRTree* node);
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
