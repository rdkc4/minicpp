#include "../statement_parser.hpp"
#include "../token_consumer.hpp"

#include <format>

StatementParser::StatementParser(TokenConsumer& consumer) 
    : exprParser{ consumer }, 
      tokenConsumer{ consumer } {}

std::unique_ptr<AST::node::ASTStmt> StatementParser::parseStmt(){
    const auto& token{ tokenConsumer.getToken() };
    if(token.gtype == GeneralTokenType::TYPE){
        return parseVariableDeclStmt();
    }

    switch(token.type){
        case TokenType::RETURN:
            return parseReturnStmt();

        case TokenType::IF:
            return parseIfStmt();

        case TokenType::LBRACE:
            return parseCompoundStmt();

        case TokenType::WHILE:
            return parseWhileStmt();

        case TokenType::FOR:
            return parseForStmt();

        case TokenType::DO:
            return parseDoWhileStmt();

        case TokenType::SWITCH:
            return parseSwitchStmt();

        case TokenType::ID: {
            auto nextType{ tokenConsumer.peek().type };
            if(nextType == TokenType::LPAREN){
                return parseFunctionCallStmt();
            }
            else if(nextType == TokenType::ASSIGN){
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

std::unique_ptr<AST::node::ASTVariableDeclStmt> StatementParser::parseVariableDeclStmt(){
    Type type{ tokenTypeToType(tokenConsumer.getToken().type) };
    tokenConsumer.consume(GeneralTokenType::TYPE);

    std::unique_ptr<AST::node::ASTVariableDeclStmt> variableDecl{ 
        std::make_unique<AST::node::ASTVariableDeclStmt>(tokenConsumer.getToken(), type)
    };
    tokenConsumer.consume(TokenType::ID);
    
    if(tokenConsumer.getToken().type == TokenType::ASSIGN){
        tokenConsumer.consume(TokenType::ASSIGN);
        variableDecl->setAssignExpr(exprParser.parseExpr());
    }
    tokenConsumer.consume(TokenType::SEMICOLON);

    return variableDecl;
}

std::unique_ptr<AST::node::ASTCompoundStmt> StatementParser::parseCompoundStmt(){
    std::unique_ptr<AST::node::ASTCompoundStmt> compoundStmt{ 
        std::make_unique<AST::node::ASTCompoundStmt>(Token{
            "compound_stmt", 
            tokenConsumer.getToken().line, 
            tokenConsumer.getToken().column
        })
    };

    tokenConsumer.consume(TokenType::LBRACE);
    while(tokenConsumer.getToken().type != TokenType::RBRACE){
        compoundStmt->addStmt(parseStmt());
    }
    tokenConsumer.consume(TokenType::RBRACE);
    
    return compoundStmt;
}

std::unique_ptr<AST::node::ASTAssignStmt> StatementParser::parseAssignStmt(bool expectsSemicolon){
    std::unique_ptr<AST::node::ASTIdExpr> variableExpr{ exprParser.parseIdExpr() };
    
    std::unique_ptr<AST::node::ASTAssignStmt> assignStmt{ 
        std::make_unique<AST::node::ASTAssignStmt>(Token{
            "=", 
            tokenConsumer.getToken().line, 
            tokenConsumer.getToken().column
        })
    };
    tokenConsumer.consume(TokenType::ASSIGN);
    
    assignStmt->setVariableIdExpr(std::move(variableExpr));
    assignStmt->setAssignedExpr(exprParser.parseExpr());

    if(expectsSemicolon){
        tokenConsumer.consume(TokenType::SEMICOLON);
    }
    return assignStmt;
}

std::unique_ptr<AST::node::ASTReturnStmt> StatementParser::parseReturnStmt(){
    const auto& returnToken{ tokenConsumer.getToken() };
    std::unique_ptr<AST::node::ASTReturnStmt> returnStmt{ 
        std::make_unique<AST::node::ASTReturnStmt>(Token{
            "return_stmt", 
            returnToken.line, 
            returnToken.column
        })
    };
    tokenConsumer.consume(TokenType::RETURN);

    if(tokenConsumer.getToken().type != TokenType::SEMICOLON){
        returnStmt->setReturnExpr(exprParser.parseExpr());
    }
    tokenConsumer.consume(TokenType::SEMICOLON);

    return returnStmt;
}

std::unique_ptr<AST::node::ASTIfStmt> StatementParser::parseIfStmt(){
    const auto& ifToken{ tokenConsumer.getToken() };
    std::unique_ptr<AST::node::ASTIfStmt> ifStmt{ 
        std::make_unique<AST::node::ASTIfStmt>(Token{
            "if_stmt", 
            ifToken.line, 
            ifToken.column
        })
    };
    tokenConsumer.consume(TokenType::IF);

    tokenConsumer.consume(TokenType::LPAREN);
    auto condition{ exprParser.parseExpr() };
    tokenConsumer.consume(TokenType::RPAREN);
    
    ifStmt->addIfStmt(std::move(condition), parseStmt());

    while(tokenConsumer.getToken().type == TokenType::ELSE && tokenConsumer.peek().type == TokenType::IF){
        tokenConsumer.consume(TokenType::ELSE);
        tokenConsumer.consume(TokenType::IF);

        tokenConsumer.consume(TokenType::LPAREN);
        condition = exprParser.parseExpr();
        tokenConsumer.consume(TokenType::RPAREN);
        
        ifStmt->addIfStmt(std::move(condition), parseStmt());
    }

    if(tokenConsumer.getToken().type == TokenType::ELSE){
        tokenConsumer.consume(TokenType::ELSE);
        ifStmt->addElseStmt(parseStmt());
    }
    
    return ifStmt;
}

std::unique_ptr<AST::node::ASTWhileStmt> StatementParser::parseWhileStmt(){
    const auto& whileToken{ tokenConsumer.getToken() };
    std::unique_ptr<AST::node::ASTWhileStmt> whileStmt{ 
        std::make_unique<AST::node::ASTWhileStmt>(Token{
            "while_stmt", 
            whileToken.line, 
            whileToken.column
        })
    };
    tokenConsumer.consume(TokenType::WHILE);
    
    tokenConsumer.consume(TokenType::LPAREN);
    auto condition{ exprParser.parseExpr() };
    tokenConsumer.consume(TokenType::RPAREN);
    
    whileStmt->setWhileStmt(std::move(condition), parseStmt());

    return whileStmt;
}

std::unique_ptr<AST::node::ASTForStmt> StatementParser::parseForStmt(){
    const auto& forToken{ tokenConsumer.getToken() };
    std::unique_ptr<AST::node::ASTForStmt> forStmt{ 
        std::make_unique<AST::node::ASTForStmt>(Token{
            "for_stmt", 
            forToken.line, 
            forToken.column
        })
    };
    tokenConsumer.consume(TokenType::FOR);

    std::unique_ptr<AST::node::ASTAssignStmt> forInitializer{ nullptr }, forIncrementer{ nullptr };
    std::unique_ptr<AST::node::ASTExpr> condition{ nullptr };

    tokenConsumer.consume(TokenType::LPAREN);
    // optional initializer
    if(tokenConsumer.getToken().type != TokenType::SEMICOLON){
        forInitializer = parseAssignStmt();
    }

    // optional condition (default: true)
    if(tokenConsumer.getToken().type != TokenType::SEMICOLON){
        condition = exprParser.parseExpr();
    }
    tokenConsumer.consume(TokenType::SEMICOLON);

    // optional incrementer
    if(tokenConsumer.getToken().type != TokenType::RPAREN){
        forIncrementer = parseAssignStmt(false); // false - doesn't expect semicolon at the end of the statement
    }
    tokenConsumer.consume(TokenType::RPAREN);

    forStmt->setForStmt(
        std::move(forInitializer), 
        std::move(condition), 
        std::move(forIncrementer), 
        parseStmt()
    );
    
    return forStmt;
}

std::unique_ptr<AST::node::ASTDoWhileStmt> StatementParser::parseDoWhileStmt(){
    const auto& dowhileToken{ tokenConsumer.getToken() };
    std::unique_ptr<AST::node::ASTDoWhileStmt> dowhileStmt{
        std::make_unique<AST::node::ASTDoWhileStmt>(Token{
            "dowhile_statement", 
            dowhileToken.line, 
            dowhileToken.column
        })
    };
    tokenConsumer.consume(TokenType::DO);

    std::unique_ptr<AST::node::ASTStmt> stmt{ parseStmt() };

    tokenConsumer.consume(TokenType::WHILE);
    tokenConsumer.consume(TokenType::LPAREN);
    dowhileStmt->setDoWhile(exprParser.parseExpr(), std::move(stmt));
    tokenConsumer.consume(TokenType::RPAREN);

    tokenConsumer.consume(TokenType::SEMICOLON);
    return dowhileStmt;
}

std::unique_ptr<AST::node::ASTFunctionCallStmt> StatementParser::parseFunctionCallStmt(){
    const auto& callToken{ tokenConsumer.getToken() };
    std::unique_ptr<AST::node::ASTFunctionCallStmt> callStmt{ 
        std::make_unique<AST::node::ASTFunctionCallStmt>(Token{
            "call_stmt", 
            callToken.line, 
            callToken.column
        })
    };
    callStmt->setFunctionCallStmt(exprParser.parseFunctionCallExpr());
    tokenConsumer.consume(TokenType::SEMICOLON);

    return callStmt;
}

std::unique_ptr<AST::node::ASTSwitchStmt> StatementParser::parseSwitchStmt(){
    const auto& switchToken{ tokenConsumer.getToken() };
    std::unique_ptr<AST::node::ASTSwitchStmt> switchStmt{ 
        std::make_unique<AST::node::ASTSwitchStmt>(Token{
            "switch_stmt", 
            switchToken.line, 
            switchToken.column
        })
    };
    tokenConsumer.consume(TokenType::SWITCH);

    tokenConsumer.consume(TokenType::LPAREN);
    switchStmt->setVariableIdExpr(exprParser.parseIdExpr());
    tokenConsumer.consume(TokenType::RPAREN);

    tokenConsumer.consume(TokenType::LBRACE);
    
    // switch must have at least 1 case
    do{
        switchStmt->addCaseStmt(parseCaseStmt());
    }while(tokenConsumer.getToken().type == TokenType::CASE);
    
    if(tokenConsumer.getToken().type == TokenType::DEFAULT){
        switchStmt->setDefaultStmt(parseDefaultStmt());
    }

    tokenConsumer.consume(TokenType::RBRACE);
    return switchStmt;
}

std::unique_ptr<AST::node::ASTSwitchBlockStmt> StatementParser::parseSwitchBlockStmt(){
    const auto& swBlockToken{ tokenConsumer.getToken() };
    std::unique_ptr<AST::node::ASTSwitchBlockStmt> switchBlockStmt{ 
        std::make_unique<AST::node::ASTSwitchBlockStmt>(Token{
            "switch_block_stmt", 
            swBlockToken.line, 
            swBlockToken.column
        })
    };
    
    while(true){
        const auto& current{ tokenConsumer.getToken() };
        if(current.type == TokenType::CASE ||
            current.type == TokenType::DEFAULT ||
            current.type == TokenType::RBRACE ||
            current.type == TokenType::BREAK
        ){
            break;
        }

        switchBlockStmt->addStmt(parseStmt());
    }

    return switchBlockStmt;
}

std::unique_ptr<AST::node::ASTCaseStmt> StatementParser::parseCaseStmt(){
    bool hasBreak{ false };
    const auto& caseToken{ tokenConsumer.getToken() };
    std::unique_ptr<AST::node::ASTCaseStmt> caseStmt{ 
        std::make_unique<AST::node::ASTCaseStmt>(Token{
            "case", 
            caseToken.line, 
            caseToken.column
        })
    };
    
    tokenConsumer.consume(TokenType::CASE);
    std::unique_ptr<AST::node::ASTLiteralExpr> literalExpr{ exprParser.parseLiteralExpr() };
    tokenConsumer.consume(TokenType::COLON);
    
    std::unique_ptr<AST::node::ASTSwitchBlockStmt> switchBlockStmt{ parseSwitchBlockStmt() };
    if(tokenConsumer.getToken().type == TokenType::BREAK){
        hasBreak = true;
        parseBreakStmt();
    }

    caseStmt->setCase(std::move(literalExpr), std::move(switchBlockStmt), hasBreak);

    return caseStmt;
}

std::unique_ptr<AST::node::ASTDefaultStmt> StatementParser::parseDefaultStmt(){
    const auto& defaultToken{ tokenConsumer.getToken() };
    std::unique_ptr<AST::node::ASTDefaultStmt> defaultStmt{ 
        std::make_unique<AST::node::ASTDefaultStmt>(Token{
            "default", 
            defaultToken.line, 
            defaultToken.column
        })
    };
    tokenConsumer.consume(TokenType::DEFAULT);
    tokenConsumer.consume(TokenType::COLON);
    
    defaultStmt->setDefaultStmt(parseSwitchBlockStmt());
    if(tokenConsumer.getToken().type == TokenType::BREAK){
        parseBreakStmt();
    }

    return defaultStmt;
}

void StatementParser::parseBreakStmt(){
    // only for switch statement at the moment, located at the end of the case
    tokenConsumer.consume(TokenType::BREAK);
    tokenConsumer.consume(TokenType::SEMICOLON);
}