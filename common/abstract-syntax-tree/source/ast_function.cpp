#include "../ast_function.hpp"

#include "../defs/ast_defs.hpp"

ASTFunction::ASTFunction(const Token& token, Type type) 
    : ASTNode(token, ASTNodeType::FUNCTION), type{ type }, predefined{ false }, alwaysReturns{ false } {}

const std::vector<std::unique_ptr<ASTParameter>>& ASTFunction::getParameters() const noexcept {
    return parameters;
}

const std::vector<std::unique_ptr<ASTStmt>>& ASTFunction::getBody() const noexcept {
    return body;
}

void ASTFunction::addParameter(std::unique_ptr<ASTParameter> parameter){
    parameters.push_back(std::move(parameter));
}

void ASTFunction::addStatement(std::unique_ptr<ASTStmt> statement){
    body.push_back(std::move(statement));
}

void ASTFunction::setPredefined(bool isPredefined) noexcept {
    predefined = isPredefined;
}

Type ASTFunction::getType() const noexcept {
    return type;
}

bool ASTFunction::isPredefined() const noexcept {
    return predefined;
}

size_t ASTFunction::getParameterCount() const noexcept {
    return parameters.size();
}

void ASTFunction::setAlwaysReturns(bool returns) noexcept {
    alwaysReturns = returns;
}

bool ASTFunction::alwaysReturnsValue() const noexcept {
    return alwaysReturns;
}

void ASTFunction::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}