#ifndef IRTEMPORARY_HPP
#define IRTEMPORARY_HPP

#include <memory>
#include <vector>
#include <string>
#include <utility>

#include "IRNode.hpp"
#include "IRExpression.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

class IRTemporary final : public IRNode {
public:
    IRTemporary(IRNodeType ntype);

    const std::vector<std::unique_ptr<IRExpression>>& getTemporaries() const noexcept;

    const std::vector<std::string>& getTemporaryNames() const noexcept;

    const std::string& getTemporaryNameAtN(size_t n) const noexcept;

    const IRExpression* getExpressionAtN(size_t n) const noexcept;
    
    std::pair<std::string, IRExpression*> getTemporaryNCAtN(size_t n) noexcept;

    void addTemporary(const std::string& tempName, std::unique_ptr<IRExpression> tempVal = nullptr, Types type = Types::NO_TYPE);

    void assignTempAtN(std::unique_ptr<IRExpression> tempVal, Types t, size_t n);

    const std::vector<Types>& getTypes() const noexcept;

    void print(size_t offset) const override;

private:
    std::vector<Types> types;
    std::vector<std::string> tempNames;
    std::vector<std::unique_ptr<IRExpression>> temporaries;
};

#endif