#ifndef ASTPROGRAM_HPP
#define ASTPROGRAM_HPP

#include <memory>

#include "ASTNode.hpp"
#include "ASTFunction.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

class ASTProgram final : public ASTNode {
public:
    ASTProgram(const Token& token, ASTNodeType ntype);

    const std::vector<std::unique_ptr<ASTFunction>>& getFunctions() const noexcept;

    size_t getFunctionCount() const noexcept;

    const ASTFunction* getFunctionAtN(size_t n) const noexcept;

    void addFunction(std::unique_ptr<ASTFunction> function);

    void print(size_t offset) const override;

private:
    std::vector<std::unique_ptr<ASTFunction>> functions;

};

#endif