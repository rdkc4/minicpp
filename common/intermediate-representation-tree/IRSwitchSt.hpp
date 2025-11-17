#ifndef IRSWITCH_ST_HPP
#define IRSWITCH_ST_HPP

#include <memory>
#include <vector>

#include "IRStatement.hpp"
#include "IRId.hpp"
#include "IRCaseSt.hpp"
#include "IRDefaultSt.hpp"
#include "defs/IRTree_defs.hpp"

/** 
 * @class IRSwitchSt
 * @brief IRT representation of the switch statement
*/
class IRSwitchSt final : public IRStatement {
public:
    IRSwitchSt(IRNodeType ntype);

    /** 
     * @brief getter for the variable of the switch statement
     * @returns const pointer to the id of the switch statement
    */
    const IRId* getVariable() const noexcept;

    /** 
     * @brief initializes the variable of the switch statement
     * @param var - pointer to the id
     * @returns void
    */
    void setVariable(std::unique_ptr<IRId> var);

    /** 
     * @brief getter for the cases of the switch statement
     * @returns reference to a const vector of pointers to cases
    */
    const std::vector<std::unique_ptr<IRCaseSt>>& getCases() const noexcept;

    /** 
     * @brief getter for the case at the specified position
     * @param n - position of the case
     * @returns const pointer to the case
    */
    const IRCaseSt* getCaseAtN(size_t n) const noexcept;

    /** 
     * @brief adds new case to the switch statement
     * @param _case - pointer to the case
     * @returns void
    */
    void addCase(std::unique_ptr<IRCaseSt> _case);

    /** 
     * @brief getter for the default case of the switch statement
     * @returns pointer or const pointer to the default case
    */
    template<typename Self>
    decltype(auto) getDefault(this Self&& self) noexcept {
        return std::forward<Self>(self)._default.get();
    }

    /** 
     * @brief initializes the default case of the switch statement
     * @param _swDefault - pointer to the default case
     * @returns void
    */
    void setDefault(std::unique_ptr<IRDefaultSt> _swDefault);

    /** 
     * @brief checks if the switch statement has default case
     * @returns true if switch statement has default, false otherwise
    */
    bool hasDefault() const noexcept;

    /** 
     * @brief getter for the count of cases
     * @returns number of cases of the switch statement
     * @note default case is not included
    */
    size_t getCaseCount() const noexcept;

    void print(size_t offset) const override;

private:
    std::unique_ptr<IRId> variable;
    std::vector<std::unique_ptr<IRCaseSt>> cases;
    std::unique_ptr<IRDefaultSt> _default;
};


#endif