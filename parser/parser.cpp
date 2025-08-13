#include "parser.hpp"

#include <memory>
#include <stdexcept>
#include <format>

Parser::Parser(Lexer& lexer) : lexer{ lexer }, currentToken{ lexer.nextToken() } {}

std::unique_ptr<ASTProgram> Parser::parseProgram(){
    std::unique_ptr<ASTProgram> _program = std::make_unique<ASTProgram>(Token{"program", 0, 0}, ASTNodeType::PROGRAM);
    
    do{
        _program->addFunction(function());
    } while(currentToken.gtype == GeneralTokenType::TYPE);

    // check if input ends correctly
    consume(TokenType::_EOF);

    return _program;
}

void Parser::consume(TokenType expectedType){
    if(currentToken.type != expectedType){
        throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> expected '{}', got '{} {}'",
            currentToken.line, currentToken.column, tokenTypeToString.at(expectedType), tokenTypeToString.at(currentToken.type), currentToken.value));
    }
    currentToken = lexer.nextToken();
}

void Parser::consume(GeneralTokenType expectedGType){
    if(currentToken.gtype != expectedGType){
        throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> expected '{}', got '{} {}'",
            currentToken.line, currentToken.column, generalTokenTypeToString.at(expectedGType), generalTokenTypeToString.at(currentToken.gtype), currentToken.value));
    }
    currentToken = lexer.nextToken();
}

// FUNCTION_LIST : FUNCTION 
//               | FUNCTION_LIST FUNCTION
void Parser::functionList(ASTProgram* _program){
    do{
        _program->addFunction(function());
    } while(currentToken.gtype == GeneralTokenType::TYPE);
}

// FUNCTION : TYPE ID LPAREN PARAMETERS RPAREN BODY
std::unique_ptr<ASTFunction> Parser::function(){
    Types type{ tokenTypeToType.find(currentToken.type) != tokenTypeToType.end() ? tokenTypeToType.at(currentToken.type) : Types::NO_TYPE };
    consume(GeneralTokenType::TYPE);
    const Token token{ currentToken };
    consume(TokenType::_ID);
    
    std::unique_ptr<ASTFunction> _function = std::make_unique<ASTFunction>(token, ASTNodeType::FUNCTION, type);

    consume(TokenType::_LPAREN);
    parameter(_function.get());
    consume(TokenType::_RPAREN);
    body(_function.get());
    
    return _function;
}

// PARAMETERS : EMPTY 
//            | PARAMETER 
//            | PARAMETERS COMMA PARAMETER
void Parser::parameter(ASTFunction* _function){
    while(currentToken.gtype == GeneralTokenType::TYPE){
        Types type{ tokenTypeToType.at(currentToken.type) };
        consume(GeneralTokenType::TYPE);
        const Token token{ currentToken };
        consume(TokenType::_ID);
        
        _function->addParameter(std::make_unique<ASTParameter>(token, ASTNodeType::PARAMETER, type));

        if(currentToken.type == TokenType::_COMMA && lexer.peekAtNext().gtype == GeneralTokenType::TYPE){
            consume(TokenType::_COMMA);
        }
        else{
            break;
        }
    }
}

// BODY : LBRACKET CONSTRUCTS RBRACKET
void Parser::body(ASTFunction* _function){
    consume(TokenType::_LBRACKET);
    while(currentToken.type != TokenType::_RBRACKET){
        _function->addStatement(statement());
    }
    consume(TokenType::_RBRACKET);
}

