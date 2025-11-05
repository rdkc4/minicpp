#ifndef FUNCTION_CODE_GENERATOR_HPP
#define FUNCTION_CODE_GENERATOR_HPP

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <atomic>

#include "../../common/intermediate-representation-tree/IRProgram.hpp"
#include "../../common/intermediate-representation-tree/IRFunction.hpp"
#include "../defs/code_generator_defs.hpp"
#include "statement_code_generator.hpp"


class FunctionCodeGenerator {
public:
    FunctionCodeGenerator(std::unordered_map<std::string, std::vector<std::string>>& asmCode);

    static CodeGeneratorThreadContext& getContext() noexcept;
    static void updatePrints(bool _prints) noexcept;
    bool hasPrint() const noexcept;

    // function
    void initFunctions(const IRProgram* _root);
    void generateFunction(const IRFunction* _function);

private:
    static thread_local CodeGeneratorThreadContext codeGenContext;
    // if printf is called generate a function for printing
    static std::atomic<bool> prints;
    StatementCodeGenerator stmtGenerator;

    std::mutex asmMtx;
    std::unordered_map<std::string, std::vector<std::string>>& asmCode;

    void generateParameter(const std::vector<std::unique_ptr<IRParameter>>& _parameters);

};

#endif