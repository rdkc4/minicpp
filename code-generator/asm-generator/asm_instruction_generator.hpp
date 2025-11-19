#ifndef ASM_INSTRUCTION_GENERATOR_HPP
#define ASM_INSTRUCTION_GENERATOR_HPP

#include <vector>
#include <string>
#include <cstddef>

namespace AsmGenerator {

    /** 
     * @namespace Instruction
     * @brief Module for generating the asm instructions
    */
    namespace Instruction {
        /// size of the register in bytes
        constexpr size_t regSize = 8;
        
        /** 
         * @brief generates the start of the asm file
         * @returns start of the asm file as a const string
        */
        const std::string genStart();

        /** 
         * @brief generates the mov instruction
         * @param asmCode - reference to a vector that contains asm code of the current function
         * @param l - left operand
         * @param r - right operand
         * @param ext - extension for the mov instruction
         * @details movext l, r
         * @returns void
        */
        void genMov(std::vector<std::string>& asmCode, std::string_view l, std::string_view r, std::string_view ext = "");

        /** 
         * @brief generates the cmp instruction
         * @param asmCode - reference to a vector that contains asm code of the current function
         * @param l - left operand
         * @param r - right operand
         * @details cmp l, r
         * @returns void
        */
        void genCmp(std::vector<std::string>& asmCode, std::string_view l, std::string_view r);

        /** 
         * @brief generates the operation instruction
         * @param asmCode - reference to a vector that contains asm code of the current function
         * @param op - operator
         * @param l - left operand
         * @param r - right operand (optional)
         * @details op l (, r)
         * @returns void
        */
        void genOperation(std::vector<std::string>& asmCode, std::string_view op, std::string_view l, std::string_view r = "");

        /** 
         * @brief generates the label
         * @param asmCode - reference to a vector that contains asm code of the current function
         * @param label - name of the label
         * @details label:
         * @returns void
        */
        void genLabel(std::vector<std::string>& asmCode, std::string_view label);

        /** 
         * @brief generates the ret instruction
         * @param asmCode - reference to a vector that contains asm code of the current function
         * @details ret
         * @returns void
        */
        void genRet(std::vector<std::string>& asmCode);

        /** 
         * @brief generates the jump instruction
         * @param asmCode - reference to a vector that contains asm code of the current function
         * @param jmp - jump command
         * @param label - label to jump to
         * @details jmp label
         * @returns void
        */
        void genJmp(std::vector<std::string>& asmCode, std::string_view jmp, std::string_view label);

        /** 
         * @brief generates the call instruction
         * @param asmCode - reference to a vector that contains asm code of the current function
         * @param func - function that is being called
         * @details call func
        */
        void genCall(std::vector<std::string>& asmCode, std::string_view func);

        /** 
         * @brief generates the push instruction
         * @param asmCode - reference to a vector that contains asm code of the current function
         * @param r - operand pushed to stack
         * @details push r
         * @returns void
        */
        void genPush(std::vector<std::string>& asmCode, std::string_view r);

        /** 
         * @brief generates the pop instruction
         * @param asmCode - reference to a vector that contains asm code of the current function
         * @param r - operand where the value from the stack is popped
         * @details pop r
         * @returns void
        */
        void genPop(std::vector<std::string>& asmCode, std::string_view r);

        /** 
         * @brief generates the prologue of the function
         * @param asmCode - reference to a vector that contains asm code of the current function
         * @details
         *
         * push %rbp
         *
         * mov %rsp, %rbp
         * @returns void
        */
        void genFuncPrologue(std::vector<std::string>& asmCode);

        /** 
         * @brief generates the epilogue of the function
         * @param asmCode - reference to a vector that contains asm code of the current function
         * @details
         *
         * mov %rbp, %rsp
         *
         * pop %rbp
         * @returns void
        */
        void genFuncEpilogue(std::vector<std::string>& asmCode);

        /** 
         * @brief generates the system call for exit
         * @param asmCode - reference to a vector that contains asm code of the current function
         * @details
         * 
         * mov %rax, %rdi 
         *
         * movq $60, %rax 
         *
         * syscall
         * @returns void
        */
        void genExit(std::vector<std::string>& asmCode);

        /** 
         * @brief generates the new line
         * @param asmCode - reference to a vector that contains asm code of the current function
         * @returns void
        */
        void genNewLine(std::vector<std::string>& asmCode);

    };
};

#endif