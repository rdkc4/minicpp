#include "../ASTAssignSt.hpp"

ASTAssignSt::ASTAssignSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

void ASTAssignSt::setVariable(std::unique_ptr<ASTId> var){
    variable = std::move(var);
}

void ASTAssignSt::setExp(std::unique_ptr<ASTExpression> nexp){
    exp = std::move(nexp);
}