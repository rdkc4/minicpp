#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <string>
#include <unordered_map>

#include "../../common/intermediate-representation-tree/IRProgram.hpp"
#include "function_code_generator.hpp"

/** 
 * @class CodeGenerator
 * @brief generates the code for the x86-64 asm
 * @details
 *
 * $ as -o output.o output.s
 *
 * $ ld -o output output.o
 *
 * $ ./output
 *
 * $ echo $? - prints return value
*/
class CodeGenerator {
public:
    /** 
     * @brief Creates an instance of the code generator
     * @param filePath - path for the output asm file
    */
    CodeGenerator(const std::string& filePath);

    /** 
     * @brief generates the unique number for the label
     * @returns number of the label
    */
    static size_t getNextLabelNum() noexcept;

    /** 
     * @brief starts the code generation of the program
     * @param _root - const pointer to the irt program
     * @returns void 
    */
    void generateCode(const IRProgram* _root);

    /** 
     * @brief checks if the code generation was successful
     * @returns true if asm is generated, false otherwise
    */
    bool successful() const noexcept;

private:
    /// number for the next label
    static std::atomic<size_t> labelNum;
    /// output file path (.s)
    const std::string outputPath;
    /// code generator specialized for generating functions
    FunctionCodeGenerator funcGenerator;

    /// maps the name of the function to its assembly code
    std::unordered_map<std::string, std::vector<std::string>> asmCode;

    /** 
     * @brief writes generated code into asm file
     * @param _root - const pointer to the root of the program
     * @returns void
    */
    void writeCode(const IRProgram* _root);

};

#endif
