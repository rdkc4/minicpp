#ifndef AST_ID_EXPR_HPP
#define AST_ID_EXPR_HPP

#include "ast_expr.hpp"
#include "defs/ast_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"
#include "../visitor/ast_visitor.hpp"

/** 
 * @class ASTIdExpr
 * @brief AST node representing id
*/
class ASTIdExpr final : public ASTExpr {
public:
    /** 
     * @brief Creates the instance of the ast id
     * @param token - const reference to the token
     * @param ntype - type of the ast node
     * @param type - type of the id
    */
    ASTIdExpr(const Token token, ASTNodeType ntype, Type type = Type::NO_TYPE);

    void accept(ASTVisitor& visitor) override;

};


#endif