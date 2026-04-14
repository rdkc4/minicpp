#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <string>
#include <unordered_map>

#include "../../common/intermediate-representation-tree/ir_program.hpp"
#include "../../thread-pool/thread_pool.hpp"
#include "function_code_generator.hpp"

/** 
 * @class CodeGenerator
 * @brief generates the code for the x86-64 asm
*/
class CodeGenerator {
public:
    /** 
     * @brief Creates an instance of the code generator
     * @param filePath - path for the output asm file
     * @param threadPool - reference to a thread pool
    */
    CodeGenerator(const std::string& filePath, ThreadPool& threadPool);

    /** 
     * @brief generates the unique number for the label
     * @returns number of the label
    */
    static size_t getNextLabelNum() noexcept;

    /** 
     * @brief starts the code generation of the program
     * @param program - const pointer to the irt program
    */
    void generateProgram(const IR::node::IRProgram* program);

    /** 
     * @brief checks if the code generation was successful
     * @returns true if asm is generated, false otherwise
    */
    bool successful() const noexcept;

private:
    /// thread pool for parallel function code generation
    ThreadPool& threadPool;

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
     * @param program - const pointer to the root of the program
    */
    void writeCode(const IR::node::IRProgram* program);

};

#endif
