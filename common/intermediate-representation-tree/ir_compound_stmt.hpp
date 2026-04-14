#ifndef IR_COMPOUND_STMT_HPP
#define IR_COMPOUND_STMT_HPP

#include <memory>
#include <vector>

#include "ir_stmt.hpp"
#include "../visitor/ir_visitor.hpp"

namespace IR::node {
    /**
     * @class IRCompoundStmt
     * @brief IRT representation of the compound statement
    */
    class IRCompoundStmt final : public IRStmt {
    public:
        /** 
         * @brief Creates the instance of the irt compound statement
        */
        IRCompoundStmt();

        /**
         * @brief getter for the statements inside of the compound statement
         * @returns const vector of pointers to statement nodes
        */
        const std::vector<std::unique_ptr<IRStmt>>& getStmts() const noexcept;

        /**
         * @brief adds new statement to the compound statement node
         * @param stmt - pointer to a statement that is being added
        */
        void addStmt(std::unique_ptr<IRStmt> stmt);

        /**
         * @brief eliminates statements of the compound statement that appear after the node that always returns
         * @param startIdx - index in the vector of statements where deletion starts
        */
        void eliminateDeadStmts(size_t startIdx);

        /**
         * @brief accepts the ir visitor
         * @param visitor - reference to an ir visitor
        */
        void accept(IR::visitor::IRVisitor& visitor) override;

    private:
        /// vector of pointers to statements of the compound statement
        std::vector<std::unique_ptr<IRStmt>> stmts;
    };

}

#endif