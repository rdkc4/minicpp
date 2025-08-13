#ifndef IRPROGRAM_HPP
#define IRPROGRAM_HPP

#include <vector>
#include <memory>

#include "IRNode.hpp"
#include "IRFunction.hpp"
#include "defs/IRTree_defs.hpp"

class IRProgram final : public IRNode {
public:
    IRProgram(IRNodeType ntype);

    const std::vector<std::unique_ptr<IRFunction>>& getFunctions() const noexcept;

    const IRFunction* getFunctionAtN(size_t n) const noexcept;

    void resizeFunctions(size_t n);

    void setFunctionAtN(std::unique_ptr<IRFunction> _function, size_t n);

    size_t getFunctionCount() const noexcept;

    void print(size_t offset) const override;

private:
    std::vector<std::unique_ptr<IRFunction>> functions;
};


#endif