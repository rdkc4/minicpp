#include "../statement_parser.hpp"
#include "../token_consumer.hpp"

#include <format>

StatementParser::StatementParser(TokenConsumer& consumer) : expParser{ consumer }, tokenConsumer{ consumer } {}

std::unique_ptr<ASTStmt> StatementParser::statement(){
    if(tokenConsumer.getToken().gtype == GeneralTokenType::TYPE){
        return variable();
    }
    else if(tokenConsumer.peek().type == TokenType::_ASSIGN){
        return assignmentStatement();
    }

    auto token = tokenConsumer.getToken();
    if(token.type == TokenType::_RETURN)
        return returnStatement();
    else if(token.type == TokenType::_IF)
        return ifStatement();
    else if(token.type == TokenType::_LBRACKET)
        return compoundStatement();
    else if(token.type == TokenType::_WHILE)
        return whileStatement();
    else if(token.type == TokenType::_FOR)
        return forStatement();
    else if(token.type == TokenType::_DO)
        return doWhileStatement();
    else if(token.type == TokenType::_SWITCH)
        return switchStatement();
    else if(token.type == TokenType::_ID && tokenConsumer.peek().type == TokenType::_LPAREN)
        return functionCallStatement();
    
    throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> near '{}'",
        token.line, token.column, token.value));
}

std::unique_ptr<ASTVariableDeclStmt> StatementParser::variable(){
    Type type{ tokenTypeToType.find(tokenConsumer.getToken().type) != tokenTypeToType.end() ? tokenTypeToType.at(tokenConsumer.getToken().type) : Type::NO_TYPE };
    tokenConsumer.consume(GeneralTokenType::TYPE);
    std::unique_ptr<ASTVariableDeclStmt> _variable = std::make_unique<ASTVariableDeclStmt>(tokenConsumer.getToken(), ASTNodeType::VARIABLE, type);
    tokenConsumer.consume(TokenType::_ID);
    
    if(tokenConsumer.getToken().type == TokenType::_ASSIGN){
        tokenConsumer.consume(TokenType::_ASSIGN);
        _variable->setAssign(expParser.numericalExpression());
    }
    tokenConsumer.consume(TokenType::_SEMICOLON);

    return _variable;
}

std::unique_ptr<ASTCompoundStmt> StatementParser::compoundStatement(){
    std::unique_ptr<ASTCompoundStmt> _compound = 
        std::make_unique<ASTCompoundStmt>(Token{"compound_statement", tokenConsumer.getToken().line, tokenConsumer.getToken().column}, ASTNodeType::COMPOUND_STATEMENT);

    tokenConsumer.consume(TokenType::_LBRACKET);
    while(tokenConsumer.getToken().type != TokenType::_RBRACKET){
        _compound->addStatement(statement());
    }
    tokenConsumer.consume(TokenType::_RBRACKET);
    
    return _compound;
}

std::unique_ptr<ASTAssignStmt> StatementParser::assignmentStatement(bool expectsSemicolon){
    std::unique_ptr<ASTIdExpr> _variable{ expParser.id() };
    
    std::unique_ptr<ASTAssignStmt> _assignment = 
        std::make_unique<ASTAssignStmt>(Token{"=", tokenConsumer.getToken().line, tokenConsumer.getToken().column}, ASTNodeType::ASSIGNMENT_STATEMENT);
    tokenConsumer.consume(TokenType::_ASSIGN);
    
    _assignment->setVariable(std::move(_variable));
    _assignment->setExp(expParser.numericalExpression());

    if(expectsSemicolon){
        tokenConsumer.consume(TokenType::_SEMICOLON);
    }
    return _assignment;
}

std::unique_ptr<ASTReturnStmt> StatementParser::returnStatement(){
    std::unique_ptr<ASTReturnStmt> _return = 
        std::make_unique<ASTReturnStmt>(Token{"return_statement", tokenConsumer.getToken().line, tokenConsumer.getToken().column}, ASTNodeType::RETURN_STATEMENT);
    tokenConsumer.consume(TokenType::_RETURN);

    if(tokenConsumer.getToken().type != TokenType::_SEMICOLON){
        _return->setExp(expParser.numericalExpression());
    }
    tokenConsumer.consume(TokenType::_SEMICOLON);

    return _return;
}

