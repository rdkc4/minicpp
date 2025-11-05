#ifndef STATEMENT_CODE_GENERATOR_HPP
#define STATEMENT_CODE_GENERATOR_HPP

#include "../../common/intermediate-representation-tree/IRStatement.hpp"
#include "../../common/intermediate-representation-tree/IRVariable.hpp"
#include "../../common/intermediate-representation-tree/IRPrintfSt.hpp"
#include "../../common/intermediate-representation-tree/IRIfSt.hpp"
#include "../../common/intermediate-representation-tree/IRWhileSt.hpp"
#include "../../common/intermediate-representation-tree/IRForSt.hpp"
#include "../../common/intermediate-representation-tree/IRDoWhileSt.hpp"
#include "../../common/intermediate-representation-tree/IRCompoundSt.hpp"
#include "../../common/intermediate-representation-tree/IRAssignSt.hpp"
#include "../../common/intermediate-representation-tree/IRReturnSt.hpp"
#include "../../common/intermediate-representation-tree/IRSwitchSt.hpp"
#include "expression_code_generator.hpp"

class StatementCodeGenerator {
public:
    void generateStatement(const IRStatement* _statement);

private:
    ExpressionCodeGenerator exprGenerator;

    // statements
    void generateVariable(const IRVariable* _variable);
    void generatePrintfStatement(const IRPrintfSt* _printf);
    void generateIfStatement(const IRIfSt* _if);
    void generateWhileStatement(const IRWhileSt* _while);
    void generateForStatement(const IRForSt* _for);
    void generateDoWhileStatement(const IRDoWhileSt* _dowhile);
    void generateCompoundStatement(const IRCompoundSt* _compound);
    void generateAssignmentStatement(const IRAssignSt* _assignment);
    void generateReturnStatement(const IRReturnSt* _return);
    void generateSwitchStatement(const IRSwitchSt* _switch);
};

#endif