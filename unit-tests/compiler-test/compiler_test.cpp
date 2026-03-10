#include <gtest/gtest.h>

#include "../../compiler/compiler.hpp"
#include "../test-utils/test_utils.hpp"

TEST(CompilerTest, NoErr){
    std::string input = "o.mcpp";
    std::string output = "o";

    __test__writeSourceToFile("int main(){ return 0; }", input);
    Compiler::ExitCode ret{ Compiler::compile({
        .stopAfterAssembly = true, 
        .input = input, 
        .output = output
    })};

    ASSERT_EQ(ret, Compiler::ExitCode::NO_ERR);

    /// file cleanup
    __test__removeFile(input);
    __test__removeFile(output + ".s");
}

TEST(CompilerTest, NoErrScope){
    std::string input = "o.mcpp";
    std::string output = "o";

    __test__writeSourceToFile("int main(){ {int a;} int a; return 0; }", input);
    Compiler::ExitCode ret{ Compiler::compile({
        .stopAfterAssembly = true, 
        .input = input, 
        .output = output
    })};

    ASSERT_EQ(ret, Compiler::ExitCode::NO_ERR);

    /// file cleanup
    __test__removeFile(input);
    __test__removeFile(output + ".s");
}

TEST(CompilerTest, LexicalErr){
    std::string input = "o.mcpp";
    std::string output = "o";

    __test__writeSourceToFile("int mai$n(){ return 0; }", input);
    Compiler::ExitCode ret{ Compiler::compile({
        .stopAfterAssembly = true, 
        .input = input, 
        .output = output
    })};

    ASSERT_EQ(ret, Compiler::ExitCode::LEXICAL_ERR);

    /// file cleanup
    __test__removeFile(input);
}

TEST(CompilerTest, SyntaxErr){
    std::string input = "o.mcpp";
    std::string output = "o";

    __test__writeSourceToFile("main(){ return 0; }", input);
    Compiler::ExitCode ret{ Compiler::compile({
        .stopAfterAssembly = true, 
        .input = input, 
        .output = output
    })};

    ASSERT_EQ(ret, Compiler::ExitCode::SYNTAX_ERR);

    /// file cleanup
    __test__removeFile(input);
}

TEST(CompilerTest, SemanticErrTypeMismatch){
    std::string input = "o.mcpp";
    std::string output = "o";

    __test__writeSourceToFile("int main(){ return 5 + 3u; }", input);
    Compiler::ExitCode ret{ Compiler::compile({
        .stopAfterAssembly = true, 
        .input = input, 
        .output = output
    })};

    ASSERT_EQ(ret, Compiler::ExitCode::SEMANTIC_ERR);

    /// file cleanup
    __test__removeFile(input);
}

TEST(CompilerTest, SemanticErrUndefVariable){
    std::string input = "o.mcpp";
    std::string output = "o";

    __test__writeSourceToFile("int main(){ return a; }", input);
    Compiler::ExitCode ret{ Compiler::compile({
        .stopAfterAssembly = true, 
        .input = input, 
        .output = output
    })};

    ASSERT_EQ(ret, Compiler::ExitCode::SEMANTIC_ERR);

    /// file cleanup
    __test__removeFile(input);
}

TEST(CompilerTest, SemanticErrRedef){
    std::string input = "o.mcpp";
    std::string output = "o";

    __test__writeSourceToFile("int main(){ int a; int a; return 0; }", input);
    Compiler::ExitCode ret{ Compiler::compile({
        .stopAfterAssembly = true, 
        .input = input, 
        .output = output
    })};

    ASSERT_EQ(ret, Compiler::ExitCode::SEMANTIC_ERR);

    /// file cleanup
    __test__removeFile(input);
}

TEST(CompilerTest, IRErr){
    std::string input = "o.mcpp";
    std::string output = "o";

    __test__writeSourceToFile("int main(){ return 3/0; }", input);
    Compiler::ExitCode ret{ Compiler::compile({
        .stopAfterAssembly = true, 
        .input = input, 
        .output = output
    })};

    ASSERT_EQ(ret, Compiler::ExitCode::IR_ERR);

    /// file cleanup
    __test__removeFile(input);
}