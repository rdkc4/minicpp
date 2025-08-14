#include "../statement_parser.hpp"
#include "../token_consumer.hpp"

StatementParser::StatementParser(Lexer& lexer) : TokenConsumer{ lexer }, expParser{ lexer } {}

// STATEMENT : VARIABLE_DECL
//           | RETURN_STATEMENT
//           | PRINTF_STATEMENT
//           | IF_STATEMENT
//           | COMPOUND_STATEMENT
//           | WHILE_STATEMENT
//           | FOR_STATEMENT
//           | DO_WHILE_STATEMENT
//           | SWITCH_STATEMENT
std::unique_ptr<ASTStatement> StatementParser::statement(){
    if(getToken().gtype == GeneralTokenType::TYPE){
        return variable();
    }
    else if(peek().type == TokenType::_ASSIGN){
        return assignmentStatement();
    }
    else if(getToken().type == TokenType::_PRINTF){
        return printfStatement();
    }
    else if(getToken().type == TokenType::_RETURN)
        return returnStatement();
    else if(getToken().type == TokenType::_IF)
        return ifStatement();
    else if(getToken().type == TokenType::_LBRACKET)
        return compoundStatement();
    else if(getToken().type == TokenType::_WHILE)
        return whileStatement();
    else if(getToken().type == TokenType::_FOR)
        return forStatement();
    else if(getToken().type == TokenType::_DO)
        return doWhileStatement();
    else if(getToken().type == TokenType::_SWITCH)
        return switchStatement();
    
    throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> near '{}'",
        getToken().line, getToken().column, getToken().value));
}

// VARIABLE : TYPE ID SEMICOLON
//          | TYPE ID ASSIGN ASSIGNMENT_STATEMENT SEMICOLON
std::unique_ptr<ASTVariable> StatementParser::variable(){
    Types type{ tokenTypeToType.find(getToken().type) != tokenTypeToType.end() ? tokenTypeToType.at(getToken().type) : Types::NO_TYPE };
    consume(GeneralTokenType::TYPE);
    std::unique_ptr<ASTVariable> _variable = std::make_unique<ASTVariable>(getToken(), ASTNodeType::VARIABLE, type);
    consume(TokenType::_ID);
    
    if(getToken().type == TokenType::_ASSIGN){
        consume(TokenType::_ASSIGN);
        _variable->setAssign(expParser.numericalExpression());
    }
    consume(TokenType::_SEMICOLON);

    return _variable;
}

// PRINTF_STATEMENT: PRINTF LPAREN NUMERICAL_EXPRESSION RPAREN SEMICOLON;
std::unique_ptr<ASTPrintfSt> StatementParser::printfStatement(){
    std::unique_ptr<ASTPrintfSt> _printf = 
        std::make_unique<ASTPrintfSt>(Token{"printf_statement", getToken().line, getToken().column}, ASTNodeType::PRINTF);

    consume(TokenType::_PRINTF);
    consume(TokenType::_LPAREN);
    _printf->setExp(expParser.numericalExpression());
    consume(TokenType::_RPAREN);
    consume(TokenType::_SEMICOLON);

    return _printf;
}

// COMPOUND_STATEMENT : LBRACKET RBRACKET
//                    | LBRACKET STATEMENT RBRACKET
std::unique_ptr<ASTCompoundSt> StatementParser::compoundStatement(){
    std::unique_ptr<ASTCompoundSt> _compound = 
        std::make_unique<ASTCompoundSt>(Token{"compound_statement", getToken().line, getToken().column}, ASTNodeType::COMPOUND_STATEMENT);

    consume(TokenType::_LBRACKET);
    while(getToken().type != TokenType::_RBRACKET){
        _compound->addStatement(statement());
    }
    consume(TokenType::_RBRACKET);
    
    return _compound;
}

// ASSIGNMENT_STATEMENT : ID ASSIGN NUMERICAL_EXPRESSION SEMICOLON
std::unique_ptr<ASTAssignSt> StatementParser::assignmentStatement(bool expectsSemicolon){
    std::unique_ptr<ASTId> _variable{ expParser.id() };
    
    std::unique_ptr<ASTAssignSt> _assignment = 
        std::make_unique<ASTAssignSt>(Token{"=", getToken().line, getToken().column}, ASTNodeType::ASSIGNMENT_STATEMENT);
    consume(TokenType::_ASSIGN);
    
    _assignment->setVariable(std::move(_variable));
    _assignment->setExp(expParser.numericalExpression());

    if(expectsSemicolon){
        consume(TokenType::_SEMICOLON);
    }
    return _assignment;
}

