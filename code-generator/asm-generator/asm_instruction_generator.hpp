#ifndef ASM_INSTRUCTION_GENERATOR_HPP
#define ASM_INSTRUCTION_GENERATOR_HPP

#include <vector>
#include <string>
#include <cstddef>

/** 
 * @namespace AsmGenerator::Instruction
 * @brief Module for generating the x86-64 asm instructions
*/
namespace AsmGenerator::Instruction {
    /// size of the register in bytes
    constexpr size_t regSize{8};
    
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
    */
    void genMov(std::vector<std::string>& asmCode, std::string_view l, std::string_view r, std::string_view ext = "");

    /**
     * @brief generates the set instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param r - operand that is being set
     * @param ext - extension for set instruction
     * @details setext r
    */
    void genSet(std::vector<std::string>& asmCode, std::string_view r, std::string_view ext);

    /**
     * @brief generates the set instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param set - set instruction
     * @param r - operand that is being set
     * @details setcc r
    */
    void genSetcc(std::vector<std::string>& asmCode, std::string_view setcc, std::string_view r);

    /** 
     * @brief generates the test instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param r - operand that is being tested
     * @param ext - extension for the mov instruction
     * @details testext r, r
    */
    void genTest(std::vector<std::string>& asmCode, std::string_view r, std::string_view ext = "");

    /** 
     * @brief generates the cmp instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param l - left operand
     * @param r - right operand
     * @details cmp l, r
    */
    void genCmp(std::vector<std::string>& asmCode, std::string_view l, std::string_view r);

    /** 
     * @brief generates the operation instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param op - operator
     * @param l - left operand
     * @param r - right operand
     * @details op l, r
    */
    void genOperation(std::vector<std::string>& asmCode, std::string_view op, std::string_view l, std::string_view r);

    /** 
     * @brief generates the operation instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param op - operator
     * @param r - operand
     * @details op r
    */
    void genOperation(std::vector<std::string>& asmCode, std::string_view op, std::string_view r);
    
    /** 
     * @brief generates the label
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param label - name of the label
     * @details label:
    */
    void genLabel(std::vector<std::string>& asmCode, std::string_view label);

    /** 
     * @brief generates the ret instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @details ret
    */
    void genRet(std::vector<std::string>& asmCode);

    /** 
     * @brief generates the jump instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param label - label to jump to
     * @details unconditional jump - jmp label
    */
    void genJmp(std::vector<std::string>& asmCode, std::string_view label);

    /** 
     * @brief generates the jump instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param jcc - jump command
     * @param label - label to jump to
     * @details jcc label
    */
    void genJcc(std::vector<std::string>& asmCode, std::string_view jcc, std::string_view label);

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
    */
    void genPush(std::vector<std::string>& asmCode, std::string_view r);

    /** 
     * @brief generates the pop instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param r - operand where the value from the stack is popped
     * @details pop r
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
    */
    void genExit(std::vector<std::string>& asmCode);

    /** 
     * @brief generates the new line
     * @param asmCode - reference to a vector that contains asm code of the current function
    */
    void genNewLine(std::vector<std::string>& asmCode);

};

#endif