#ifndef COMPILER_FIXTURE_HPP
#define COMPILER_FIXTURE_HPP

#include <gtest/gtest.h>
#include <string>

#include "../../compiler/compiler.hpp"
#include "../test-utils/test_utils.hpp"

class CompilerFixture : public ::testing::Test {
protected:
    const std::string input{ "tmp.mcpp" };
    const std::string output{ "tmp" };
    Compiler::ExitCode returnCode;

    void initCompiler(std::string_view source){
        __test__writeSourceToFile(source, input);
        returnCode =  Compiler::compile({
            .stopAfterAssembly = true, 
            .input = input, 
            .output = output
        });
    }

    void TearDown() override {
        __test__removeFile(input);
        __test__removeFile(output + ".s");
    }

};

#endif