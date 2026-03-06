#include "../ir_compound_stmt.hpp"

IRCompoundStmt::IRCompoundStmt(IRNodeType ntype) : IRStmt(ntype) {}

const std::vector<std::unique_ptr<IRStmt>>& IRCompoundStmt::getStatements() const noexcept {
    return statements;
}

void IRCompoundStmt::addStatement(std::unique_ptr<IRStmt> statement){
    statements.push_back(std::move(statement));
}

void IRCompoundStmt::eliminateDead(size_t startIdx){
    if(startIdx < statements.size()){
        statements.erase(statements.begin() + startIdx, statements.end());
    }
}