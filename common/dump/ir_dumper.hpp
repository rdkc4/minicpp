#ifndef IR_DUMPER_HPP
#define IR_DUMPER_HPP

#include <ostream>
#include <string_view>

#include "../visitor/ir_visitor.hpp"
#include "../intermediate-representation-tree/ir_node.hpp"
#include "../intermediate-representation-tree/ir_program.hpp"
#include "../intermediate-representation-tree/ir_function.hpp"
#include "../intermediate-representation-tree/ir_parameter.hpp"
#include "../intermediate-representation-tree/ir_variable_decl_stmt.hpp"
#include "../intermediate-representation-tree/ir_assign_stmt.hpp"
#include "../intermediate-representation-tree/ir_compound_stmt.hpp"
#include "../intermediate-representation-tree/ir_for_stmt.hpp"
#include "../intermediate-representation-tree/ir_function_call_stmt.hpp"
#include "../intermediate-representation-tree/ir_if_stmt.hpp"
#include "../intermediate-representation-tree/ir_while_stmt.hpp"
#include "../intermediate-representation-tree/ir_dowhile_stmt.hpp"
#include "../intermediate-representation-tree/ir_return_stmt.hpp"
#include "../intermediate-representation-tree/ir_switch_stmt.hpp"
#include "../intermediate-representation-tree/ir_case_stmt.hpp"
#include "../intermediate-representation-tree/ir_default_stmt.hpp"
#include "../intermediate-representation-tree/ir_switch_block_stmt.hpp"
#include "../intermediate-representation-tree/ir_binary_expr.hpp"
#include "../intermediate-representation-tree/ir_function_call_expr.hpp"
#include "../intermediate-representation-tree/ir_id_expr.hpp"
#include "../intermediate-representation-tree/ir_literal_expr.hpp"
#include "../intermediate-representation-tree/ir_temporary_expr.hpp"

class IRDumper final : public IRVisitor {
public:
    IRDumper(std::ostream& out);

    ~IRDumper() = default;

    void visit(IRProgram* program) override;

    void visit(IRFunction* function) override;

    void visit(IRParameter* parameter) override;

    void visit(IRVariableDeclStmt* variableDecl) override;

    void visit(IRAssignStmt* assignStmt) override;

    void visit(IRCompoundStmt* compoundStmt) override;

    void visit(IRForStmt* forStmt) override;

    void visit(IRFunctionCallStmt* callStmt) override;

    void visit(IRIfStmt* ifStmt) override;

    void visit(IRReturnStmt* returnStmt) override;

    void visit(IRWhileStmt* whileStmt) override;

    void visit(IRDoWhileStmt* dowhileStmt) override;

    void visit(IRSwitchStmt* switchStmt) override;

    void visit(IRCaseStmt* caseStmt) override;

    void visit(IRDefaultStmt* defaultStmt) override;

    void visit(IRSwitchBlockStmt* switchBlockStmt) override;

    void visit(IRBinaryExpr* binaryExpr) override;

    void visit(IRFunctionCallExpr* callExpr) override;

    void visit(IRIdExpr* idExpr) override;

    void visit(IRLiteralExpr* literalExpr) override;
    
    void visit(IRTemporaryExpr* tempExpr) override;

private:
    std::ostream& out;

    int indent;

    void dumpIndent();

    void dumpNode(const IRNode* node, std::string_view details = "");

    void dumpNode(std::string_view nodeLabel);

};

#endif