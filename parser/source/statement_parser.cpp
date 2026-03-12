#include "../statement_parser.hpp"
#include "../token_consumer.hpp"

#include <format>

StatementParser::StatementParser(TokenConsumer& consumer) : exprParser{ consumer }, tokenConsumer{ consumer } {}

std::unique_ptr<ASTStmt> StatementParser::parseStmt(){
    if(tokenConsumer.getToken().gtype == GeneralTokenType::TYPE){
        return parseVariableDeclStmt();
    }
    else if(tokenConsumer.peek().type == TokenType::_ASSIGN){
        return parseAssignStmt();
    }

    auto token = tokenConsumer.getToken();
    if(token.type == TokenType::_RETURN)
        return parseReturnStmt();
    else if(token.type == TokenType::_IF)
        return parseIfStmt();
    else if(token.type == TokenType::_LBRACKET)
        return parseCompoundStmt();
    else if(token.type == TokenType::_WHILE)
        return parseWhileStmt();
    else if(token.type == TokenType::_FOR)
        return parseForStmt();
    else if(token.type == TokenType::_DO)
        return parseDoWhileStmt();
    else if(token.type == TokenType::_SWITCH)
        return parseSwitchStmt();
    else if(token.type == TokenType::_ID && tokenConsumer.peek().type == TokenType::_LPAREN)
        return parseFunctionCallStmt();
    
    throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> near '{}'",
        token.line, token.column, token.value));
}

std::unique_ptr<ASTVariableDeclStmt> StatementParser::parseVariableDeclStmt(){
    Type type{ tokenTypeToType.find(tokenConsumer.getToken().type) != tokenTypeToType.end() ? tokenTypeToType.at(tokenConsumer.getToken().type) : Type::NO_TYPE };
    tokenConsumer.consume(GeneralTokenType::TYPE);
    std::unique_ptr<ASTVariableDeclStmt> variableDecl = std::make_unique<ASTVariableDeclStmt>(tokenConsumer.getToken(), type);
    tokenConsumer.consume(TokenType::_ID);
    
    if(tokenConsumer.getToken().type == TokenType::_ASSIGN){
        tokenConsumer.consume(TokenType::_ASSIGN);
        variableDecl->setAssignExpr(exprParser.parseNumericalExpr());
    }
    tokenConsumer.consume(TokenType::_SEMICOLON);

    return variableDecl;
}

std::unique_ptr<ASTCompoundStmt> StatementParser::parseCompoundStmt(){
    std::unique_ptr<ASTCompoundStmt> compoundStmt = 
        std::make_unique<ASTCompoundStmt>(Token{"compound_statement", tokenConsumer.getToken().line, tokenConsumer.getToken().column});

    tokenConsumer.consume(TokenType::_LBRACKET);
    while(tokenConsumer.getToken().type != TokenType::_RBRACKET){
        compoundStmt->addStmt(parseStmt());
    }
    tokenConsumer.consume(TokenType::_RBRACKET);
    
    return compoundStmt;
}

std::unique_ptr<ASTAssignStmt> StatementParser::parseAssignStmt(bool expectsSemicolon){
    std::unique_ptr<ASTIdExpr> variableExpr{ exprParser.parseIdExpr() };
    
    std::unique_ptr<ASTAssignStmt> assignStmt = 
        std::make_unique<ASTAssignStmt>(Token{"=", tokenConsumer.getToken().line, tokenConsumer.getToken().column});
    tokenConsumer.consume(TokenType::_ASSIGN);
    
    assignStmt->setVariableIdExpr(std::move(variableExpr));
    assignStmt->setAssignedExpr(exprParser.parseNumericalExpr());

    if(expectsSemicolon){
        tokenConsumer.consume(TokenType::_SEMICOLON);
    }
    return assignStmt;
}

std::unique_ptr<ASTReturnStmt> StatementParser::parseReturnStmt(){
    std::unique_ptr<ASTReturnStmt> returnStmt = 
        std::make_unique<ASTReturnStmt>(Token{"return_statement", tokenConsumer.getToken().line, tokenConsumer.getToken().column});
    tokenConsumer.consume(TokenType::_RETURN);

    if(tokenConsumer.getToken().type != TokenType::_SEMICOLON){
        returnStmt->setReturnExpr(exprParser.parseNumericalExpr());
    }
    tokenConsumer.consume(TokenType::_SEMICOLON);

    return returnStmt;
}

