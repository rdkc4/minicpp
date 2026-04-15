#include "../ast_dir.hpp"

AST::node::ASTDir::ASTDir(const Token token, AST::defs::ASTNodeType ntype) 
    : ASTNode(token, ntype) {}