// STATEMENT : VARIABLE_DECL
//           | RETURN_STATEMENT
//           | PRINTF_STATEMENT
//           | IF_STATEMENT
//           | COMPOUND_STATEMENT
//           | WHILE_STATEMENT
//           | FOR_STATEMENT
//           | DO_WHILE_STATEMENT
//           | SWITCH_STATEMENT
std::unique_ptr<ASTStatement> Parser::statement(){
    if(currentToken.gtype == GeneralTokenType::TYPE){
        return variable();
    }
    else if(lexer.peekAtNext().type == TokenType::_ASSIGN){
        std::unique_ptr<ASTAssignSt> _assignment = assignmentStatement();
        consume(TokenType::_SEMICOLON);
        return _assignment;
    }
    else if(currentToken.type == TokenType::_PRINTF){
        return printfStatement();
    }
    else if(currentToken.type == TokenType::_RETURN)
        return returnStatement();
    else if(currentToken.type == TokenType::_IF)
        return ifStatement();
    else if(currentToken.type == TokenType::_LBRACKET)
        return compoundStatement();
    else if(currentToken.type == TokenType::_WHILE)
        return whileStatement();
    else if(currentToken.type == TokenType::_FOR)
        return forStatement();
    else if(currentToken.type == TokenType::_DO)
        return doWhileStatement();
    else if(currentToken.type == TokenType::_SWITCH)
        return switchStatement();
    
    throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> near '{}'",
        currentToken.line, currentToken.column, currentToken.value));
}

// VARIABLE : TYPE ID SEMICOLON
//          | TYPE ID ASSIGN ASSIGNMENT_STATEMENT SEMICOLON
std::unique_ptr<ASTVariable> Parser::variable(){
    Types type{ tokenTypeToType.find(currentToken.type) != tokenTypeToType.end() ? tokenTypeToType.at(currentToken.type) : Types::NO_TYPE };
    consume(GeneralTokenType::TYPE);
    std::unique_ptr<ASTVariable> _variable = std::make_unique<ASTVariable>(currentToken, ASTNodeType::VARIABLE, type);
    consume(TokenType::_ID);
    
    if(currentToken.type == TokenType::_ASSIGN){
        consume(TokenType::_ASSIGN);
        _variable->setAssign(numericalExpression());
    }
    consume(TokenType::_SEMICOLON);

    return _variable;
}

// PRINTF_STATEMENT: PRINTF LPAREN NUMERICAL_EXPRESSION RPAREN SEMICOLON;
std::unique_ptr<ASTPrintfSt> Parser::printfStatement(){
    std::unique_ptr<ASTPrintfSt> _printf = 
        std::make_unique<ASTPrintfSt>(Token{"printf_statement", currentToken.line, currentToken.column}, ASTNodeType::PRINTF);

    consume(TokenType::_PRINTF);
    consume(TokenType::_LPAREN);
    _printf->setExp(numericalExpression());
    consume(TokenType::_RPAREN);
    consume(TokenType::_SEMICOLON);

    return _printf;
}

// COMPOUND_STATEMENT : LBRACKET CONSTRUCTS RBRACKET
std::unique_ptr<ASTCompoundSt> Parser::compoundStatement(){
    std::unique_ptr<ASTCompoundSt> _compound = 
        std::make_unique<ASTCompoundSt>(Token{"compound_statement", currentToken.line, currentToken.column}, ASTNodeType::COMPOUND_STATEMENT);

    consume(TokenType::_LBRACKET);
    while(currentToken.type != TokenType::_RBRACKET){
        _compound->addStatement(statement());
    }
    consume(TokenType::_RBRACKET);
    
    return _compound;
}

// ASSIGNMENT_STATEMENT : ID ASSIGN NUMERICAL_EXPRESSION SEMICOLON
std::unique_ptr<ASTAssignSt> Parser::assignmentStatement(){
    std::unique_ptr<ASTId> _variable = id();
    
    std::unique_ptr<ASTAssignSt> _assignment = 
        std::make_unique<ASTAssignSt>(Token{"=", currentToken.line, currentToken.column}, ASTNodeType::ASSIGNMENT_STATEMENT);
    consume(TokenType::_ASSIGN);
    
    _assignment->setVariable(std::move(_variable));
    _assignment->setExp(numericalExpression());
    return _assignment;
}

