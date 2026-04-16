#include <gtest/gtest.h>

#include "code_generator_fixture.hpp"

#if defined(__x86_64__) && defined(__clang__)

TEST_F(CodeGeneratorFixture, ReturnsExpr){
    initCodeGen("int main(){ return 1 + 2 * 3 - 4 / 2; }");
    
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 5);
}

TEST_F(CodeGeneratorFixture, ReturnsFuncCall){
    initCodeGen("int sq(int x){ return x * x; } int main(){ return sq(5); }");

    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 25);
}

TEST_F(CodeGeneratorFixture, ReturnsRecursiveFuncCall){
    initCodeGen("int fib(int n){ if(n == 0) return 0; else if(n == 1) return 1; else return fib(n-1) + fib(n-2); } int main(){ return fib(6); }");

    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 8);
}

TEST_F(CodeGeneratorFixture, SwitchStatementCase){
    initCodeGen("int main(){ int x = 5; switch(x){ case 1: return 3; case 3: return 2; case 5: return 1; default: return 0; } }");

    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 1);
}

TEST_F(CodeGeneratorFixture, SwitchStatementDefault){
    initCodeGen("int main(){ int x = 5; switch(x){ case 1: return 3; case 3: return 2; default: return 0; } }");

    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 0);
}

TEST_F(CodeGeneratorFixture, WhileStatement){
    initCodeGen("int main(){ int x = 5; while(x > 0) x = x - 1; return x; }");

    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 0);
}

TEST_F(CodeGeneratorFixture, DoWhileStatement){
    initCodeGen("int main(){ int x = 5; do{ x = x + 3; }while(x < 10); return x; }");

    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 11);
}

TEST_F(CodeGeneratorFixture, ForStatement){
    initCodeGen("int main(){ int x = 5; int i; for(i = 0; i < 10; i = i + 1) x = x + 1; return x; }");

    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 15);
}

TEST_F(CodeGeneratorFixture, ReturnsLogicalExpr){
    initCodeGen("int main(){ return 1 < 2 && 3 > 4 || 2 <= 3; }");
    
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 1);
}

TEST_F(CodeGeneratorFixture, IfStatementLiteralCondition){
    initCodeGen("int main(){ if(1) return 0; return 1; }");
    
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 0);
}

TEST_F(CodeGeneratorFixture, DoWhileStatementIdCondition){
    initCodeGen("int main(){ int x = 3; do{ x = x - 1; }while(x); return x; }");
    
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 0);
}

TEST_F(CodeGeneratorFixture, WhileStatementFunctionCallCondition){
    initCodeGen("int fn(int x){ return x; } int main(){ int x = 3; while(fn(x)) x = x - 1; return x; }");
    
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 0);
}

TEST_F(CodeGeneratorFixture, ForStatementComplexConditionExpression){
    initCodeGen("int main(){ int a = 5; int b = 10; int c = 3; int d = 8; int sum = 0; int i; for(i = 0;((i < 20) && ((a + b * c - d / 2) > 0) && ((i << 1) < (b >> 1)) && ((a ^ b) != (c | d)) && ((a & d) <= (b + c))) || ((i & 3) == 0); i = i + 1){ sum = sum + (i * 2 + (a - c)); } return sum; }");
    
    ASSERT_TRUE(WIFEXITED(run_status)) << "Process didn't exit normally.\n";

    int exitCode = WEXITSTATUS(run_status);
    EXPECT_EQ(exitCode, 12);
}

#else

TEST(CodeGeneratorFixture, SkippedDueToArchitecture) {
    GTEST_SKIP() << "Skipping: Not running on an x86_64 system.";
}

#endif