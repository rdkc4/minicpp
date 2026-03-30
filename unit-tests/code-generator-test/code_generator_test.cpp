#include <gtest/gtest.h>
#include <format>

#include "../../compiler/compiler.hpp"
#include "../test-utils/test_utils.hpp"

#if defined(__x86_64__) && defined(__clang__)

TEST(CodeGenTest, ReturnsNumExp){
    std::vector<std::string> ext = {".mcpp", ".s", ".o", ""};

    const std::string input{"tmp1.mcpp"};
    const std::string output{ "tmp1" };

    __test__writeSourceToFile("int main(){ return 1 + 2 * 3 - 4 / 2; }", input);
    
    Compiler::compile({.input = input, .output = output});

    int run_status = __test__runCommand({
        std::format("./{}", output)
    });
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 5);

    // cleanup files
    for(const auto& e : ext){
        __test__removeFile(std::format("{}{}", output, e));
    }
}

TEST(CodeGenTest, ReturnsFuncCall){
    std::vector<std::string> ext = {".mcpp", ".s", ".o", ""};
    
    const std::string input{"tmp2.mcpp"};
    const std::string output{ "tmp2" };

    __test__writeSourceToFile("int sq(int x){ return x * x; } int main(){ return sq(5); }", input);
    
    Compiler::compile({.input = input, .output = output});

    int run_status = __test__runCommand({
        std::format("./{}", output)
    });
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 25);

    // cleanup files
    for(const auto& e : ext){
        __test__removeFile(std::format("{}{}", output, e));
    }
}

TEST(CodeGenTest, ReturnsRecursiveFuncCall){
    std::vector<std::string> ext = {".mcpp", ".s", ".o", ""};

    const std::string input{"tmp3.mcpp"};
    const std::string output{ "tmp3" };

    __test__writeSourceToFile("int fib(int n){ if(n == 0) return 0; else if(n == 1) return 1; else return fib(n-1) + fib(n-2); } int main(){ return fib(6); }", input);

    Compiler::compile({.input = input, .output = output});

    int run_status = __test__runCommand({
        std::format("./{}", output)
    });
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 8);

    // cleanup files
    for(const auto& e : ext){
        __test__removeFile(std::format("{}{}", output, e));
    }
}

TEST(CodeGenTest, SwitchStatementCase){
    std::vector<std::string> ext = {".mcpp", ".s", ".o", ""};
    
    const std::string input{"tmp4.mcpp"};
    const std::string output{ "tmp4" };

    __test__writeSourceToFile("int main(){ int x = 5; switch(x){ case 1: return 3; case 3: return 2; case 5: return 1; default: return 0; } }", input);

    Compiler::compile({.input = input, .output = output});

    int run_status = __test__runCommand({
        std::format("./{}", output)
    });
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 1);

    // cleanup files
    for(const auto& e : ext){
        __test__removeFile(std::format("{}{}", output, e));
    }
}

TEST(CodeGenTest, SwitchStatementDefault){
    std::vector<std::string> ext = {".mcpp", ".s", ".o", ""};
    
    const std::string input{"tmp5.mcpp"};
    const std::string output{ "tmp5" };

    __test__writeSourceToFile("int main(){ int x = 5; switch(x){ case 1: return 3; case 3: return 2; default: return 0; } }", input);

    Compiler::compile({.input = input, .output = output});

    int run_status = __test__runCommand({
        std::format("./{}", output)
    });
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 0);

    // cleanup files
    for(const auto& e : ext){
        __test__removeFile(std::format("{}{}", output, e));
    }
}

TEST(CodeGenTest, WhileStatement){
    std::vector<std::string> ext = {".mcpp", ".s", ".o", ""};
    
    const std::string input{"tmp6.mcpp"};
    const std::string output{ "tmp6" };

    __test__writeSourceToFile("int main(){ int x = 5; while(x > 0) x = x - 1; return x; }", input);

    Compiler::compile({.input = input, .output = output});

    int run_status = __test__runCommand({
        std::format("./{}", output)
    });
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 0);

    // cleanup files
    for(const auto& e : ext){
        __test__removeFile(std::format("{}{}", output, e));
    }
}

TEST(CodeGenTest, DoWhileStatement){
    std::vector<std::string> ext = {".mcpp", ".s", ".o", ""};
    
    const std::string input{"tmp7.mcpp"};
    const std::string output{ "tmp7" };

    __test__writeSourceToFile("int main(){ int x = 5; do{ x = x + 3; }while(x < 10); return x; }", input);

    Compiler::compile({.input = input, .output = output});

    int run_status = __test__runCommand({
        std::format("./{}", output)
    });
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 11);

    // cleanup files
    for(const auto& e : ext){
        __test__removeFile(std::format("{}{}", output, e));
    }
}

TEST(CodeGenTest, ForStatement){
    std::vector<std::string> ext = {".mcpp", ".s", ".o", ""};
    
    const std::string input{"tmp8.mcpp"};
    const std::string output{ "tmp8" };

    __test__writeSourceToFile("int main(){ int x = 5; int i; for(i = 0; i < 10; i = i + 1) x = x + 1; return x; }", input);

    Compiler::compile({.input = input, .output = output});

    int run_status = __test__runCommand({
        std::format("./{}", output)
    });
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 15);

    // cleanup files
    for(const auto& e : ext){
        __test__removeFile(std::format("{}{}", output, e));
    }
}

#else

TEST(CodeGenTest, SkippedDueToArchitecture) {
    GTEST_SKIP() << "Skipping: Not running on an x86_64 system.";
}

#endif