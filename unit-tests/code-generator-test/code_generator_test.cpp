#include <gtest/gtest.h>
#include <cstdlib>
#include <filesystem>
#include <format>

#include "../../compiler/compiler.hpp"

#if defined(__x86_64__) // tests use GNU assembler (as) and GNU linker (ld)

TEST(CodeGenTest, ReturnsNumExp){
    std::vector<std::string> ext = {".s", ".o", ""};

    const std::string input{"int main(){ return 1 + 2 * 3 - 4 / 2; }"};
    const std::string outputNoExt{ "tmp1" };
    const std::string output{ outputNoExt + ext[0] };
    
    Compiler::compile(input, output);

    std::string asmCmd = std::format("as {} -o {}", output, outputNoExt + ext[1]);
    int assemble = system(asmCmd.c_str());
    ASSERT_EQ(assemble, 0) << std::format("{} failed.\n", asmCmd);

    std::string linkCmd = std::format("ld {} -o {}", outputNoExt + ext[1], outputNoExt);
    int link = system(linkCmd.c_str());
    ASSERT_EQ(link, 0) << std::format("{} failed.\n",linkCmd);

    std::string runCmd = std::format("./{}", outputNoExt);
    int run_status = system(runCmd.c_str());
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 5);

    // cleanup files
    for(const auto& e : ext){
        std::filesystem::remove(std::format("{}{}", outputNoExt, e));
    }
}

TEST(CodeGenTest, ReturnsFuncCall){
    std::vector<std::string> ext = {".s", ".o", ""};

    const std::string input{"int sq(int x){ return x * x; } int main(){ return sq(5); }"};
    const std::string outputNoExt{ "tmp2" };
    const std::string output{ outputNoExt + ext[0] };
    
    Compiler::compile(input, output);

    std::string asmCmd = std::format("as {} -o {}", output, outputNoExt + ext[1]);
    int assemble = system(asmCmd.c_str());
    ASSERT_EQ(assemble, 0) << std::format("{} failed.\n", asmCmd);

    std::string linkCmd = std::format("ld {} -o {}", outputNoExt + ext[1], outputNoExt);
    int link = system(linkCmd.c_str());
    ASSERT_EQ(link, 0) << std::format("{} failed.\n",linkCmd);

    std::string runCmd = std::format("./{}", outputNoExt);
    int run_status = system(runCmd.c_str());
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 25);

    // cleanup files
    for(const auto& e : ext){
        std::filesystem::remove(std::format("{}{}", outputNoExt, e));
    }
}

TEST(CodeGenTest, ReturnsRecursiveFuncCall){
    std::vector<std::string> ext = {".s", ".o", ""};

    const std::string input{"int fib(int n){ if(n == 0) return 0; else if(n == 1) return 1; else return fib(n-1) + fib(n-2); } int main(){ return fib(6); }"};
    const std::string outputNoExt{ "tmp3" };
    const std::string output{ outputNoExt + ext[0] };
    
    Compiler::compile(input, output);

    std::string asmCmd = std::format("as {} -o {}", output, outputNoExt + ext[1]);
    int assemble = system(asmCmd.c_str());
    ASSERT_EQ(assemble, 0) << std::format("{} failed.\n", asmCmd);

    std::string linkCmd = std::format("ld {} -o {}", outputNoExt + ext[1], outputNoExt);
    int link = system(linkCmd.c_str());
    ASSERT_EQ(link, 0) << std::format("{} failed.\n",linkCmd);

    std::string runCmd = std::format("./{}", outputNoExt);
    int run_status = system(runCmd.c_str());
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 8);

    // cleanup files
    for(const auto& e : ext){
        std::filesystem::remove(std::format("{}{}", outputNoExt, e));
    }
}

TEST(CodeGenTest, SwitchStatementCase){
    std::vector<std::string> ext = {".s", ".o", ""};

    const std::string input{"int main(){ int x = 5; switch(x){ case 1: return 3; case 3: return 2; case 5: return 1; default: return 0; } }"};
    const std::string outputNoExt{ "tmp4" };
    const std::string output{ outputNoExt + ext[0] };
    
    Compiler::compile(input, output);

    std::string asmCmd = std::format("as {} -o {}", output, outputNoExt + ext[1]);
    int assemble = system(asmCmd.c_str());
    ASSERT_EQ(assemble, 0) << std::format("{} failed.\n", asmCmd);

    std::string linkCmd = std::format("ld {} -o {}", outputNoExt + ext[1], outputNoExt);
    int link = system(linkCmd.c_str());
    ASSERT_EQ(link, 0) << std::format("{} failed.\n",linkCmd);

    std::string runCmd = std::format("./{}", outputNoExt);
    int run_status = system(runCmd.c_str());
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 1);

    // cleanup files
    for(const auto& e : ext){
        std::filesystem::remove(std::format("{}{}", outputNoExt, e));
    }
}