// RETURN_STATEMENT : RETURN SEMICOLON 
//                  | RETURN NUMERICAL_EXPRESSION SEMICOLON
std::unique_ptr<ASTReturnSt> StatementParser::returnStatement(){
    std::unique_ptr<ASTReturnSt> _return = 
        std::make_unique<ASTReturnSt>(Token{"return_statement", getToken().line, getToken().column}, ASTNodeType::RETURN_STATEMENT);
    consume(TokenType::_RETURN);

    if(getToken().type != TokenType::_SEMICOLON){
        _return->setExp(expParser.numericalExpression());
    }
    consume(TokenType::_SEMICOLON);

    return _return;
}

// IF_STATEMENT : IF LPAREN RELATIONAL_EXPRESSION RPAREN STATEMENT
//              | IF_STATEMENT ELSE IF STATEMENT
//              | IF_STATEMENT ELSE STATEMENT
std::unique_ptr<ASTIfSt> StatementParser::ifStatement(){
    std::unique_ptr<ASTIfSt> _if = 
        std::make_unique<ASTIfSt>(Token{"if_statement", getToken().line, getToken().column}, ASTNodeType::IF_STATEMENT);
    consume(TokenType::_IF);

    consume(TokenType::_LPAREN);
    auto condition{ expParser.relationalExpression() };
    consume(TokenType::_RPAREN);
    
    _if->addIf(std::move(condition), statement());

    while(getToken().type == TokenType::_ELSE && peek().type == TokenType::_IF){
        consume(TokenType::_ELSE);
        consume(TokenType::_IF);

        consume(TokenType::_LPAREN);
        condition = expParser.relationalExpression();
        consume(TokenType::_RPAREN);
        
        _if->addIf(std::move(condition), statement());
    }

    if(getToken().type == TokenType::_ELSE){
        consume(TokenType::_ELSE);
        _if->addElse(statement());
    }
    
    return _if;
}

// WHILE_STATEMENT : WHILE LPAREN RELATIONAL_EXPRESSION RPAREN STATEMENT
std::unique_ptr<ASTWhileSt> StatementParser::whileStatement(){
    std::unique_ptr<ASTWhileSt> _while = 
        std::make_unique<ASTWhileSt>(Token{"while_statement", getToken().line, getToken().column}, ASTNodeType::WHILE_STATEMENT);
    consume(TokenType::_WHILE);
    
    consume(TokenType::_LPAREN);
    auto condition{ expParser.relationalExpression() };
    consume(TokenType::_RPAREN);
    
    _while->setDoWhile(std::move(condition), statement());

    return _while;
}

// FOR_STATEMENT : FOR LPAREN ASSIGN_STATEMENT SEMICOLON RELATIONAL_EXPRESSION SEMICOLON ASSIGNMENT_STATEMENT RPAREN STATEMENT
std::unique_ptr<ASTForSt> StatementParser::forStatement(){
    std::unique_ptr<ASTForSt> _for = 
        std::make_unique<ASTForSt>(Token{"for_statement", getToken().line, getToken().column}, ASTNodeType::FOR_STATEMENT);
    consume(TokenType::_FOR);

    std::unique_ptr<ASTAssignSt> _init{ nullptr }, _inc{ nullptr };
    std::unique_ptr<ASTExpression> _condition{ nullptr };

    consume(TokenType::_LPAREN);
    // optional initializer
    if(getToken().type != TokenType::_SEMICOLON){
        _init = assignmentStatement();
    }

    // optional condition (default: true)
    if(getToken().type != TokenType::_SEMICOLON){
        _condition = expParser.relationalExpression();
    }
    consume(TokenType::_SEMICOLON);

    // optional incrementer
    if(getToken().type != TokenType::_RPAREN){
        _inc = assignmentStatement(false); // false - doesn't expect semicolon at the end of statement
    }
    consume(TokenType::_RPAREN);

    _for->setForSt(std::move(_init), std::move(_condition), std::move(_inc), statement());
    
    return _for;
}

