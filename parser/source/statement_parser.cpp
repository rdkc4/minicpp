#include "../statement_parser.hpp"
#include "../token_consumer.hpp"

#include <format>

StatementParser::StatementParser(TokenConsumer& consumer) 
    : exprParser{ consumer }, 
      tokenConsumer{ consumer } {}

std::unique_ptr<syntax::ast::ASTStmt> StatementParser::parseStmt(){
    const auto& token{ tokenConsumer.getToken() };
    if(token.gtype == syntax::GeneralTokenType::TYPE){
        return parseVariableDeclStmt();
    }

    switch(token.type){
        case syntax::TokenType::RETURN:
            return parseReturnStmt();

        case syntax::TokenType::IF:
            return parseIfStmt();

        case syntax::TokenType::LBRACE:
            return parseCompoundStmt();

        case syntax::TokenType::WHILE:
            return parseWhileStmt();

        case syntax::TokenType::FOR:
            return parseForStmt();

        case syntax::TokenType::DO:
            return parseDoWhileStmt();

        case syntax::TokenType::SWITCH:
            return parseSwitchStmt();

        case syntax::TokenType::ID: {
            auto nextType{ tokenConsumer.peek().type };
            if(nextType == syntax::TokenType::LPAREN){
                return parseFunctionCallStmt();
            }
            else if(nextType == syntax::TokenType::ASSIGN){
                return parseAssignStmt();
            }
        }
        default:
            break;
    }
    
    throw std::runtime_error(
        std::format("Line {}, Column {}: SYNTAX ERROR -> near '{}'",
            token.line, token.column, token.value
        )
    );
}

std::unique_ptr<syntax::ast::ASTVariableDeclStmt> StatementParser::parseVariableDeclStmt(){
    Type type{ tokenTypeToType(tokenConsumer.getToken().type) };
    tokenConsumer.consume(syntax::GeneralTokenType::TYPE);

    std::unique_ptr<syntax::ast::ASTVariableDeclStmt> variableDecl{ 
        std::make_unique<syntax::ast::ASTVariableDeclStmt>(tokenConsumer.getToken(), type)
    };
    tokenConsumer.consume(syntax::TokenType::ID);
    
    if(tokenConsumer.getToken().type == syntax::TokenType::ASSIGN){
        tokenConsumer.consume(syntax::TokenType::ASSIGN);
        variableDecl->setAssignExpr(exprParser.parseExpr());
    }
    tokenConsumer.consume(syntax::TokenType::SEMICOLON);

    return variableDecl;
}

std::unique_ptr<syntax::ast::ASTCompoundStmt> StatementParser::parseCompoundStmt(){
    std::unique_ptr<syntax::ast::ASTCompoundStmt> compoundStmt{ 
        std::make_unique<syntax::ast::ASTCompoundStmt>(syntax::Token{
            "compound_stmt", 
            tokenConsumer.getToken().line, 
            tokenConsumer.getToken().column
        })
    };

    tokenConsumer.consume(syntax::TokenType::LBRACE);
    while(tokenConsumer.getToken().type != syntax::TokenType::RBRACE){
        compoundStmt->addStmt(parseStmt());
    }
    tokenConsumer.consume(syntax::TokenType::RBRACE);
    
    return compoundStmt;
}

std::unique_ptr<syntax::ast::ASTAssignStmt> StatementParser::parseAssignStmt(bool expectsSemicolon){
    std::unique_ptr<syntax::ast::ASTIdExpr> variableExpr{ exprParser.parseIdExpr() };
    
    std::unique_ptr<syntax::ast::ASTAssignStmt> assignStmt{ 
        std::make_unique<syntax::ast::ASTAssignStmt>(syntax::Token{
            "=", 
            tokenConsumer.getToken().line, 
            tokenConsumer.getToken().column
        })
    };
    tokenConsumer.consume(syntax::TokenType::ASSIGN);
    
    assignStmt->setVariableIdExpr(std::move(variableExpr));
    assignStmt->setAssignedExpr(exprParser.parseExpr());

    if(expectsSemicolon){
        tokenConsumer.consume(syntax::TokenType::SEMICOLON);
    }
    return assignStmt;
}

std::unique_ptr<syntax::ast::ASTReturnStmt> StatementParser::parseReturnStmt(){
    const auto& returnToken{ tokenConsumer.getToken() };
    std::unique_ptr<syntax::ast::ASTReturnStmt> returnStmt{ 
        std::make_unique<syntax::ast::ASTReturnStmt>(syntax::Token{
            "return_stmt", 
            returnToken.line, 
            returnToken.column
        })
    };
    tokenConsumer.consume(syntax::TokenType::RETURN);

    if(tokenConsumer.getToken().type != syntax::TokenType::SEMICOLON){
        returnStmt->setReturnExpr(exprParser.parseExpr());
    }
    tokenConsumer.consume(syntax::TokenType::SEMICOLON);

    return returnStmt;
}

