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

class ASTSwitchSt final : public ASTStatement {
public:
    ASTSwitchSt(const Token& token, ASTNodeType ntype);

    template<typename Self>
    decltype(auto) getVariable(this Self&& self) noexcept {
        return std::forward<Self>(self).variable.get();
    }

    void setVariable(std::unique_ptr<ASTId> var);

    const std::vector<std::unique_ptr<ASTCaseSt>>& getCases() const noexcept;

    const ASTDefaultSt* getDefault() const noexcept;

    void addCase(std::unique_ptr<ASTCaseSt> _case);

    void setDefault(std::unique_ptr<ASTDefaultSt> _swDefault);

    bool hasDefault() const noexcept;

    void print(size_t offset) const override;

private:
    std::unique_ptr<ASTId> variable;
    std::vector<std::unique_ptr<ASTCaseSt>> cases;
    std::unique_ptr<ASTDefaultSt> _default;

};

#endif