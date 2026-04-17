#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <mutex>
#include <string_view>
#include <string>
#include <unordered_map>

#include "../../common/intermediate-representation-tree/ir_program.hpp"
#include "../../thread-pool/thread_pool.hpp"

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
    CodeGenerator(std::string_view filePath, ThreadPool& threadPool);

    /** 
     * @brief starts the code generation of the program
     * @param program - const pointer to the irt program
    */
    void generateProgram(const ir::IRProgram* program);

    /** 
     * @brief checks if the code generation was successful
     * @returns true if asm is generated, false otherwise
    */
    bool successful() const noexcept;

private:
    /// maps the name of the function to its assembly code
    std::unordered_map<std::string, std::vector<std::string>> asmCode;

    /// mutex protecting the asmCode
    std::mutex mtx;

    /// thread pool for parallel function code generation
    ThreadPool& threadPool;

    /// output file path (.s)
    const std::string outputPath;

    /** 
     * @brief writes generated code into asm file
     * @param program - const pointer to the root of the program
    */
    void writeCode(const ir::IRProgram* program);

};

#endif
