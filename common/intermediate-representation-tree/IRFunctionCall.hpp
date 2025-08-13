#ifndef IRFUNCTION_CALL_HPP
#define IRFUNCTION_CALL_HPP

#include <vector>
#include <string>
#include <memory>

#include "IRExpression.hpp"
#include "IRTemporary.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

class IRFunctionCall final : public IRExpression {
public:
    IRFunctionCall(IRNodeType ntype, const std::string& callName, Types type);

    const std::vector<std::unique_ptr<IRExpression>>& getArguments() const noexcept;

    const IRExpression* getArgumentAtN(size_t n) const noexcept;

    const std::vector<std::unique_ptr<IRTemporary>>& getTemporaries() const noexcept;

    void addArgument(std::unique_ptr<IRExpression> argument, std::unique_ptr<IRTemporary> temp = nullptr);

    const std::string& getCallName() const noexcept;

    void setCallName(const std::string& _callName);

    size_t getArgumentCount() const noexcept;

    void print(size_t offset) const override;

private:
    std::string callName;
    std::vector<std::unique_ptr<IRExpression>> arguments;
    std::vector<std::unique_ptr<IRTemporary>> temporaries;
};

#endif