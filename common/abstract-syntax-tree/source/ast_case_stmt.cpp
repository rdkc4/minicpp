#include "../ast_case_stmt.hpp"

ASTCaseStmt::ASTCaseStmt(const Token& token, ASTNodeType ntype) : ASTStmt(token, ntype) {}

const ASTLiteralExpr* ASTCaseStmt::getLiteral() const noexcept {
    return literal.get();
}

const ASTSwitchBlockStmt* ASTCaseStmt::getSwitchBlock() const noexcept {
    return swBlock.get();
}

void ASTCaseStmt::setCase(std::unique_ptr<ASTLiteralExpr> lit, std::unique_ptr<ASTSwitchBlockStmt> _swBlock, bool hasBr){
    literal = std::move(lit);
    swBlock = std::move(_swBlock);
    _break = hasBr;
}

bool ASTCaseStmt::hasBreak() const noexcept {
    return _break;
}

void ASTCaseStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}