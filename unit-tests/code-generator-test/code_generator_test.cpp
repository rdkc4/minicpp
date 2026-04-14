#include <gtest/gtest.h>
#include <format>

#include "../../compiler/compiler.hpp"
#include "../test-utils/test_utils.hpp"

#if defined(__x86_64__) && defined(__clang__)

TEST(CodeGenTest, ReturnsExpr){
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

TEST(CodeGenTest, ReturnsLogicalExpr){
    std::vector<std::string> ext = {".mcpp", ".s", ".o", ""};

    const std::string input{"tmp9.mcpp"};
    const std::string output{ "tmp9" };

    __test__writeSourceToFile("int main(){ return 1 < 2 && 3 > 4 || 2 <= 3; }", input);
    
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

TEST(CodeGenTest, IfStatementLiteralCondition){
    std::vector<std::string> ext = {".mcpp", ".s", ".o", ""};

    const std::string input{"tmp10.mcpp"};
    const std::string output{ "tmp10" };

    __test__writeSourceToFile("int main(){ if(1) return 0; return 1; }", input);
    
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

TEST(CodeGenTest, DoWhileStatementIdCondition){
    std::vector<std::string> ext = {".mcpp", ".s", ".o", ""};

    const std::string input{"tmp11.mcpp"};
    const std::string output{ "tmp11" };

    __test__writeSourceToFile("int main(){ int x = 3; do{ x = x - 1; }while(x); return x; }", input);
    
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

TEST(CodeGenTest, WhileStatementFunctionCallCondition){
    std::vector<std::string> ext = {".mcpp", ".s", ".o", ""};

    const std::string input{"tmp12.mcpp"};
    const std::string output{ "tmp12" };

    __test__writeSourceToFile("int fn(int x){ return x; } int main(){ int x = 3; while(fn(x)) x = x - 1; return x; }", input);
    
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

TEST(CodeGenTest, ForStatementComplexConditionExpression){
    std::vector<std::string> ext = {".mcpp", ".s", ".o", ""};

    const std::string input{"tmp13.mcpp"};
    const std::string output{ "tmp13" };

    __test__writeSourceToFile("int main(){ int a = 5; int b = 10; int c = 3; int d = 8; int sum = 0; int i; for(i = 0;((i < 20) && ((a + b * c - d / 2) > 0) && ((i << 1) < (b >> 1)) && ((a ^ b) != (c | d)) && ((a & d) <= (b + c))) || ((i & 3) == 0); i = i + 1){ sum = sum + (i * 2 + (a - c)); } return sum; }", input);
    
    Compiler::compile({.input = input, .output = output});

    int run_status = __test__runCommand({
        std::format("./{}", output)
    });
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 12);

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