std::unique_ptr<syntax::ast::ASTIfStmt> StatementParser::parseIfStmt(){
    const auto& ifToken{ tokenConsumer.getToken() };
    std::unique_ptr<syntax::ast::ASTIfStmt> ifStmt{ 
        std::make_unique<syntax::ast::ASTIfStmt>(syntax::Token{
            "if_stmt", 
            ifToken.line, 
            ifToken.column
        })
    };
    tokenConsumer.consume(syntax::TokenType::IF);

    tokenConsumer.consume(syntax::TokenType::LPAREN);
    auto condition{ exprParser.parseExpr() };
    tokenConsumer.consume(syntax::TokenType::RPAREN);
    
    ifStmt->addIfStmt(std::move(condition), parseStmt());

    while(tokenConsumer.getToken().type == syntax::TokenType::ELSE && tokenConsumer.peek().type == syntax::TokenType::IF){
        tokenConsumer.consume(syntax::TokenType::ELSE);
        tokenConsumer.consume(syntax::TokenType::IF);

        tokenConsumer.consume(syntax::TokenType::LPAREN);
        condition = exprParser.parseExpr();
        tokenConsumer.consume(syntax::TokenType::RPAREN);
        
        ifStmt->addIfStmt(std::move(condition), parseStmt());
    }

    if(tokenConsumer.getToken().type == syntax::TokenType::ELSE){
        tokenConsumer.consume(syntax::TokenType::ELSE);
        ifStmt->addElseStmt(parseStmt());
    }
    
    return ifStmt;
}

std::unique_ptr<syntax::ast::ASTWhileStmt> StatementParser::parseWhileStmt(){
    const auto& whileToken{ tokenConsumer.getToken() };
    std::unique_ptr<syntax::ast::ASTWhileStmt> whileStmt{ 
        std::make_unique<syntax::ast::ASTWhileStmt>(syntax::Token{
            "while_stmt", 
            whileToken.line, 
            whileToken.column
        })
    };
    tokenConsumer.consume(syntax::TokenType::WHILE);
    
    tokenConsumer.consume(syntax::TokenType::LPAREN);
    auto condition{ exprParser.parseExpr() };
    tokenConsumer.consume(syntax::TokenType::RPAREN);
    
    whileStmt->setWhileStmt(std::move(condition), parseStmt());

    return whileStmt;
}

std::unique_ptr<syntax::ast::ASTForStmt> StatementParser::parseForStmt(){
    const auto& forToken{ tokenConsumer.getToken() };
    std::unique_ptr<syntax::ast::ASTForStmt> forStmt{ 
        std::make_unique<syntax::ast::ASTForStmt>(syntax::Token{
            "for_stmt", 
            forToken.line, 
            forToken.column
        })
    };
    tokenConsumer.consume(syntax::TokenType::FOR);

    std::unique_ptr<syntax::ast::ASTAssignStmt> forInitializer{ nullptr }, forIncrementer{ nullptr };
    std::unique_ptr<syntax::ast::ASTExpr> condition{ nullptr };

    tokenConsumer.consume(syntax::TokenType::LPAREN);
    // optional initializer
    if(tokenConsumer.getToken().type != syntax::TokenType::SEMICOLON){
        forInitializer = parseAssignStmt();
    }

    // optional condition (default: true)
    if(tokenConsumer.getToken().type != syntax::TokenType::SEMICOLON){
        condition = exprParser.parseExpr();
    }
    tokenConsumer.consume(syntax::TokenType::SEMICOLON);

    // optional incrementer
    if(tokenConsumer.getToken().type != syntax::TokenType::RPAREN){
        forIncrementer = parseAssignStmt(false); // false - doesn't expect semicolon at the end of the statement
    }
    tokenConsumer.consume(syntax::TokenType::RPAREN);

    forStmt->setForStmt(
        std::move(forInitializer), 
        std::move(condition), 
        std::move(forIncrementer), 
        parseStmt()
    );
    
    return forStmt;
}

std::unique_ptr<syntax::ast::ASTDoWhileStmt> StatementParser::parseDoWhileStmt(){
    const auto& dowhileToken{ tokenConsumer.getToken() };
    std::unique_ptr<syntax::ast::ASTDoWhileStmt> dowhileStmt{
        std::make_unique<syntax::ast::ASTDoWhileStmt>(syntax::Token{
            "dowhile_statement", 
            dowhileToken.line, 
            dowhileToken.column
        })
    };
    tokenConsumer.consume(syntax::TokenType::DO);

    std::unique_ptr<syntax::ast::ASTStmt> stmt{ parseStmt() };

    tokenConsumer.consume(syntax::TokenType::WHILE);
    tokenConsumer.consume(syntax::TokenType::LPAREN);
    dowhileStmt->setDoWhile(exprParser.parseExpr(), std::move(stmt));
    tokenConsumer.consume(syntax::TokenType::RPAREN);

    tokenConsumer.consume(syntax::TokenType::SEMICOLON);
    return dowhileStmt;
}

