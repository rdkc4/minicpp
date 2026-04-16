#include "../ast_function.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTFunction::ASTFunction(const Token& token, Type type) 
    : ASTNode(token, AST::defs::ASTNodeType::FUNCTION), 
      type{ type }, 
      predefined{ false }, 
      alwaysReturns{ false } {}

const std::vector<std::unique_ptr<AST::node::ASTParameter>>& 
AST::node::ASTFunction::getParameters() const noexcept {
    return parameters;
}

const std::vector<std::unique_ptr<AST::node::ASTStmt>>& 
AST::node::ASTFunction::getBody() const noexcept {
    return body;
}

void AST::node::ASTFunction::addParameter(std::unique_ptr<AST::node::ASTParameter> parameter){
    parameters.push_back(std::move(parameter));
}

void AST::node::ASTFunction::addStatement(std::unique_ptr<AST::node::ASTStmt> statement){
    body.push_back(std::move(statement));
}

void AST::node::ASTFunction::setPredefined(bool isPredefined) noexcept {
    predefined = isPredefined;
}

Type AST::node::ASTFunction::getType() const noexcept {
    return type;
}

bool AST::node::ASTFunction::isPredefined() const noexcept {
    return predefined;
}

size_t AST::node::ASTFunction::getParameterCount() const noexcept {
    return parameters.size();
}

void AST::node::ASTFunction::setAlwaysReturns(bool returns) noexcept {
    alwaysReturns = returns;
}

bool AST::node::ASTFunction::alwaysReturnsValue() const noexcept {
    return alwaysReturns;
}

void AST::node::ASTFunction::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}