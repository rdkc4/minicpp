#ifndef IR_SWITCH_STMT_HPP
#define IR_SWITCH_STMT_HPP

#include <memory>
#include <vector>

#include "ir_stmt.hpp"
#include "ir_id_expr.hpp"
#include "ir_case_stmt.hpp"
#include "ir_default_stmt.hpp"
#include "../visitor/ir_visitor.hpp"

namespace ir {
    /** 
     * @class IRSwitchStmt
     * @brief IRT representation of the switch statement
    */
    class IRSwitchStmt final : public IRStmt {
    public:
        /** 
         * @brief Creates the instance of the irt switch-statement
        */
        IRSwitchStmt();

        /** 
         * @brief getter for the variable of the switch statement
         * @returns pointer or const pointer to the id of the switch statement
        */
        template<typename Self>
        decltype(auto) getVariableIdExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).variableIdExpr.get();
        }

        /** 
         * @brief initializes the variable of the switch statement
         * @param idExpr - pointer to the id expression
        */
        void setVariableIdExpr(std::unique_ptr<IRIdExpr> idExpr);

        /** 
         * @brief getter for the cases of the switch statement
         * @returns reference to a const vector of pointers to cases
        */
        const std::vector<std::unique_ptr<IRCaseStmt>>& getCaseStmts() const noexcept;

        /** 
         * @brief getter for the case at the specified position
         * @param n - position of the case
         * @returns const pointer to the case
        */
        const IRCaseStmt* getCaseStmtAtN(size_t n) const noexcept;

        /** 
         * @brief adds new case to the switch statement
         * @param caseStmt - pointer to the case
        */
        void addCaseStmt(std::unique_ptr<IRCaseStmt> caseStmt);

        /** 
         * @brief getter for the default case of the switch statement
         * @returns pointer or const pointer to the default case
        */
        template<typename Self>
        decltype(auto) getDefaultStmt(this Self&& self) noexcept {
            return std::forward<Self>(self).defaultStmt.get();
        }

        /** 
         * @brief initializes the default case of the switch statement
         * @param swDefaultStmt - pointer to the default case
        */
        void setDefaultStmt(std::unique_ptr<IRDefaultStmt> swDefaultStmt);

        /** 
         * @brief checks if the switch statement has default case
         * @returns true if switch statement has default, false otherwise
        */
        bool hasDefaultStmt() const noexcept;

        /** 
         * @brief getter for the count of cases
         * @returns number of cases of the switch statement
         * @note default case is not included
        */
        size_t getCaseCount() const noexcept;

        /**
         * @brief accepts the ir visitor
         * @param visitor - reference to an ir visitor
        */
        void accept(IRVisitor& visitor) override;

    private:
        /// pointer to the id of the variable of the switch-statement
        std::unique_ptr<IRIdExpr> variableIdExpr;

        /// vector of pointers to cases of the switch-statement
        std::vector<std::unique_ptr<IRCaseStmt>> caseStmts;

        /// pointer to default case of the switch-statement
        std::unique_ptr<IRDefaultStmt> defaultStmt;
        
    };

}

#endif