TEST(CodeGenTest, SwitchStatementDefault){
    std::vector<std::string> ext = {".s", ".o", ""};

    const std::string input{"int main(){ int x = 5; switch(x){ case 1: return 3; case 3: return 2; default: return 0; } }"};
    const std::string outputNoExt{ "tmp5" };
    const std::string output{ outputNoExt + ext[0] };
    
    Compiler::compile(input, output);

    std::string asmCmd = std::format("as {} -o {}", output, outputNoExt + ext[1]);
    int assemble = system(asmCmd.c_str());
    ASSERT_EQ(assemble, 0) << std::format("{} failed.\n", asmCmd);

    std::string linkCmd = std::format("ld {} -o {}", outputNoExt + ext[1], outputNoExt);
    int link = system(linkCmd.c_str());
    ASSERT_EQ(link, 0) << std::format("{} failed.\n",linkCmd);

    std::string runCmd = std::format("./{}", outputNoExt);
    int run_status = system(runCmd.c_str());
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 0);

    // cleanup files
    for(const auto& e : ext){
        std::filesystem::remove(std::format("{}{}", outputNoExt, e));
    }
}

TEST(CodeGenTest, WhileStatement){
    std::vector<std::string> ext = {".s", ".o", ""};

    const std::string input{"int main(){ int x = 5; while(x > 0) x = x - 1; return x; }"};
    const std::string outputNoExt{ "tmp6" };
    const std::string output{ outputNoExt + ext[0] };
    
    Compiler::compile(input, output);

    std::string asmCmd = std::format("as {} -o {}", output, outputNoExt + ext[1]);
    int assemble = system(asmCmd.c_str());
    ASSERT_EQ(assemble, 0) << std::format("{} failed.\n", asmCmd);

    std::string linkCmd = std::format("ld {} -o {}", outputNoExt + ext[1], outputNoExt);
    int link = system(linkCmd.c_str());
    ASSERT_EQ(link, 0) << std::format("{} failed.\n",linkCmd);

    std::string runCmd = std::format("./{}", outputNoExt);
    int run_status = system(runCmd.c_str());
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 0);

    // cleanup files
    for(const auto& e : ext){
        std::filesystem::remove(std::format("{}{}", outputNoExt, e));
    }
}

TEST(CodeGenTest, DoWhileStatement){
    std::vector<std::string> ext = {".s", ".o", ""};

    const std::string input{"int main(){ int x = 5; do{ x = x + 3; }while(x < 10); return x; }"};
    const std::string outputNoExt{ "tmp7" };
    const std::string output{ outputNoExt + ext[0] };
    
    Compiler::compile(input, output);

    std::string asmCmd = std::format("as {} -o {}", output, outputNoExt + ext[1]);
    int assemble = system(asmCmd.c_str());
    ASSERT_EQ(assemble, 0) << std::format("{} failed.\n", asmCmd);

    std::string linkCmd = std::format("ld {} -o {}", outputNoExt + ext[1], outputNoExt);
    int link = system(linkCmd.c_str());
    ASSERT_EQ(link, 0) << std::format("{} failed.\n",linkCmd);

    std::string runCmd = std::format("./{}", outputNoExt);
    int run_status = system(runCmd.c_str());
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 11);

    // cleanup files
    for(const auto& e : ext){
        std::filesystem::remove(std::format("{}{}", outputNoExt, e));
    }
}

TEST(CodeGenTest, ForStatement){
    std::vector<std::string> ext = {".s", ".o", ""};

    const std::string input{"int main(){ int x = 5; int i; for(i = 0; i < 10; i = i + 1) x = x + 1; return x; }"};
    const std::string outputNoExt{ "tmp8" };
    const std::string output{ outputNoExt + ext[0] };
    
    Compiler::compile(input, output);

    std::string asmCmd = std::format("as {} -o {}", output, outputNoExt + ext[1]);
    int assemble = system(asmCmd.c_str());
    ASSERT_EQ(assemble, 0) << std::format("{} failed.\n", asmCmd);

    std::string linkCmd = std::format("ld {} -o {}", outputNoExt + ext[1], outputNoExt);
    int link = system(linkCmd.c_str());
    ASSERT_EQ(link, 0) << std::format("{} failed.\n",linkCmd);

    std::string runCmd = std::format("./{}", outputNoExt);
    int run_status = system(runCmd.c_str());
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 15);

    // cleanup files
    for(const auto& e : ext){
        std::filesystem::remove(std::format("{}{}", outputNoExt, e));
    }
}

#else

TEST(CodeGenTest, SkippedDueToArchitecture) {
    GTEST_SKIP() << "Skipping: Not running on an x86_64 system.";
}

#endif