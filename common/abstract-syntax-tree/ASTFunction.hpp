#ifndef ASTFUNCTION_HPP
#define ASTFUNCTION_HPP

#include <memory>
#include <vector>

#include "ASTNode.hpp"
#include "ASTParameter.hpp"
#include "ASTStatement.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"

class ASTFunction final : public ASTNode {
public:
    ASTFunction(const Token& token, ASTNodeType ntype, Types type = Types::NO_TYPE);

    const std::vector<std::unique_ptr<ASTParameter>>& getParameters() const noexcept;

    const std::vector<std::unique_ptr<ASTStatement>>& getBody() const noexcept;

    void addParameter(std::unique_ptr<ASTParameter> parameter);

    void addStatement(std::unique_ptr<ASTStatement> statement);

    Types getType() const noexcept;

    size_t getParameterCount() const noexcept;

    void print(size_t offset) const override;

private:
    Types type;
    std::vector<std::unique_ptr<ASTParameter>> parameters;
    std::vector<std::unique_ptr<ASTStatement>> body;

};

#endif