// RETURN_STATEMENT : RETURN SEMICOLON 
//                  | RETURN NUMERICAL_EXPRESSION SEMICOLON
std::unique_ptr<ASTReturnSt> Parser::returnStatement(){
    std::unique_ptr<ASTReturnSt> _return = 
        std::make_unique<ASTReturnSt>(Token{"return_statement", currentToken.line, currentToken.column}, ASTNodeType::RETURN_STATEMENT);
    consume(TokenType::_RETURN);

    if(currentToken.type != TokenType::_SEMICOLON){
        _return->setExp(numericalExpression());
    }
    consume(TokenType::_SEMICOLON);

    return _return;
}

// IF_STATEMENT : IF LPAREN RELATIONAL_EXPRESSION RPAREN CONSTRUCT
//              | IF_STATEMENT ELSE CONSTRUCT 
//              | IF_STATEMENT ELSE IF CONSTRUCT
std::unique_ptr<ASTIfSt> Parser::ifStatement(){
    std::unique_ptr<ASTIfSt> _if = 
        std::make_unique<ASTIfSt>(Token{"if_statement", currentToken.line, currentToken.column}, ASTNodeType::IF_STATEMENT);
    consume(TokenType::_IF);

    consume(TokenType::_LPAREN);
    auto condition{ relationalExpression() };
    consume(TokenType::_RPAREN);
    
    _if->addIf(std::move(condition), statement());

    while(currentToken.type == TokenType::_ELSE && lexer.peekAtNext().type == TokenType::_IF){
        consume(TokenType::_ELSE);
        consume(TokenType::_IF);

        consume(TokenType::_LPAREN);
        condition = relationalExpression();
        consume(TokenType::_RPAREN);
        
        _if->addIf(std::move(condition), statement());
    }

    if(currentToken.type == TokenType::_ELSE){
        consume(TokenType::_ELSE);
        _if->addElse(statement());
    }
    
    return _if;
}

// WHILE_STATEMENT : WHILE LPAREN RELATIONAL_EXPRESSION RPAREN CONSTRUCT
std::unique_ptr<ASTWhileSt> Parser::whileStatement(){
    std::unique_ptr<ASTWhileSt> _while = 
        std::make_unique<ASTWhileSt>(Token{"while_statement", currentToken.line, currentToken.column}, ASTNodeType::WHILE_STATEMENT);
    consume(TokenType::_WHILE);
    
    consume(TokenType::_LPAREN);
    auto condition{ relationalExpression() };
    consume(TokenType::_RPAREN);
    
    _while->setDoWhile(std::move(condition), statement());

    return _while;
}

// FOR_STATEMENT : FOR LPAREN ASSIGN_STATEMENT SEMICOLON RELATIONAL_EXPRESSION SEMICOLON ASSIGNMENT_STATEMENT RPAREN CONSTRUCT
std::unique_ptr<ASTForSt> Parser::forStatement(){
    std::unique_ptr<ASTForSt> _for = 
        std::make_unique<ASTForSt>(Token{"for_statement", currentToken.line, currentToken.column}, ASTNodeType::FOR_STATEMENT);
    consume(TokenType::_FOR);

    std::unique_ptr<ASTAssignSt> _init{ nullptr }, _inc{ nullptr };
    std::unique_ptr<ASTExpression> _condition{ nullptr };

    consume(TokenType::_LPAREN);
    if(currentToken.type != TokenType::_SEMICOLON){
        _init = assignmentStatement();
    }
    consume(TokenType::_SEMICOLON);

    if(currentToken.type != TokenType::_SEMICOLON){
        _condition = relationalExpression();
    }
    consume(TokenType::_SEMICOLON);

    if(currentToken.type != TokenType::_SEMICOLON){
        _inc = assignmentStatement();
    }
    consume(TokenType::_RPAREN);

    _for->setForSt(std::move(_init), std::move(_condition), std::move(_inc), statement());
    
    return _for;
}

