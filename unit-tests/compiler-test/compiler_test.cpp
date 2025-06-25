#include <gtest/gtest.h>
#include <filesystem>

#include "../../compiler/compiler.hpp"

TEST(CompilerTest, NoErr){
    std::string path{"o.s"};
    Compiler compiler;
    ExitCode ret{ compiler.compile("int main(){ return 0; }", path) };

    ASSERT_EQ(ret, ExitCode::NO_ERR);
    std::filesystem::remove(path); // clean up asm code
}

TEST(CompilerTest, NoErrScope){
    std::string path{"o.s"};
    Compiler compiler;
    ExitCode ret{ compiler.compile("int main(){ {int a;} int a; return 0; }", path) };

    ASSERT_EQ(ret, ExitCode::NO_ERR);
    std::filesystem::remove(path); // clean up asm code
}

TEST(CompilerTest, LexicalErr){
    std::string path{ "o.s"};
    Compiler compiler;
    ExitCode ret{ compiler.compile("int mai$n(){ return 0; }", path) };

    ASSERT_EQ(ret, ExitCode::LEXICAL_ERR);
}

TEST(CompilerTest, SyntaxErr){
    std::string path{ "o.s" };
    Compiler compiler;
    ExitCode ret{ compiler.compile("main(){ return 0; }", path) };

    ASSERT_EQ(ret, ExitCode::SYNTAX_ERR);
}

TEST(CompilerTest, SemanticErrTypeMismatch){
    std::string path{ "o.s" };
    Compiler compiler;
    ExitCode ret{ compiler.compile("int main(){ return 5 + 3u; }", path) };

    ASSERT_EQ(ret, ExitCode::SEMANTIC_ERR);
}

TEST(CompilerTest, SemanticErrUndefVariable){
    std::string path{ "o.s" };
    Compiler compiler;
    ExitCode ret{ compiler.compile("int main(){ return a; }", path) };

    ASSERT_EQ(ret, ExitCode::SEMANTIC_ERR);
}

TEST(CompilerTest, SemanticErrRedef){
    std::string path{ "o.s" };
    Compiler compiler;
    ExitCode ret{ compiler.compile("int main(){ int a; int a; return 0; }", path) };

    ASSERT_EQ(ret, ExitCode::SEMANTIC_ERR);
}

TEST(CompilerTest, IRErr){
    std::string path{ "o.s" };
    Compiler compiler;
    ExitCode ret{ compiler.compile("int main(){ return 3/0; }", path) };

    ASSERT_EQ(ret, ExitCode::IR_ERR);
}

TEST(CompilerTest, CodeGenErr){
    std::string path{ "" };
    Compiler compiler;
    ExitCode ret{ compiler.compile("int main(){ return 0; }", path) };

    ASSERT_EQ(ret, ExitCode::CODEGEN_ERR);
}