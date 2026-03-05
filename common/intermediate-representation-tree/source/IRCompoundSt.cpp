#include "../IRCompoundSt.hpp"

IRCompoundSt::IRCompoundSt(IRNodeType ntype) : IRStatement(ntype) {}

const std::vector<std::unique_ptr<IRStatement>>& IRCompoundSt::getStatements() const noexcept {
    return statements;
}

void IRCompoundSt::addStatement(std::unique_ptr<IRStatement> statement){
    statements.push_back(std::move(statement));
}

void IRCompoundSt::eliminateDead(size_t startIdx){
    if(startIdx < statements.size()){
        statements.erase(statements.begin() + startIdx, statements.end());
    }
}