// DO_WHILE_STATEMENT : DO CONSTRUCT WHILE LPAREN RELATIONAL_EXPRESSION RPAREN SEMICOLON
std::unique_ptr<ASTDoWhileSt> Parser::doWhileStatement(){
    std::unique_ptr<ASTDoWhileSt> _dowhile = 
        std::make_unique<ASTDoWhileSt>(Token{"dowhile_statement", currentToken.line, currentToken.column}, ASTNodeType::DO_WHILE_STATEMENT);
    
    consume(TokenType::_DO);
    std::unique_ptr<ASTStatement> _statement{ statement() };
    consume(TokenType::_WHILE);

    consume(TokenType::_LPAREN);
    _dowhile->setDoWhile(relationalExpression(), std::move(_statement));
    consume(TokenType::_RPAREN);

    consume(TokenType::_SEMICOLON);
    return _dowhile;
}

// SWITCH_STATEMENT : SWITCH LPAREN ID RPAREN LBRACKET CASES _DEFAULT RBRACKET
std::unique_ptr<ASTSwitchSt> Parser::switchStatement(){
    std::unique_ptr<ASTSwitchSt> _switch = 
        std::make_unique<ASTSwitchSt>(Token{"switch_statement", currentToken.line, currentToken.column}, ASTNodeType::SWITCH_STATEMENT);
    consume(TokenType::_SWITCH);

    consume(TokenType::_LPAREN);
    _switch->setVariable(id());
    consume(TokenType::_RPAREN);

    consume(TokenType::_LBRACKET);
    
    // switch must have at least 1 case
    do{
        _switch->addCase(_case());
    }while(currentToken.type == TokenType::_CASE);
    
    if(currentToken.type == TokenType::_DEFAULT){
        _switch->setDefault(_default());
    }

    consume(TokenType::_RBRACKET);
    return _switch;
}

// SWITCH_CASE_CONSTRUCT : CONSTRUCT 
//                       | SWITCH_CASE_CONSTRUCT CONSTRUCT
std::unique_ptr<ASTSwitchBlock> Parser::switchCaseBlock(){
    std::unique_ptr<ASTSwitchBlock> _switchBlock = 
        std::make_unique<ASTSwitchBlock>(Token{"switch_block", currentToken.line, currentToken.column}, ASTNodeType::SWITCH_BLOCK);
    
    while(currentToken.type != TokenType::_CASE && currentToken.type != TokenType::_DEFAULT && 
            currentToken.type != TokenType::_RBRACKET && currentToken.type != TokenType::_BREAK){
        
        _switchBlock->addStatement(statement());
    }

    return _switchBlock;
}

// _CASE : CASE LITERAL COLON SWITCH_CASE_CONSTRUCT
//       | CASE LITERAL COLON SWITCH_CASE_CONSTRUCT _BREAK
std::unique_ptr<ASTCaseSt> Parser::_case(){
    bool hasBreak{ false };
    std::unique_ptr<ASTCaseSt> _swCase = 
        std::make_unique<ASTCaseSt>(Token{"case", currentToken.line, currentToken.column}, ASTNodeType::CASE);
    consume(TokenType::_CASE);
    std::unique_ptr<ASTLiteral> _literal{ literal() };
    consume(TokenType::_COLON);
    
    std::unique_ptr<ASTSwitchBlock> _swBlock{ switchCaseBlock() };
    if(currentToken.type == TokenType::_BREAK){
        hasBreak = true;
        _break();
    }

    _swCase->setCase(std::move(_literal), std::move(_swBlock), hasBreak);

    return _swCase;
}

// _DEFAULT : DEFAULT COLON SWITCH_CASE_CONSTRUCT
//          | DEFAULT COLON SWITCH_CASE_CONSTRUCT _BREAK
std::unique_ptr<ASTDefaultSt> Parser::_default(){
    std::unique_ptr<ASTDefaultSt> _swDefault = 
        std::make_unique<ASTDefaultSt>(Token{"default", currentToken.line, currentToken.column}, ASTNodeType::DEFAULT);
    consume(TokenType::_DEFAULT);
    consume(TokenType::_COLON);
    
    _swDefault->setDefault(switchCaseBlock());
    if(currentToken.type == TokenType::_BREAK){
        _break();
    }

    return _swDefault;
}

