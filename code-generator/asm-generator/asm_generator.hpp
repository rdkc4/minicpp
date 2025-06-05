#ifndef ASM_GENERATOR_HPP
#define ASM_GENERATOR_HPP

#include <fstream>
#include <string>

class AsmGenerator{
    public:
        
        AsmGenerator(std::string& file);

        ~AsmGenerator() noexcept;

        bool isOpen() const;

        // start of an asm file
        void genStart();

        // asm instructions
        void genMov(std::string_view l, std::string_view r, std::string_view ext = "");
        void genCmp(std::string_view l, std::string_view r);
        void genOperation(std::string_view op, std::string_view l, std::string_view r = "");
        void genLabel(std::string_view label);
        void genRet();
        void genJmp(std::string_view jmp, std::string_view label);
        void genCall(std::string_view func);

        // stack instructions
        void genPush(std::string_view r);
        void genPop(std::string_view r);

        // function prologue
        void genFuncPrologue();

        // function epilogue
        void genFuncEpilogue();

        // system call for exit
        void genExit();

        void genNewLine();

    private:
        // output file for asm code (.s)
        std::ofstream output;


};

#endif