std::unique_ptr<ASTIfStmt> StatementParser::ifStatement(){
    std::unique_ptr<ASTIfStmt> _if = 
        std::make_unique<ASTIfStmt>(Token{"if_statement", tokenConsumer.getToken().line, tokenConsumer.getToken().column}, ASTNodeType::IF_STATEMENT);
    tokenConsumer.consume(TokenType::_IF);

    tokenConsumer.consume(TokenType::_LPAREN);
    auto condition{ expParser.relationalExpression() };
    tokenConsumer.consume(TokenType::_RPAREN);
    
    _if->addIf(std::move(condition), statement());

    while(tokenConsumer.getToken().type == TokenType::_ELSE && tokenConsumer.peek().type == TokenType::_IF){
        tokenConsumer.consume(TokenType::_ELSE);
        tokenConsumer.consume(TokenType::_IF);

        tokenConsumer.consume(TokenType::_LPAREN);
        condition = expParser.relationalExpression();
        tokenConsumer.consume(TokenType::_RPAREN);
        
        _if->addIf(std::move(condition), statement());
    }

    if(tokenConsumer.getToken().type == TokenType::_ELSE){
        tokenConsumer.consume(TokenType::_ELSE);
        _if->addElse(statement());
    }
    
    return _if;
}

std::unique_ptr<ASTWhileStmt> StatementParser::whileStatement(){
    std::unique_ptr<ASTWhileStmt> _while = 
        std::make_unique<ASTWhileStmt>(Token{"while_statement", tokenConsumer.getToken().line, tokenConsumer.getToken().column}, ASTNodeType::WHILE_STATEMENT);
    tokenConsumer.consume(TokenType::_WHILE);
    
    tokenConsumer.consume(TokenType::_LPAREN);
    auto condition{ expParser.relationalExpression() };
    tokenConsumer.consume(TokenType::_RPAREN);
    
    _while->setWhile(std::move(condition), statement());

    return _while;
}

std::unique_ptr<ASTForStmt> StatementParser::forStatement(){
    std::unique_ptr<ASTForStmt> _for = 
        std::make_unique<ASTForStmt>(Token{"for_statement", tokenConsumer.getToken().line, tokenConsumer.getToken().column}, ASTNodeType::FOR_STATEMENT);
    tokenConsumer.consume(TokenType::_FOR);

    std::unique_ptr<ASTAssignStmt> _init{ nullptr }, _inc{ nullptr };
    std::unique_ptr<ASTExpr> _condition{ nullptr };

    tokenConsumer.consume(TokenType::_LPAREN);
    // optional initializer
    if(tokenConsumer.getToken().type != TokenType::_SEMICOLON){
        _init = assignmentStatement();
    }

    // optional condition (default: true)
    if(tokenConsumer.getToken().type != TokenType::_SEMICOLON){
        _condition = expParser.relationalExpression();
    }
    tokenConsumer.consume(TokenType::_SEMICOLON);

    // optional incrementer
    if(tokenConsumer.getToken().type != TokenType::_RPAREN){
        _inc = assignmentStatement( false); // false - doesn't expect semicolon at the end of the statement
    }
    tokenConsumer.consume(TokenType::_RPAREN);

    _for->setForSt(std::move(_init), std::move(_condition), std::move(_inc), statement());
    
    return _for;
}

std::unique_ptr<ASTDoWhileStmt> StatementParser::doWhileStatement(){
    std::unique_ptr<ASTDoWhileStmt> _dowhile = 
        std::make_unique<ASTDoWhileStmt>(Token{"dowhile_statement", tokenConsumer.getToken().line, tokenConsumer.getToken().column}, ASTNodeType::DO_WHILE_STATEMENT);
    
    tokenConsumer.consume(TokenType::_DO);
    std::unique_ptr<ASTStmt> _statement{ statement() };
    tokenConsumer.consume(TokenType::_WHILE);

    tokenConsumer.consume(TokenType::_LPAREN);
    _dowhile->setDoWhile(expParser.relationalExpression(), std::move(_statement));
    tokenConsumer.consume(TokenType::_RPAREN);

    tokenConsumer.consume(TokenType::_SEMICOLON);
    return _dowhile;
}