std::unique_ptr<ASTIfStmt> StatementParser::parseIfStmt(){
    std::unique_ptr<ASTIfStmt> ifStmt = 
        std::make_unique<ASTIfStmt>(Token{"if_statement", tokenConsumer.getToken().line, tokenConsumer.getToken().column});
    tokenConsumer.consume(TokenType::_IF);

    tokenConsumer.consume(TokenType::_LPAREN);
    auto condition{ exprParser.parseRelationalExpr() };
    tokenConsumer.consume(TokenType::_RPAREN);
    
    ifStmt->addIfStmt(std::move(condition), parseStmt());

    while(tokenConsumer.getToken().type == TokenType::_ELSE && tokenConsumer.peek().type == TokenType::_IF){
        tokenConsumer.consume(TokenType::_ELSE);
        tokenConsumer.consume(TokenType::_IF);

        tokenConsumer.consume(TokenType::_LPAREN);
        condition = exprParser.parseRelationalExpr();
        tokenConsumer.consume(TokenType::_RPAREN);
        
        ifStmt->addIfStmt(std::move(condition), parseStmt());
    }

    if(tokenConsumer.getToken().type == TokenType::_ELSE){
        tokenConsumer.consume(TokenType::_ELSE);
        ifStmt->addElseStmt(parseStmt());
    }
    
    return ifStmt;
}

std::unique_ptr<ASTWhileStmt> StatementParser::parseWhileStmt(){
    std::unique_ptr<ASTWhileStmt> whileStmt = 
        std::make_unique<ASTWhileStmt>(Token{"while_statement", tokenConsumer.getToken().line, tokenConsumer.getToken().column});
    tokenConsumer.consume(TokenType::_WHILE);
    
    tokenConsumer.consume(TokenType::_LPAREN);
    auto condition{ exprParser.parseRelationalExpr() };
    tokenConsumer.consume(TokenType::_RPAREN);
    
    whileStmt->setWhileStmt(std::move(condition), parseStmt());

    return whileStmt;
}

std::unique_ptr<ASTForStmt> StatementParser::parseForStmt(){
    std::unique_ptr<ASTForStmt> forStmt = 
        std::make_unique<ASTForStmt>(Token{"for_statement", tokenConsumer.getToken().line, tokenConsumer.getToken().column});
    tokenConsumer.consume(TokenType::_FOR);

    std::unique_ptr<ASTAssignStmt> forInitializer{ nullptr }, forIncrementer{ nullptr };
    std::unique_ptr<ASTExpr> condition{ nullptr };

    tokenConsumer.consume(TokenType::_LPAREN);
    // optional initializer
    if(tokenConsumer.getToken().type != TokenType::_SEMICOLON){
        forInitializer = parseAssignStmt();
    }

    // optional condition (default: true)
    if(tokenConsumer.getToken().type != TokenType::_SEMICOLON){
        condition = exprParser.parseRelationalExpr();
    }
    tokenConsumer.consume(TokenType::_SEMICOLON);

    // optional incrementer
    if(tokenConsumer.getToken().type != TokenType::_RPAREN){
        forIncrementer = parseAssignStmt(false); // false - doesn't expect semicolon at the end of the statement
    }
    tokenConsumer.consume(TokenType::_RPAREN);

    forStmt->setForSt(std::move(forInitializer), std::move(condition), std::move(forIncrementer), parseStmt());
    
    return forStmt;
}

std::unique_ptr<ASTDoWhileStmt> StatementParser::parseDoWhileStmt(){
    std::unique_ptr<ASTDoWhileStmt> dowhileStmt = 
        std::make_unique<ASTDoWhileStmt>(Token{"dowhile_statement", tokenConsumer.getToken().line, tokenConsumer.getToken().column});
    
    tokenConsumer.consume(TokenType::_DO);
    std::unique_ptr<ASTStmt> stmt{ parseStmt() };
    tokenConsumer.consume(TokenType::_WHILE);

    tokenConsumer.consume(TokenType::_LPAREN);
    dowhileStmt->setDoWhile(exprParser.parseRelationalExpr(), std::move(stmt));
    tokenConsumer.consume(TokenType::_RPAREN);

    tokenConsumer.consume(TokenType::_SEMICOLON);
    return dowhileStmt;
}

