#ifndef ASM_INSTRUCTION_GENERATOR_HPP
#define ASM_INSTRUCTION_GENERATOR_HPP

#include <atomic>
#include <string_view>
#include <vector>
#include <string>
#include <cstddef>

/** 
 * @namespace assembly
 * @brief Module for generating the x86-64 assembly
*/
namespace assembly {
    /// size of the register in bytes
    constexpr size_t regSize{8};

    /// number for the next label
    extern std::atomic<size_t> labelNum;

    /** 
     * @brief generates the unique number for the label
     * @returns number of the label
    */
    size_t getNextLabelNum() noexcept;
    
    /** 
     * @brief generates the start of the asm file
     * @returns start of the asm file as a const string
    */
    const std::string genStart();

    /** 
     * @brief generates the mov instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param src - source operand
     * @param dest - destination operand
     * @param ext - extension for the mov instruction, defaults to empty string
     * @details movext src, dest
    */
    void genMov(std::vector<std::string>& asmCode, std::string_view src, std::string_view dest, std::string_view ext = "");

    /**
     * @brief generates the set instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param dest - destination operand
     * @param ext - extension for set instruction
     * @details setext dest
    */
    void genSet(std::vector<std::string>& asmCode, std::string_view dest, std::string_view ext);

    /**
     * @brief generates the set instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param set - set instruction
     * @param dest - destination operand
     * @details setcc dest
    */
    void genSetcc(std::vector<std::string>& asmCode, std::string_view setcc, std::string_view dest);

    /** 
     * @brief generates the test instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param op - operand
     * @param ext - extension for the mov instruction, defaults to empty string
     * @details testext op, op
    */
    void genTest(std::vector<std::string>& asmCode, std::string_view op, std::string_view ext = "");

    /** 
     * @brief generates the test instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param lOp - left operand
     * @param rOp - right operand
     * @param ext - extension for the mov instruction, defaults to empty string
     * @details testext lOp, rOp
    */
    void genTest(std::vector<std::string> &asmCode, std::string_view lOp, std::string_view rOp, std::string_view ext = "");

    /** 
     * @brief generates the cmp instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param lOp - left operand
     * @param rOp - right operand
     * @details cmp lOp, rOp
    */
    void genCmp(std::vector<std::string>& asmCode, std::string_view lOp, std::string_view rOp);

    /** 
     * @brief generates the operation instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param operation - performed operation
     * @param src - source operand
     * @param dest - destination operand
     * @details operation lOp, rOp
    */
    void genOperation(std::vector<std::string>& asmCode, std::string_view operation, std::string_view src, std::string_view dest);

    /** 
     * @brief generates the operation instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param operation - performed operation
     * @param dest - destination operand
     * @details operation dest
    */
    void genOperation(std::vector<std::string>& asmCode, std::string_view operation, std::string_view dest);
    
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
     * @param src - source operand
     * @details push src
    */
    void genPush(std::vector<std::string>& asmCode, std::string_view src);

    /** 
     * @brief generates the pop instruction
     * @param asmCode - reference to a vector that contains asm code of the current function
     * @param dest - destination operand
     * @details pop dest
    */
    void genPop(std::vector<std::string>& asmCode, std::string_view dest);

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