std::unique_ptr<ASTFunctionCallStmt> StatementParser::functionCallStatement(){
    std::unique_ptr<ASTFunctionCallStmt> _funcCall = std::make_unique<ASTFunctionCallStmt>(Token{"fcall_statement", tokenConsumer.getToken().line, tokenConsumer.getToken().column}, ASTNodeType::FUNCTION_CALL_STATEMENT);
    _funcCall->initFunctionCallSt(expParser.functionCall());
    tokenConsumer.consume(TokenType::_SEMICOLON);

    return _funcCall;
}

std::unique_ptr<ASTSwitchStmt> StatementParser::switchStatement(){
    std::unique_ptr<ASTSwitchStmt> _switch = 
        std::make_unique<ASTSwitchStmt>(Token{"switch_statement", tokenConsumer.getToken().line, tokenConsumer.getToken().column}, ASTNodeType::SWITCH_STATEMENT);
    tokenConsumer.consume(TokenType::_SWITCH);

    tokenConsumer.consume(TokenType::_LPAREN);
    _switch->setVariable(expParser.id());
    tokenConsumer.consume(TokenType::_RPAREN);

    tokenConsumer.consume(TokenType::_LBRACKET);
    
    // switch must have at least 1 case
    do{
        _switch->addCase(_case());
    }while(tokenConsumer.getToken().type == TokenType::_CASE);
    
    if(tokenConsumer.getToken().type == TokenType::_DEFAULT){
        _switch->setDefault(_default());
    }

    tokenConsumer.consume(TokenType::_RBRACKET);
    return _switch;
}

std::unique_ptr<ASTSwitchBlockStmt> StatementParser::switchCaseBlock(){
    std::unique_ptr<ASTSwitchBlockStmt> _switchBlock = 
        std::make_unique<ASTSwitchBlockStmt>(Token{"switch_block", tokenConsumer.getToken().line, tokenConsumer.getToken().column}, ASTNodeType::SWITCH_BLOCK);
    
    while(tokenConsumer.getToken().type != TokenType::_CASE && tokenConsumer.getToken().type != TokenType::_DEFAULT && 
            tokenConsumer.getToken().type != TokenType::_RBRACKET && tokenConsumer.getToken().type != TokenType::_BREAK){
        
        _switchBlock->addStatement(statement());
    }

    return _switchBlock;
}

std::unique_ptr<ASTCaseStmt> StatementParser::_case(){
    bool hasBreak{ false };
    std::unique_ptr<ASTCaseStmt> _swCase = 
        std::make_unique<ASTCaseStmt>(Token{"case", tokenConsumer.getToken().line, tokenConsumer.getToken().column}, ASTNodeType::CASE);
    tokenConsumer.consume(TokenType::_CASE);
    std::unique_ptr<ASTLiteralExpr> _literal{ expParser.literal() };
    tokenConsumer.consume(TokenType::_COLON);
    
    std::unique_ptr<ASTSwitchBlockStmt> _swBlock{ switchCaseBlock() };
    if(tokenConsumer.getToken().type == TokenType::_BREAK){
        hasBreak = true;
        _break();
    }

    _swCase->setCase(std::move(_literal), std::move(_swBlock), hasBreak);

    return _swCase;
}

std::unique_ptr<ASTDefaultStmt> StatementParser::_default(){
    std::unique_ptr<ASTDefaultStmt> _swDefault = 
        std::make_unique<ASTDefaultStmt>(Token{"default", tokenConsumer.getToken().line, tokenConsumer.getToken().column}, ASTNodeType::DEFAULT);
    tokenConsumer.consume(TokenType::_DEFAULT);
    tokenConsumer.consume(TokenType::_COLON);
    
    _swDefault->setDefault(switchCaseBlock());
    if(tokenConsumer.getToken().type == TokenType::_BREAK){
        _break();
    }

    return _swDefault;
}

void StatementParser::_break(){
    // only for switch statement at the moment, located at the end of the case
    tokenConsumer.consume(TokenType::_BREAK);
    tokenConsumer.consume(TokenType::_SEMICOLON);
}