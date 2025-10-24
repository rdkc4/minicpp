#ifndef IRSWITCH_ST_HPP
#define IRSWITCH_ST_HPP

#include <memory>
#include <vector>

#include "IRStatement.hpp"
#include "IRId.hpp"
#include "IRCaseSt.hpp"
#include "IRDefaultSt.hpp"
#include "defs/IRTree_defs.hpp"

class IRSwitchSt final : public IRStatement {
public:
    IRSwitchSt(IRNodeType ntype);

    const IRId* getVariable() const noexcept;

    void setVariable(std::unique_ptr<IRId> var);

    const std::vector<std::unique_ptr<IRCaseSt>>& getCases() const noexcept;

    const IRCaseSt* getCaseAtN(size_t n) const noexcept;

    void addCase(std::unique_ptr<IRCaseSt> _case);

    template<typename Self>
    decltype(auto) getDefault(this Self&& self) noexcept {
        return std::forward<Self>(self)._default.get();
    }

    void setDefault(std::unique_ptr<IRDefaultSt> _swDefault);

    bool hasDefault() const noexcept;

    size_t getCaseCount() const noexcept;

    void print(size_t offset) const override;

private:
    std::unique_ptr<IRId> variable;
    std::vector<std::unique_ptr<IRCaseSt>> cases;
    std::unique_ptr<IRDefaultSt> _default;
};


#endif