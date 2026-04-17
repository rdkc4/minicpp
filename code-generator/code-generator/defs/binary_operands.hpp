#ifndef BINARY_OPERANDS_HPP
#define BINARY_OPERANDS_HPP

#include <string>

/**
 * @struct BinaryOperands
 * @brief struct defining operands that are participating
 * in binary operation
*/
struct BinaryOperands {
    /// left register in binary operation
    std::string leftOperand;

    /// right operand in binary operation
    std::string rightOperand;

};

#endif