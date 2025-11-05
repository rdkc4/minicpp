#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <string>
#include <unordered_map>

#include "../../common/intermediate-representation-tree/IRProgram.hpp"
#include "function_code_generator.hpp"

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

        // ensuring label number is unique for every non-related
        static size_t getNextLabelNum() noexcept;

        // start code generation
        void generateCode(const IRProgram* _root);

        bool successful() const noexcept;

    private:
        // number for next label
        // since functions are generated parallely, labelNums won't be in any particular order
        static std::atomic<size_t> labelNum;
        // output file path (.s)
        const std::string outputPath;
        FunctionCodeGenerator funcGenerator;

        std::unordered_map<std::string, std::vector<std::string>> asmCode;

        // writing generated code to file
        void writeCode(const IRProgram* _root);

};

#endif
