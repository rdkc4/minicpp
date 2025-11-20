#ifndef IRCOMPOUND_ST_HPP
#define IRCOMPOUND_ST_HPP

#include <memory>
#include <vector>

#include "IRStatement.hpp"
#include "defs/IRTree_defs.hpp"

/**
 * @class IRCompoundSt
 * @brief IRT representation of the compound statement
*/
class IRCompoundSt final : public IRStatement {
public:
    /** 
     * @brief Creates the instance of the irt compound statement
     * @param ntype - type of the irt node
    */
    IRCompoundSt(IRNodeType ntype);

    /**
     * @brief getter for the statements inside of the compound statement
     * @returns const vector of pointers to statement nodes
    */
    const std::vector<std::unique_ptr<IRStatement>>& getStatements() const noexcept;

    /**
     * @brief adds new statement to the compound statement node
     * @param statement - pointer to a statement that is being added
     * @returns void
    */
    void addStatement(std::unique_ptr<IRStatement> statement);

    /**
     * @brief eliminates statements of the compound statement that appear after the node that always returns
     * @param startIdx - index in the vector of statements where deletion starts
     * @returns void
    */
    void eliminateDead(size_t startIdx);

    /**
     * @brief prints formatted string of the compound statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    /// vector of pointers to statements of the compound statement
    std::vector<std::unique_ptr<IRStatement>> statements;
};

#endif
