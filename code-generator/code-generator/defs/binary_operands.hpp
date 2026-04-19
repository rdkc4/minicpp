#ifndef BINARY_OPERANDS_HPP
#define BINARY_OPERANDS_HPP

#include <string>

namespace code_gen {
    /**
     * @struct BinaryOperands
     * @brief struct defining the operands of the binary operation
    */
    struct BinaryOperands {
        /// left register in binary operation
        std::string leftOperand;

        /// right operand in binary operation
        std::string rightOperand;

    };

}

#endif