// DO_WHILE_STATEMENT : DO STATEMENT WHILE LPAREN RELATIONAL_EXPRESSION RPAREN SEMICOLON
std::unique_ptr<ASTDoWhileSt> StatementParser::doWhileStatement(){
    std::unique_ptr<ASTDoWhileSt> _dowhile = 
        std::make_unique<ASTDoWhileSt>(Token{"dowhile_statement", getToken().line, getToken().column}, ASTNodeType::DO_WHILE_STATEMENT);
    
    consume(TokenType::_DO);
    std::unique_ptr<ASTStatement> _statement{ statement() };
    consume(TokenType::_WHILE);

    consume(TokenType::_LPAREN);
    _dowhile->setDoWhile(expParser.relationalExpression(), std::move(_statement));
    consume(TokenType::_RPAREN);

    consume(TokenType::_SEMICOLON);
    return _dowhile;
}

// SWITCH_STATEMENT : SWITCH LPAREN ID RPAREN LBRACKET CASES _DEFAULT RBRACKET
std::unique_ptr<ASTSwitchSt> StatementParser::switchStatement(){
    std::unique_ptr<ASTSwitchSt> _switch = 
        std::make_unique<ASTSwitchSt>(Token{"switch_statement", getToken().line, getToken().column}, ASTNodeType::SWITCH_STATEMENT);
    consume(TokenType::_SWITCH);

    consume(TokenType::_LPAREN);
    _switch->setVariable(expParser.id());
    consume(TokenType::_RPAREN);

    consume(TokenType::_LBRACKET);
    
    // switch must have at least 1 case
    do{
        _switch->addCase(_case());
    }while(getToken().type == TokenType::_CASE);
    
    if(getToken().type == TokenType::_DEFAULT){
        _switch->setDefault(_default());
    }

    consume(TokenType::_RBRACKET);
    return _switch;
}

// SWITCH_CASE_BLOCK : EMPTY
//                   | STATEMENT
//                   | SWITCH_CASE_BLOCK STATEMENT
std::unique_ptr<ASTSwitchBlock> StatementParser::switchCaseBlock(){
    std::unique_ptr<ASTSwitchBlock> _switchBlock = 
        std::make_unique<ASTSwitchBlock>(Token{"switch_block", getToken().line, getToken().column}, ASTNodeType::SWITCH_BLOCK);
    
    while(getToken().type != TokenType::_CASE && getToken().type != TokenType::_DEFAULT && 
            getToken().type != TokenType::_RBRACKET && getToken().type != TokenType::_BREAK){
        
        _switchBlock->addStatement(statement());
    }

    return _switchBlock;
}

// _CASE : CASE LITERAL COLON SWITCH_CASE_BLOCK
//       | CASE LITERAL COLON SWITCH_CASE_BLOCK _BREAK
std::unique_ptr<ASTCaseSt> StatementParser::_case(){
    bool hasBreak{ false };
    std::unique_ptr<ASTCaseSt> _swCase = 
        std::make_unique<ASTCaseSt>(Token{"case", getToken().line, getToken().column}, ASTNodeType::CASE);
    consume(TokenType::_CASE);
    std::unique_ptr<ASTLiteral> _literal{ expParser.literal() };
    consume(TokenType::_COLON);
    
    std::unique_ptr<ASTSwitchBlock> _swBlock{ switchCaseBlock() };
    if(getToken().type == TokenType::_BREAK){
        hasBreak = true;
        _break();
    }

    _swCase->setCase(std::move(_literal), std::move(_swBlock), hasBreak);

    return _swCase;
}

// _DEFAULT : DEFAULT COLON SWITCH_CASE_BLOCK
//          | DEFAULT COLON SWITCH_CASE_BLOCK _BREAK
std::unique_ptr<ASTDefaultSt> StatementParser::_default(){
    std::unique_ptr<ASTDefaultSt> _swDefault = 
        std::make_unique<ASTDefaultSt>(Token{"default", getToken().line, getToken().column}, ASTNodeType::DEFAULT);
    consume(TokenType::_DEFAULT);
    consume(TokenType::_COLON);
    
    _swDefault->setDefault(switchCaseBlock());
    if(getToken().type == TokenType::_BREAK){
        _break();
    }

    return _swDefault;
}

// _BREAK : BREAK SEMICOLON
void StatementParser::_break(){
    // only for switch statement at the moment, located at the end of case
    consume(TokenType::_BREAK);
    consume(TokenType::_SEMICOLON);
}