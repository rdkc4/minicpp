#include "../ast_dir.hpp"

syntax::ast::ASTDir::ASTDir(const syntax::Token& token, syntax::ast::ASTNodeType ntype) 
    : ASTNode(token, ntype) {}