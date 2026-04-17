#ifndef IR_LITERAL_EXPR_HPP
#define IR_LITERAL_EXPR_HPP

#include <string>

#include "ir_expr.hpp"
#include "../defs/defs.hpp"
#include "../visitor/ir_visitor.hpp"

namespace ir {
    /** 
     * @class IRLiteralExpr
     * @brief IRT representation of the literal node
    */
    class IRLiteralExpr final : public IRExpr {
    public:
        /** 
         * @brief Creates the instance of the irt literal
         * @param val - value of the literal
         * @param type - type of the literal
        */
        IRLiteralExpr(const std::string& val, Type type);

        /** 
         * @brief getter for the value of the literal
         * @returns reference to a value of the literal as const string
        */
        const std::string& getValue() const noexcept;

        /** 
         * @brief initializes the value of the literal
         * @param val - reference to a value
        */
        void setValue(const std::string& val);

        /**
         * @brief accepts the ir visitor
         * @param visitor - reference to an ir visitor
        */
        void accept(IRVisitor& visitor) override;

    private:
        /// value of the literal
        std::string value;
    };

}

#endif