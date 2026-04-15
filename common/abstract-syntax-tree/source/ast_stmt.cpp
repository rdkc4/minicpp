#include "../ast_stmt.hpp"

AST::node::ASTStmt::ASTStmt(const Token& token, AST::defs::ASTNodeType ntype) 
    : ASTNode(token, ntype) {}