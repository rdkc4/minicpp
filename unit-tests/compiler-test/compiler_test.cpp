#include <gtest/gtest.h>

#include "compiler_fixture.hpp"

TEST_F(CompilerFixture, NoErr){
    initCompiler("int main(){ return 0; }");

    ASSERT_EQ(returnCode, compiler::ExitCode::NO_ERR);
}

TEST_F(CompilerFixture, NoErrScope){
    initCompiler("int main(){ {int a;} int a; return 0; }");

    ASSERT_EQ(returnCode, compiler::ExitCode::NO_ERR);
}

TEST_F(CompilerFixture, LexicalErr){
    initCompiler("int mai$n(){ return 0; }");

    ASSERT_EQ(returnCode, compiler::ExitCode::LEXICAL_ERR);
}

TEST_F(CompilerFixture, SyntaxErr){
    initCompiler("main(){ return 0; }");

    ASSERT_EQ(returnCode, compiler::ExitCode::SYNTAX_ERR);
}

TEST_F(CompilerFixture, SemanticErrTypeMismatch){
    initCompiler("int main(){ return 5 + 3u; }");

    ASSERT_EQ(returnCode, compiler::ExitCode::SEMANTIC_ERR);
}

TEST_F(CompilerFixture, SemanticErrUndefVariable){
    initCompiler("int main(){ return a; }");

    ASSERT_EQ(returnCode, compiler::ExitCode::SEMANTIC_ERR);
}

TEST_F(CompilerFixture, SemanticErrRedef){
    initCompiler("int main(){ int a; int a; return 0; }");

    ASSERT_EQ(returnCode, compiler::ExitCode::SEMANTIC_ERR);
}

TEST_F(CompilerFixture, IRErr){
    initCompiler("int main(){ return 3/0; }");

    ASSERT_EQ(returnCode, compiler::ExitCode::IR_ERR);
}