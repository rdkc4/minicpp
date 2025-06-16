#ifndef ASM_GENERATOR_HPP
#define ASM_GENERATOR_HPP

#include <string>
#include <vector>

class AsmGenerator{
    public:
        
        AsmGenerator();

        // start of an asm file
        const std::string genStart();

        // asm instructions
        void genMov(std::vector<std::string>& asmCode, std::string_view l, std::string_view r, std::string_view ext = "");
        void genCmp(std::vector<std::string>& asmCode, std::string_view l, std::string_view r);
        void genOperation(std::vector<std::string>& asmCode, std::string_view op, std::string_view l, std::string_view r = "");
        void genLabel(std::vector<std::string>& asmCode, std::string_view label);
        void genRet(std::vector<std::string>& asmCode);
        void genJmp(std::vector<std::string>& asmCode, std::string_view jmp, std::string_view label);
        void genCall(std::vector<std::string>& asmCode, std::string_view func);

        // stack instructions
        void genPush(std::vector<std::string>& asmCode, std::string_view r);
        void genPop(std::vector<std::string>& asmCode, std::string_view r);

        // function prologue
        void genFuncPrologue(std::vector<std::string>& asmCode);

        // function epilogue
        void genFuncEpilogue(std::vector<std::string>& asmCode);

        // system call for exit
        void genExit(std::vector<std::string>& asmCode);

        void genNewLine(std::vector<std::string>& asmCode);

        // generating function that prints values
        void printfFunction(std::vector<std::string>& printfFunc);

};

#endif