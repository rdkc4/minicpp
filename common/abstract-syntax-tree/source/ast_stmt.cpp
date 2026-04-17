#include "../ast_stmt.hpp"

syntax::ast::ASTStmt::ASTStmt(const syntax::Token& token, syntax::ast::ASTNodeType ntype) 
    : ASTNode(token, ntype) {}