// _BREAK : BREAK SEMICOLON
void Parser::_break(){
    // only for switch statement at the moment
    consume(TokenType::_BREAK);
    consume(TokenType::_SEMICOLON);
}

// NUMERICAL_EXPRESSION : EXPRESSION 
//                      | NUMERICAL_EXPRESSION OPERATOR EXPRESSION
// organized in a tree as reverse polish notation (RPN)
// ast is built on the run (per subexpression)
std::unique_ptr<ASTExpression> Parser::numericalExpression(){
    std::unique_ptr<ASTExpression> _expression = expression();

    // holding the expression
    std::stack<std::unique_ptr<ASTExpression>> rpn;
    // handling the order of the operators
    std::stack<std::unique_ptr<ASTBinaryExpression>> weakOperators;
    
    rpn.push(std::move(_expression));
    while(currentToken.gtype == GeneralTokenType::OPERATOR){
        std::unique_ptr<ASTBinaryExpression> _op{ _operator() };

        _expression = expression();
        rpn.push(std::move(_expression));

        weakOperators.push(std::move(_op));
        while(getPrecedence(weakOperators.top()->getToken().value) < getPrecedence(currentToken.value)){
            std::unique_ptr<ASTBinaryExpression> _nextOperator{ _operator() };

            std::unique_ptr<ASTExpression> _nextExpression{ expression() };
            rpn.push(std::move(_nextExpression));
            
            if(getPrecedence(_nextOperator->getToken().value) < getPrecedence(currentToken.value)){
                weakOperators.push(std::move(_nextOperator));
            }
            else{
                rpn.push(std::move(_nextOperator));
            }

        }
        while(!weakOperators.empty() && getPrecedence(weakOperators.top()->getToken().value) >= getPrecedence(currentToken.value)){
            rpn.push(std::move(weakOperators.top()));
            weakOperators.pop();
        }
    }

    std::unique_ptr<ASTExpression> _numericalExpRoot{ std::move(rpn.top()) };
    rpn.pop();

    return rpnToTree(rpn, _numericalExpRoot);
}

// recursive top-down tree building from rpn based stack
// child is not binary expression - leaf
// if parent has children - subtree handled
std::unique_ptr<ASTExpression> Parser::rpnToTree(std::stack<std::unique_ptr<ASTExpression>>& rpn, std::unique_ptr<ASTExpression>& root) const {
    if(root->getNodeType() == ASTNodeType::ID || root->getNodeType() == ASTNodeType::LITERAL || root->getNodeType() == ASTNodeType::FUNCTION_CALL){
        return std::move(root);
    }
    else if(auto binExp = static_cast<ASTBinaryExpression*>(root.get())){
        if(binExp->initialized()){
            return std::move(root);
        }

        std::unique_ptr<ASTExpression> _rightSubtreeRoot = std::move(rpn.top());
        rpn.pop();
        std::unique_ptr<ASTExpression> _rightSubtree = rpnToTree(rpn,_rightSubtreeRoot);

        std::unique_ptr<ASTExpression> _leftSubtreeRoot = std::move(rpn.top());
        rpn.pop();
        std::unique_ptr<ASTExpression> _leftSubtree = rpnToTree(rpn, _leftSubtreeRoot);

        binExp->setOperands(std::move(_leftSubtree), std::move(_rightSubtree));
    }

    return std::move(root);
}

