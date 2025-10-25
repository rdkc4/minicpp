#ifndef STATEMENT_PARSER_HPP
#define STATEMENT_PARSER_HPP

#include <memory>

#include "../common/abstract-syntax-tree/ASTStatement.hpp"
#include "../common/abstract-syntax-tree/ASTVariable.hpp"
#include "../common/abstract-syntax-tree/ASTPrintfSt.hpp"
#include "../common/abstract-syntax-tree/ASTCompoundSt.hpp"
#include "../common/abstract-syntax-tree/ASTAssignSt.hpp"
#include "../common/abstract-syntax-tree/ASTReturnSt.hpp"
#include "../common/abstract-syntax-tree/ASTIfSt.hpp"
#include "../common/abstract-syntax-tree/ASTWhileSt.hpp"
#include "../common/abstract-syntax-tree/ASTDoWhileSt.hpp"
#include "../common/abstract-syntax-tree/ASTForSt.hpp"
#include "../common/abstract-syntax-tree/ASTSwitchSt.hpp"

#include "expression_parser.hpp"
#include "token_consumer.hpp"

class StatementParser {
public:
    StatementParser(TokenConsumer& consumer);

    std::unique_ptr<ASTStatement> statement();

protected:
    ExpressionParser expParser;
    TokenConsumer& tokenConsumer;

    // statements
    std::unique_ptr<ASTVariable> variable();
    std::unique_ptr<ASTPrintfSt> printfStatement();
    std::unique_ptr<ASTCompoundSt> compoundStatement();
    std::unique_ptr<ASTAssignSt> assignmentStatement(bool expectsSemicolon = true);
    std::unique_ptr<ASTReturnSt> returnStatement();
    std::unique_ptr<ASTIfSt> ifStatement();
    std::unique_ptr<ASTWhileSt> whileStatement();
    std::unique_ptr<ASTForSt> forStatement();
    std::unique_ptr<ASTDoWhileSt> doWhileStatement();
    std::unique_ptr<ASTSwitchSt> switchStatement();
    std::unique_ptr<ASTSwitchBlock> switchCaseBlock();
    std::unique_ptr<ASTCaseSt> _case();
    std::unique_ptr<ASTDefaultSt> _default();
    void _break();

};

#endif