#include "../ASTFunction.hpp"

ASTFunction::ASTFunction(const Token& token, ASTNodeType ntype, Types type) : ASTNode(token, ntype), type{ type }, predefined{ false } {}

const std::vector<std::unique_ptr<ASTParameter>>& ASTFunction::getParameters() const noexcept {
    return parameters;
}

const std::vector<std::unique_ptr<ASTStatement>>& ASTFunction::getBody() const noexcept {
    return body;
}

void ASTFunction::addParameter(std::unique_ptr<ASTParameter> parameter){
    parameters.push_back(std::move(parameter));
}

void ASTFunction::addStatement(std::unique_ptr<ASTStatement> statement){
    body.push_back(std::move(statement));
}

void ASTFunction::setPredefined(bool isPredefined) noexcept {
    predefined = isPredefined;
}

Types ASTFunction::getType() const noexcept {
    return type;
}

bool ASTFunction::isPredefined() const noexcept {
    return predefined;
}

size_t ASTFunction::getParameterCount() const noexcept {
    return parameters.size();
}