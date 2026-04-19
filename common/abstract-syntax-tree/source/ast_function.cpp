#include "../ast_function.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTFunction::ASTFunction(const syntax::Token& token, types::Type type) 
    : ASTNode(token, syntax::ast::ASTNodeType::FUNCTION), 
      type{ type }, 
      predefined{ false }, 
      alwaysReturns{ false } {}

const std::vector<std::unique_ptr<syntax::ast::ASTParameter>>& 
syntax::ast::ASTFunction::getParameters() const noexcept {
    return parameters;
}

const std::vector<std::unique_ptr<syntax::ast::ASTStmt>>& 
syntax::ast::ASTFunction::getBody() const noexcept {
    return body;
}

void syntax::ast::ASTFunction::addParameter(std::unique_ptr<syntax::ast::ASTParameter> parameter){
    parameters.push_back(std::move(parameter));
}

void syntax::ast::ASTFunction::addStatement(std::unique_ptr<syntax::ast::ASTStmt> statement){
    body.push_back(std::move(statement));
}

void syntax::ast::ASTFunction::setPredefined(bool isPredefined) noexcept {
    predefined = isPredefined;
}

types::Type syntax::ast::ASTFunction::getType() const noexcept {
    return type;
}

bool syntax::ast::ASTFunction::isPredefined() const noexcept {
    return predefined;
}

size_t syntax::ast::ASTFunction::getParameterCount() const noexcept {
    return parameters.size();
}

void syntax::ast::ASTFunction::setAlwaysReturns(bool returns) noexcept {
    alwaysReturns = returns;
}

bool syntax::ast::ASTFunction::alwaysReturnsValue() const noexcept {
    return alwaysReturns;
}

void syntax::ast::ASTFunction::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}