std::unique_ptr<ASTFunctionCallStmt> StatementParser::parseFunctionCallStmt(){
    std::unique_ptr<ASTFunctionCallStmt> callStmt = std::make_unique<ASTFunctionCallStmt>(Token{"fcall_statement", tokenConsumer.getToken().line, tokenConsumer.getToken().column});
    callStmt->setFunctionCallStmt(exprParser.parseFunctionCallExpr());
    tokenConsumer.consume(TokenType::_SEMICOLON);

    return callStmt;
}

std::unique_ptr<ASTSwitchStmt> StatementParser::parseSwitchStmt(){
    std::unique_ptr<ASTSwitchStmt> switchStmt = 
        std::make_unique<ASTSwitchStmt>(Token{"switch_statement", tokenConsumer.getToken().line, tokenConsumer.getToken().column});
    tokenConsumer.consume(TokenType::_SWITCH);

    tokenConsumer.consume(TokenType::_LPAREN);
    switchStmt->setVariableIdExpr(exprParser.parseIdExpr());
    tokenConsumer.consume(TokenType::_RPAREN);

    tokenConsumer.consume(TokenType::_LBRACKET);
    
    // switch must have at least 1 case
    do{
        switchStmt->addCaseStmt(parseCaseStmt());
    }while(tokenConsumer.getToken().type == TokenType::_CASE);
    
    if(tokenConsumer.getToken().type == TokenType::_DEFAULT){
        switchStmt->setDefaultStmt(parseDefaultStmt());
    }

    tokenConsumer.consume(TokenType::_RBRACKET);
    return switchStmt;
}

std::unique_ptr<ASTSwitchBlockStmt> StatementParser::parseSwitchBlockStmt(){
    std::unique_ptr<ASTSwitchBlockStmt> switchBlockStmt = 
        std::make_unique<ASTSwitchBlockStmt>(Token{"switch_block", tokenConsumer.getToken().line, tokenConsumer.getToken().column});
    
    while(tokenConsumer.getToken().type != TokenType::_CASE && tokenConsumer.getToken().type != TokenType::_DEFAULT && 
            tokenConsumer.getToken().type != TokenType::_RBRACKET && tokenConsumer.getToken().type != TokenType::_BREAK){
        
        switchBlockStmt->addStmt(parseStmt());
    }

    return switchBlockStmt;
}

std::unique_ptr<ASTCaseStmt> StatementParser::parseCaseStmt(){
    bool hasBreak{ false };
    std::unique_ptr<ASTCaseStmt> caseStmt = 
        std::make_unique<ASTCaseStmt>(Token{"case", tokenConsumer.getToken().line, tokenConsumer.getToken().column});
    tokenConsumer.consume(TokenType::_CASE);
    std::unique_ptr<ASTLiteralExpr> literalExpr{ exprParser.parseLiteralExpr() };
    tokenConsumer.consume(TokenType::_COLON);
    
    std::unique_ptr<ASTSwitchBlockStmt> switchBlockStmt{ parseSwitchBlockStmt() };
    if(tokenConsumer.getToken().type == TokenType::_BREAK){
        hasBreak = true;
        parseBreakStmt();
    }

    caseStmt->setCase(std::move(literalExpr), std::move(switchBlockStmt), hasBreak);

    return caseStmt;
}

std::unique_ptr<ASTDefaultStmt> StatementParser::parseDefaultStmt(){
    std::unique_ptr<ASTDefaultStmt> defaultStmt = 
        std::make_unique<ASTDefaultStmt>(Token{"default", tokenConsumer.getToken().line, tokenConsumer.getToken().column});
    tokenConsumer.consume(TokenType::_DEFAULT);
    tokenConsumer.consume(TokenType::_COLON);
    
    defaultStmt->setDefaultStmt(parseSwitchBlockStmt());
    if(tokenConsumer.getToken().type == TokenType::_BREAK){
        parseBreakStmt();
    }

    return defaultStmt;
}

void StatementParser::parseBreakStmt(){
    // only for switch statement at the moment, located at the end of the case
    tokenConsumer.consume(TokenType::_BREAK);
    tokenConsumer.consume(TokenType::_SEMICOLON);
}