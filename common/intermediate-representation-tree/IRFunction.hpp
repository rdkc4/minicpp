#ifndef IRFUNCTION_HPP
#define IRFUNCTION_HPP

#include <memory>
#include <vector>
#include <string>

#include "IRNode.hpp"
#include "IRParameter.hpp"
#include "IRStatement.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

class IRFunction final : public IRNode {
public:
    IRFunction(IRNodeType ntype, const std::string& funcName, Types type);

    const std::vector<std::unique_ptr<IRParameter>>& getParameters() const noexcept;

    void addParameter(std::unique_ptr<IRParameter> parameter);

    const std::vector<std::unique_ptr<IRStatement>>& getBody() const noexcept;

    void addStatement(std::unique_ptr<IRStatement> statement);

    const std::string& getFunctionName() const noexcept;

    void setFunctionName(const std::string& funcName);

    Types getType() const noexcept;

    void setType(Types t) noexcept;

    const std::string& getRequiredMemory() const noexcept;

    void setRequiredMemory(const std::string& size);

    void eliminateDead(size_t startIdx);

    void print(size_t offset) const override;

private:
    std::string functionName;
    std::string requiredMemory;
    Types type;
    std::vector<std::unique_ptr<IRParameter>> parameters;
    std::vector<std::unique_ptr<IRStatement>> body;
};

#endif