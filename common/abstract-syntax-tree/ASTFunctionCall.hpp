#ifndef ASTFUNCTION_CALL_HPP
#define ASTFUNCTION_CALL_HPP

#include <memory>
#include <vector>

#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"

class ASTFunctionCall final : public ASTExpression {
public:
    ASTFunctionCall(const Token& token, ASTNodeType ntype, Types type = Types::NO_TYPE);

    const std::vector<std::unique_ptr<ASTExpression>>& getArguments() const noexcept;

    const ASTExpression* getArgumentAtN(size_t n) const noexcept;

    void addArgument(std::unique_ptr<ASTExpression> arg);

    size_t getArgumentCount() const noexcept;

    void print(size_t offset) const override;

private:
    std::vector<std::unique_ptr<ASTExpression>> arguments;
};

#endif