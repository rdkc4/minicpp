#ifndef ASTSWITCH_ST_HPP
#define ASTSWITCH_ST_HPP

#include <memory>
#include <vector>

#include "ASTStatement.hpp"
#include "ASTId.hpp"
#include "ASTCaseSt.hpp"
#include "ASTDefaultSt.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

/** 
 * @class ASTSwitchSt
 * @brief AST node representing switch statement
*/
class ASTSwitchSt final : public ASTStatement {
public:
    ASTSwitchSt(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for a variable (id of a variable)
     * @returns pointer or const pointer to id
    */
    template<typename Self>
    decltype(auto) getVariable(this Self&& self) noexcept {
        return std::forward<Self>(self).variable.get();
    }

    /** 
     * @brief initializes id of a variable
     * @param var - pointer to id of a variable
     * @returns void
    */
    void setVariable(std::unique_ptr<ASTId> var);

    /** 
     * @brief getter for the cases of the switch statement
     * @returns reference to a vector of pointers to cases
    */
    const std::vector<std::unique_ptr<ASTCaseSt>>& getCases() const noexcept;

    /** 
     * @brief getter for the default statement of the switch statement
     * @returns const pointer to a default statement
    */
    const ASTDefaultSt* getDefault() const noexcept;

    /** 
     * @brief adds new case
     * @param _case - pointer to a case that is being added
     * @returns void
    */
    void addCase(std::unique_ptr<ASTCaseSt> _case);

    /** 
     * @brief adds default statement
     * @param _swDefault - pointer to a default statement
     * @returns void
    */
    void setDefault(std::unique_ptr<ASTDefaultSt> _swDefault);

    /** 
     * @brief checks if default statement exists
     * @returns true if default exists, false otherwise
    */
    bool hasDefault() const noexcept;

    /** 
     * @brief prints switch statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::unique_ptr<ASTId> variable;
    std::vector<std::unique_ptr<ASTCaseSt>> cases;
    std::unique_ptr<ASTDefaultSt> _default;

};

#endif