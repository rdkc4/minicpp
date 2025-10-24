#ifndef ASTVARIABLE_HPP
#define ASTVARIABLE_HPP

#include <memory>

#include "ASTStatement.hpp"
#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"

class ASTVariable final : public ASTStatement {
public:
    ASTVariable(const Token& token, ASTNodeType ntype, Types type = Types::NO_TYPE);

    Types getType() const noexcept;

    void setType(Types t) noexcept;

    template<typename Self>
    decltype(auto) getAssign(this Self&& self) noexcept {
        return std::forward<Self>(self).assignment.get();
    }

    void setAssign(std::unique_ptr<ASTExpression> assign);

    bool hasAssign() const noexcept;

    void print(size_t offset) const override;

private:
    Types type;
    std::unique_ptr<ASTExpression> assignment;

};

#endif