// EXPRESSION : LITERAL 
//            | FUNCTION_CALL 
//            | ID 
//            | LPAREN NUMERICAL_EXPRESSION RPAREN
std::unique_ptr<ASTExpression> Parser::expression(){
    if(currentToken.type == TokenType::_LITERAL){
        return literal();
    }
    else if(currentToken.type == TokenType::_ID && lexer.peekAtNext().type == TokenType::_LPAREN){
        return functionCall();
    }
    else if(currentToken.type == TokenType::_ID){
        return id();    
    }
    else if(currentToken.type == TokenType::_LPAREN){
        consume(TokenType::_LPAREN);
        std::unique_ptr<ASTExpression> _numericalExp = numericalExpression();
        consume(TokenType::_RPAREN);
        return _numericalExp;
    }
    throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> expected 'EXPRESSION', got '{} {}'",
            currentToken.line, currentToken.column, tokenTypeToString.at(currentToken.type), currentToken.value));
}


// RELATIONAL EXPRESSION : NUMERICAL_EXPRESSION RELOP NUMERICAL_EXPRESSION
std::unique_ptr<ASTExpression> Parser::relationalExpression(){
    std::unique_ptr<ASTExpression> _leftOperand = numericalExpression();
    std::unique_ptr<ASTBinaryExpression> _relationalExpression = _operator(true);
    _relationalExpression->setOperands(std::move(_leftOperand), numericalExpression());
    return _relationalExpression;
}


// FUNCTION_CALL : ID LPAREN ARGUMENT RPAREN
std::unique_ptr<ASTFunctionCall> Parser::functionCall(){
    std::unique_ptr<ASTFunctionCall> _functionCall = std::make_unique<ASTFunctionCall>(currentToken, ASTNodeType::FUNCTION_CALL);
    consume(TokenType::_ID);
    
    consume(TokenType::_LPAREN);
    argument(_functionCall.get());
    consume(TokenType::_RPAREN);
    
    return _functionCall;
}

// ARGUMENT : EMPTY 
//          | NUMERICAL_EXPRESSION 
//          | ARGUMENT COMMA NUMERICAL_EXPRESSION
void Parser::argument(ASTFunctionCall* _functionCall){
    while(currentToken.type != TokenType::_RPAREN){
        _functionCall->addArgument(numericalExpression());
        if(currentToken.type == TokenType::_COMMA && lexer.peekAtNext().type != TokenType::_RPAREN){
            consume(TokenType::_COMMA);
        }
        else{
            break;
        }
    }
}

// ID : ID
std::unique_ptr<ASTId> Parser::id(){
    const Token token{ currentToken };
    consume(TokenType::_ID);

    return std::make_unique<ASTId>(token, ASTNodeType::ID);
}

// LITERAL : LITERAL(unsigned) | LITERAL(int)
std::unique_ptr<ASTLiteral> Parser::literal(){
    const Token token{ currentToken };
    consume(TokenType::_LITERAL);
    if(token.value.back() == 'u'){
        return std::make_unique<ASTLiteral>(token, ASTNodeType::LITERAL, Types::UNSIGNED);
    }
    return std::make_unique<ASTLiteral>(token, ASTNodeType::LITERAL, Types::INT);
}

std::unique_ptr<ASTBinaryExpression> Parser::_operator(bool isRel){
    std::unique_ptr<ASTBinaryExpression> _op =  std::make_unique<ASTBinaryExpression>(Token{currentToken}, ASTNodeType::BINARY_EXPRESSION);
    if(stringToOperator.find(currentToken.value) != stringToOperator.end()){
        _op->setOperator(stringToOperator.at(currentToken.value));
    }

    if(isRel){
        consume(TokenType::_RELOP);
    }
    else{
        consume(GeneralTokenType::OPERATOR);
    }

    return _op;
}

// -> GLOBAL VARIABLES (TODO)
// -> RELATIONAL EXPRESSIONS WITH MULTIPLE CONDITIONS (TODO)
// -> MORE TYPES (TODO)
// -> ENUMERATORS (TODO)
// -> LISTS (TODO)
// -> ANONYMOUS FUNCTIONS (TODO)
// -> INCLUDE / HANDLING MULTIPLE FILES (TODO)
// -> PRE/POST INCREMENT/DECREMENT (TODO)
// -> BITWISE OPERATORS (!, ~) (TODO)
// -> TERNARY OPERATOR ? : (TODO)