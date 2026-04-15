#ifndef CODE_GENERATOR_FIXTURE_HPP
#define CODE_GENERATOR_FIXTURE_HPP

#include <gtest/gtest.h>
#include <string_view>
#include <vector>

#include "../test-utils/test_utils.hpp"
#include "../../compiler/compiler.hpp"

class CodeGeneratorFixture : public ::testing::Test {
protected:
    std::vector<std::string> exts { ".mcpp", ".s", ".o", "" }; 

    const std::string input{ "tmp.mcpp" };
    const std::string output{ "tmp" };
    int run_status;

    void initCodeGen(std::string_view source){
        __test__writeSourceToFile(source, input);
    
        Compiler::compile({.input = input, .output = output});
        run_status = __test__runCommand({
            std::format("./{}", output)
        });
    }

    void TearDown() override {
        for(const auto& ext : exts){
            __test__removeFile(std::format("{}{}", output, ext));
        }
    }

};

#endif