std::unique_ptr<syntax::ast::ASTFunctionCallStmt> StatementParser::parseFunctionCallStmt(){
    const auto& callToken{ tokenConsumer.getToken() };
    std::unique_ptr<syntax::ast::ASTFunctionCallStmt> callStmt{ 
        std::make_unique<syntax::ast::ASTFunctionCallStmt>(syntax::Token{
            "call_stmt", 
            callToken.line, 
            callToken.column
        })
    };
    callStmt->setFunctionCallStmt(exprParser.parseFunctionCallExpr());
    tokenConsumer.consume(syntax::TokenType::SEMICOLON);

    return callStmt;
}

std::unique_ptr<syntax::ast::ASTSwitchStmt> StatementParser::parseSwitchStmt(){
    const auto& switchToken{ tokenConsumer.getToken() };
    std::unique_ptr<syntax::ast::ASTSwitchStmt> switchStmt{ 
        std::make_unique<syntax::ast::ASTSwitchStmt>(syntax::Token{
            "switch_stmt", 
            switchToken.line, 
            switchToken.column
        })
    };
    tokenConsumer.consume(syntax::TokenType::SWITCH);

    tokenConsumer.consume(syntax::TokenType::LPAREN);
    switchStmt->setVariableIdExpr(exprParser.parseIdExpr());
    tokenConsumer.consume(syntax::TokenType::RPAREN);

    tokenConsumer.consume(syntax::TokenType::LBRACE);
    
    // switch must have at least 1 case
    do{
        switchStmt->addCaseStmt(parseCaseStmt());
    }while(tokenConsumer.getToken().type == syntax::TokenType::CASE);
    
    if(tokenConsumer.getToken().type == syntax::TokenType::DEFAULT){
        switchStmt->setDefaultStmt(parseDefaultStmt());
    }

    tokenConsumer.consume(syntax::TokenType::RBRACE);
    return switchStmt;
}

std::unique_ptr<syntax::ast::ASTSwitchBlockStmt> StatementParser::parseSwitchBlockStmt(){
    const auto& swBlockToken{ tokenConsumer.getToken() };
    std::unique_ptr<syntax::ast::ASTSwitchBlockStmt> switchBlockStmt{ 
        std::make_unique<syntax::ast::ASTSwitchBlockStmt>(syntax::Token{
            "switch_block_stmt", 
            swBlockToken.line, 
            swBlockToken.column
        })
    };
    
    while(true){
        const auto& current{ tokenConsumer.getToken() };
        if(current.type == syntax::TokenType::CASE ||
            current.type == syntax::TokenType::DEFAULT ||
            current.type == syntax::TokenType::RBRACE ||
            current.type == syntax::TokenType::BREAK
        ){
            break;
        }

        switchBlockStmt->addStmt(parseStmt());
    }

    return switchBlockStmt;
}

std::unique_ptr<syntax::ast::ASTCaseStmt> StatementParser::parseCaseStmt(){
    bool hasBreak{ false };
    const auto& caseToken{ tokenConsumer.getToken() };
    std::unique_ptr<syntax::ast::ASTCaseStmt> caseStmt{ 
        std::make_unique<syntax::ast::ASTCaseStmt>(syntax::Token{
            "case", 
            caseToken.line, 
            caseToken.column
        })
    };
    
    tokenConsumer.consume(syntax::TokenType::CASE);
    std::unique_ptr<syntax::ast::ASTLiteralExpr> literalExpr{ exprParser.parseLiteralExpr() };
    tokenConsumer.consume(syntax::TokenType::COLON);
    
    std::unique_ptr<syntax::ast::ASTSwitchBlockStmt> switchBlockStmt{ parseSwitchBlockStmt() };
    if(tokenConsumer.getToken().type == syntax::TokenType::BREAK){
        hasBreak = true;
        parseBreakStmt();
    }

    caseStmt->setCase(std::move(literalExpr), std::move(switchBlockStmt), hasBreak);

    return caseStmt;
}

std::unique_ptr<syntax::ast::ASTDefaultStmt> StatementParser::parseDefaultStmt(){
    const auto& defaultToken{ tokenConsumer.getToken() };
    std::unique_ptr<syntax::ast::ASTDefaultStmt> defaultStmt{ 
        std::make_unique<syntax::ast::ASTDefaultStmt>(syntax::Token{
            "default", 
            defaultToken.line, 
            defaultToken.column
        })
    };
    tokenConsumer.consume(syntax::TokenType::DEFAULT);
    tokenConsumer.consume(syntax::TokenType::COLON);
    
    defaultStmt->setDefaultStmt(parseSwitchBlockStmt());
    if(tokenConsumer.getToken().type == syntax::TokenType::BREAK){
        parseBreakStmt();
    }

    return defaultStmt;
}

void StatementParser::parseBreakStmt(){
    // only for switch statement at the moment, located at the end of the case
    tokenConsumer.consume(syntax::TokenType::BREAK);
    tokenConsumer.consume(syntax::TokenType::SEMICOLON);
}