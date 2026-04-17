#ifndef CODE_GENERATOR_DEFS_HPP
#define CODE_GENERATOR_DEFS_HPP

#include <array>
#include <cassert>

#include "../../../common/intermediate-representation-tree/defs/ir_defs.hpp"

/**
 * @enum ExprContext
 * @brief context in which expression appears
*/
enum class ExprContext {
    VALUE,
    BRANCH
};

/**
 * @struct JumpInfo
 * @brief structure providing the jump, opposite jump, and set instructions
*/
struct JumpInfo {
    /// jump instruction
    std::string_view jcc;

    /// inverse jump instruction
    std::string_view jccInverse;

    /// set instruction
    std::string_view setcc;
};

/// invalid jump info entry
constexpr JumpInfo invalidJumpInfo { .jcc = "", .jccInverse = "", .setcc = "" };

/// maps jump ir node types to their jump information
constexpr std::array<JumpInfo, ir::IR_NODE_TYPE_COUNT> irNodeJumpInfo {
    [] {
        /**
         * @brief converts ir node type enum to array index
         * @param type - element of the ir node type enum
         * @returns index in the array for provided node type
        */
        constexpr auto idx {
            [](ir::IRNodeType type) noexcept -> size_t {
                return static_cast<size_t>(type);
            }
        };

        std::array<JumpInfo, ir::IR_NODE_TYPE_COUNT> nodes{};
        nodes.fill(invalidJumpInfo);

        nodes[idx(ir::IRNodeType::JG)]  = {.jcc = "jg",  .jccInverse = "jle", .setcc = "setg" };
        nodes[idx(ir::IRNodeType::JA)]  = {.jcc = "ja",  .jccInverse = "jbe", .setcc = "seta" };
        nodes[idx(ir::IRNodeType::JL)]  = {.jcc = "jl",  .jccInverse = "jge", .setcc = "setl" };
        nodes[idx(ir::IRNodeType::JB)]  = {.jcc = "jb",  .jccInverse = "jae", .setcc = "setb" };
        nodes[idx(ir::IRNodeType::JGE)] = {.jcc = "jge", .jccInverse = "jl",  .setcc = "setge"};
        nodes[idx(ir::IRNodeType::JAE)] = {.jcc = "jae", .jccInverse = "jb",  .setcc = "setae"};
        nodes[idx(ir::IRNodeType::JLE)] = {.jcc = "jle", .jccInverse = "jg",  .setcc = "setle"};
        nodes[idx(ir::IRNodeType::JBE)] = {.jcc = "jbe", .jccInverse = "ja",  .setcc = "setbe"};
        nodes[idx(ir::IRNodeType::JE)]  = {.jcc = "je",  .jccInverse = "jne", .setcc = "sete" };
        nodes[idx(ir::IRNodeType::JNE)] = {.jcc = "jne", .jccInverse = "je",  .setcc = "setne"};

        return nodes;
    }()
};

/**
 * @brief convert ir node type to jump info
 * @param type - ir node type
 * @returns jump info of the ir node
*/
constexpr JumpInfo irNodeTypeToJumpInfo(ir::IRNodeType type) {
    return irNodeJumpInfo[static_cast<size_t>(type)];
}

/// array of names of the general-purpose registers
constexpr std::array<std::string_view, 8> gpRegisters {
    "%r8", 
    "%r9", 
    "%r10", 
    "%r11", 
    "%r12", 
    "%r13", 
    "%r14", 
    "%r15"
};

#endif
