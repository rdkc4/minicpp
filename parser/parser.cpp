#include "parser.hpp"

#include <memory>
#include <stdexcept>
#include <format>

Parser::Parser(Lexer& lexer) : lexer{ lexer }, currentToken{ lexer.nextToken() } {}

std::unique_ptr<ASTree> Parser::parseProgram(){
    std::unique_ptr<ASTree> root = std::make_unique<ASTree>(Token{"program", 0, 0}, ASTNodeType::PROGRAM);
    root->pushChild(functionList());

    // check if input ends correctly
    consume(TokenType::_EOF);

    return root;
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
std::unique_ptr<ASTree> Parser::functionList(){
    std::unique_ptr<ASTree> _functionList = std::make_unique<ASTree>(Token{"function_list", 0, 0}, ASTNodeType::FUNCTION_LIST); 
    do{
        _functionList->pushChild(function());
    } while(currentToken.type == TokenType::_TYPE);

    return _functionList;
}

// FUNCTION : TYPE ID LPAREN PARAMETERS RPAREN BODY
std::unique_ptr<ASTree> Parser::function(){
    Types returnType{ stringToType.find(currentToken.value) != stringToType.end() ? stringToType.at(currentToken.value) : Types::NO_TYPE };
    consume(TokenType::_TYPE);
    Token token{ currentToken };
    consume(TokenType::_ID);
    
    std::unique_ptr<ASTree> _function = std::make_unique<ASTree>(token, returnType, ASTNodeType::FUNCTION);

    consume(TokenType::_LPAREN);
    _function->pushChild(parameter());
    consume(TokenType::_RPAREN);
    _function->pushChild(body());
    
    return _function;
}

// PARAMETERS : EMPTY 
//            | PARAMETER 
//            | PARAMETERS COMMA PARAMETER
std::unique_ptr<ASTree> Parser::parameter(){
    std::unique_ptr<ASTree> _parameters = 
        std::make_unique<ASTree>(Token{"params", currentToken.line, currentToken.column}, ASTNodeType::PARAMETER);

    while(currentToken.type == TokenType::_TYPE){
        Types type{ stringToType.at(currentToken.value) };
        consume(TokenType::_TYPE);
        auto token{ currentToken };
        consume(TokenType::_ID);
        
        _parameters->pushChild(std::make_unique<ASTree>(token, type, ASTNodeType::PARAMETER));

        if(currentToken.type == TokenType::_COMMA && lexer.peekAtNext().type == TokenType::_TYPE){
            consume(TokenType::_COMMA);
        }
        else{
            break;
        }
    }
    return _parameters;
}

// BODY : LBRACKET CONSTRUCTS RBRACKET
std::unique_ptr<ASTree> Parser::body(){
    std::unique_ptr<ASTree> _body = std::make_unique<ASTree>(Token{"body", currentToken.line, currentToken.column}, ASTNodeType::BODY);
    consume(TokenType::_LBRACKET);
    while(currentToken.type != TokenType::_RBRACKET){
        _body->pushChild(construct());
    }
    consume(TokenType::_RBRACKET);
    return _body;
}

// CONSTRUCT : VARIABLE 
//           | STATEMENT
std::unique_ptr<ASTree> Parser::construct(){
    if(currentToken.type == TokenType::_TYPE){
        return variable();
    }
    else{
        return statement();
    }
}

// VARIABLE : TYPE ID SEMICOLON
//          | TYPE ID ASSIGN ASSIGNMENT_STATEMENT SEMICOLON
std::unique_ptr<ASTree> Parser::variable(){
    Types type{ stringToType.find(currentToken.value) != stringToType.end() ? stringToType.at(currentToken.value) : Types::NO_TYPE };
    consume(TokenType::_TYPE);
    Token token{ currentToken };
    std::unique_ptr<ASTree> _variable = std::make_unique<ASTree>(token, type, ASTNodeType::VARIABLE);
    
    if(lexer.peekAtNext().type == TokenType::_ASSIGN){
        _variable->pushChild(assignmentStatement());
    }
    else{
        consume(TokenType::_ID);
    }
    consume(TokenType::_SEMICOLON);

    return _variable;
}

// STATEMENT : RETURN_STATEMENT
//           | PRINTF_STATEMENT
//           | IF_STATEMENT
//           | COMPOUND_STATEMENT
//           | WHILE_STATEMENT
//           | FOR_STATEMENT
//           | DO_WHILE_STATEMENT
//           | SWITCH_STATEMENT
std::unique_ptr<ASTree> Parser::statement(){
    if(lexer.peekAtNext().type == TokenType::_ASSIGN){
        std::unique_ptr<ASTree> _assignment = assignmentStatement();
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

// PRINTF_STATEMENT: PRINTF LPAREN NUMERICAL_EXPRESSION RPAREN SEMICOLON;
std::unique_ptr<ASTree> Parser::printfStatement(){
    std::unique_ptr<ASTree> _printf = 
        std::make_unique<ASTree>(Token{"printf_statement", currentToken.line, currentToken.column}, ASTNodeType::PRINTF);

    consume(TokenType::_PRINTF);
    consume(TokenType::_LPAREN);
    _printf->pushChild(numericalExpression());
    consume(TokenType::_RPAREN);
    consume(TokenType::_SEMICOLON);

    return _printf;
}

// COMPOUND_STATEMENT : LBRACKET CONSTRUCTS RBRACKET
std::unique_ptr<ASTree> Parser::compoundStatement(){
    std::unique_ptr<ASTree> _compound = 
        std::make_unique<ASTree>(Token{"compound_statement", currentToken.line, currentToken.column}, ASTNodeType::COMPOUND_STATEMENT);

    consume(TokenType::_LBRACKET);
    while(currentToken.type != TokenType::_RBRACKET){
        _compound->pushChild(construct());
    }
    consume(TokenType::_RBRACKET);
    
    return _compound;
}

// ASSIGNMENT_STATEMENT : ID ASSIGN NUMERICAL_EXPRESSION SEMICOLON
std::unique_ptr<ASTree> Parser::assignmentStatement(){
    std::unique_ptr<ASTree> _variable = std::make_unique<ASTree>(Token{currentToken}, ASTNodeType::VARIABLE);
    consume(TokenType::_ID);
    
    std::unique_ptr<ASTree> _assignment = 
        std::make_unique<ASTree>(Token{"=", currentToken.line, currentToken.column}, ASTNodeType::ASSIGNMENT_STATEMENT);
    consume(TokenType::_ASSIGN);
    
    _assignment->pushChild(std::move(_variable));
    _assignment->pushChild(numericalExpression());
    return _assignment;
}

// RETURN_STATEMENT : RETURN SEMICOLON 
//                  | RETURN NUMERICAL_EXPRESSION SEMICOLON
std::unique_ptr<ASTree> Parser::returnStatement(){
    std::unique_ptr<ASTree> _return = 
        std::make_unique<ASTree>(Token{"return_statement", currentToken.line, currentToken.column}, ASTNodeType::RETURN_STATEMENT);
    consume(TokenType::_RETURN);

    if(currentToken.type == TokenType::_SEMICOLON){
        _return->setType(Types::VOID);
        consume(TokenType::_SEMICOLON);
    }
    else{
        _return->pushChild(numericalExpression());
        consume(TokenType::_SEMICOLON);
    }

    return _return;
}

// IF_STATEMENT : IF LPAREN RELATIONAL_EXPRESSION RPAREN CONSTRUCT
//              | IF_STATEMENT ELSE CONSTRUCT 
//              | IF_STATEMENT ELSE IF CONSTRUCT
std::unique_ptr<ASTree> Parser::ifStatement(){
    std::unique_ptr<ASTree> _if = 
        std::make_unique<ASTree>(Token{"if_statement", currentToken.line, currentToken.column}, ASTNodeType::IF_STATEMENT);
    consume(TokenType::_IF);
    
    consume(TokenType::_LPAREN);
    _if->pushChild(relationalExpression());
    consume(TokenType::_RPAREN);
    
    _if->pushChild(construct());

    while(currentToken.type == TokenType::_ELSE && lexer.peekAtNext().type == TokenType::_IF){
        consume(TokenType::_ELSE);
        consume(TokenType::_IF);

        consume(TokenType::_LPAREN);
        _if->pushChild(relationalExpression());
        consume(TokenType::_RPAREN);
        
        _if->pushChild(construct());
    }

    if(currentToken.type == TokenType::_ELSE){
        consume(TokenType::_ELSE);
        _if->pushChild(construct());
    }
    
    return _if;
}

// WHILE_STATEMENT : WHILE LPAREN RELATIONAL_EXPRESSION RPAREN CONSTRUCT
std::unique_ptr<ASTree> Parser::whileStatement(){
    std::unique_ptr<ASTree> _while = 
        std::make_unique<ASTree>(Token{"while_statement", currentToken.line, currentToken.column}, ASTNodeType::WHILE_STATEMENT);
    consume(TokenType::_WHILE);
    
    consume(TokenType::_LPAREN);
    _while->pushChild(relationalExpression());
    consume(TokenType::_RPAREN);
    
    _while->pushChild(construct());

    return _while;
}

// FOR_STATEMENT : FOR LPAREN ASSIGN_STATEMENT SEMICOLON RELATIONAL_EXPRESSION SEMICOLON ASSIGNMENT_STATEMENT RPAREN CONSTRUCT
std::unique_ptr<ASTree> Parser::forStatement(){
    std::unique_ptr<ASTree> _for = 
        std::make_unique<ASTree>(Token{"for_statement", currentToken.line, currentToken.column}, ASTNodeType::FOR_STATEMENT);
    consume(TokenType::_FOR);

    consume(TokenType::_LPAREN);
    _for->pushChild(assignmentStatement());
    consume(TokenType::_SEMICOLON);
    _for->pushChild(relationalExpression());
    consume(TokenType::_SEMICOLON);
    _for->pushChild(assignmentStatement());
    consume(TokenType::_RPAREN);

    _for->pushChild(construct());
    
    return _for;
}

// DO_WHILE_STATEMENT : DO CONSTRUCT WHILE LPAREN RELATIONAL_EXPRESSION RPAREN SEMICOLON
std::unique_ptr<ASTree> Parser::doWhileStatement(){
    std::unique_ptr<ASTree> _dowhile = std::make_unique<ASTree>(Token{"dowhile_statement", currentToken.line, currentToken.column}, ASTNodeType::DO_WHILE_STATEMENT);
    consume(TokenType::_DO);
    _dowhile->pushChild(construct());
    consume(TokenType::_WHILE);

    consume(TokenType::_LPAREN);
    _dowhile->pushChild(relationalExpression());
    consume(TokenType::_RPAREN);

    consume(TokenType::_SEMICOLON);
    return _dowhile;
}

// SWITCH_STATEMENT : SWITCH LPAREN ID RPAREN LBRACKET CASES _DEFAULT RBRACKET
std::unique_ptr<ASTree> Parser::switchStatement(){
    std::unique_ptr<ASTree> _switch = 
        std::make_unique<ASTree>(Token{"switch_statement", currentToken.line, currentToken.column}, ASTNodeType::SWITCH_STATEMENT);
    consume(TokenType::_SWITCH);

    consume(TokenType::_LPAREN);
    _switch->pushChild(id());
    consume(TokenType::_RPAREN);

    consume(TokenType::_LBRACKET);
    
    // switch must have at least 1 case
    do{
        _switch->pushChild(_case());
    }while(currentToken.type == TokenType::_CASE);
    
    if(currentToken.type == TokenType::_DEFAULT){
        _switch->pushChild(_default());
    }

    consume(TokenType::_RBRACKET);
    return _switch;
}

// SWITCH_CASE_CONSTRUCT : CONSTRUCT 
//                       | SWITCH_CASE_CONSTRUCT CONSTRUCT
std::unique_ptr<ASTree> Parser::switchCaseConstruct(){
    std::unique_ptr<ASTree> _switchConstructs = 
        std::make_unique<ASTree>(Token{"construct_list", currentToken.line, currentToken.column}, ASTNodeType::CONSTRUCT_LIST);
    
    while(currentToken.type != TokenType::_CASE && currentToken.type != TokenType::_DEFAULT && 
            currentToken.type != TokenType::_RBRACKET && currentToken.type != TokenType::_BREAK){
        
        _switchConstructs->pushChild(construct());
    }

    return _switchConstructs;
}

// _CASE : CASE LITERAL COLON SWITCH_CASE_CONSTRUCT
//       | CASE LITERAL COLON SWITCH_CASE_CONSTRUCT _BREAK
std::unique_ptr<ASTree> Parser::_case(){
    std::unique_ptr<ASTree> _swCase = std::make_unique<ASTree>(Token{"case", currentToken.line, currentToken.column}, ASTNodeType::CASE);
    consume(TokenType::_CASE);
    _swCase->pushChild(literal());
    consume(TokenType::_LITERAL);
    consume(TokenType::_COLON);
    
    _swCase->pushChild(switchCaseConstruct());
    if(currentToken.type == TokenType::_BREAK){
        _swCase->pushChild(_break());
    }

    return _swCase;
}

// _DEFAULT : DEFAULT COLON SWITCH_CASE_CONSTRUCT
//          | DEFAULT COLON SWITCH_CASE_CONSTRUCT _BREAK
std::unique_ptr<ASTree> Parser::_default(){
    std::unique_ptr<ASTree> _swDefault = 
        std::make_unique<ASTree>(Token{"default", currentToken.line, currentToken.column}, ASTNodeType::DEFAULT);
    consume(TokenType::_DEFAULT);
    consume(TokenType::_COLON);
    
    _swDefault->pushChild(switchCaseConstruct());
    if(currentToken.type == TokenType::_BREAK){
        consume(TokenType::_BREAK);
        consume(TokenType::_SEMICOLON);
    }

    return _swDefault;
}

// _BREAK : BREAK SEMICOLON
std::unique_ptr<ASTree> Parser::_break(){
    // only for switch statement at the moment
    std::unique_ptr<ASTree> _swBreak = 
        std::make_unique<ASTree>(Token{"break", currentToken.line, currentToken.column}, ASTNodeType::BREAK);
    consume(TokenType::_BREAK);
    consume(TokenType::_SEMICOLON);
    return _swBreak;
}

// NUMERICAL_EXPRESSION : EXPRESSION 
//                      | NUMERICAL_EXPRESSION AROP EXPRESSION
// organized in a tree as reverse polish notation (RPN)
// ast is built on the run (per subexpression)
std::unique_ptr<ASTree> Parser::numericalExpression(){
    std::unique_ptr<ASTree> _expression = expression();

    // holding the expression
    std::stack<std::unique_ptr<ASTree>> rpn;
    // handling the order of the operators
    std::stack<std::unique_ptr<ASTree>> weakOperators;
    
    rpn.push(std::move(_expression));
    while(currentToken.gtype == GeneralTokenType::OPERATOR){
        std::unique_ptr<ASTree> _operator = std::make_unique<ASTree>(Token{currentToken}, ASTNodeType::NUMERICAL_EXPRESSION);
        consume(GeneralTokenType::OPERATOR);

        _expression = expression();
        rpn.push(std::move(_expression));

        weakOperators.push(std::move(_operator));
        while(getPrecedence(weakOperators.top()->getToken().value) < getPrecedence(currentToken.value)){
            std::unique_ptr<ASTree> _nextOperator = std::make_unique<ASTree>(Token{currentToken}, ASTNodeType::NUMERICAL_EXPRESSION);
            consume(GeneralTokenType::OPERATOR);

            std::unique_ptr<ASTree> _nextExpression = expression();
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

    std::unique_ptr<ASTree> _numericalExpRoot = std::move(rpn.top());
    rpn.pop();

    return rpnToTree(rpn, _numericalExpRoot);
}

// recursive top-down tree building from rpn based stack
// child is not numerical expression - leaf
// if parent has children - subtree handled
std::unique_ptr<ASTree> Parser::rpnToTree(std::stack<std::unique_ptr<ASTree>>& rpn, std::unique_ptr<ASTree>& root) const {
    if(root->getNodeType() != ASTNodeType::NUMERICAL_EXPRESSION || !root->getChildren().empty()){
        return std::move(root);
    }
    std::unique_ptr<ASTree> _rightSubtreeRoot = std::move(rpn.top());
    rpn.pop();
    std::unique_ptr<ASTree> _rightSubtree = rpnToTree(rpn,_rightSubtreeRoot);

    std::unique_ptr<ASTree> _leftSubtreeRoot = std::move(rpn.top());
    rpn.pop();
    std::unique_ptr<ASTree> _leftSubtree = rpnToTree(rpn, _leftSubtreeRoot);

    root->pushChild(std::move(_leftSubtree));
    root->pushChild(std::move(_rightSubtree));

    return std::move(root);
}

// EXPRESSION : LITERAL 
//            | FUNCTION_CALL 
//            | ID 
//            | LPAREN NUMERICAL_EXPRESSION RPAREN
std::unique_ptr<ASTree> Parser::expression(){
    if(currentToken.type == TokenType::_LITERAL){
        std::unique_ptr<ASTree> _literal = literal();
        consume(TokenType::_LITERAL);
        return _literal;
    }
    else if(currentToken.type == TokenType::_ID && lexer.peekAtNext().type == TokenType::_LPAREN){
        return functionCall();
    }
    else if(currentToken.type == TokenType::_ID){
        return id();    
    }
    else if(currentToken.type == TokenType::_LPAREN){
        consume(TokenType::_LPAREN);
        std::unique_ptr<ASTree> _numericalExp = numericalExpression();
        consume(TokenType::_RPAREN);
        return _numericalExp;
    }
    throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> expected 'EXPRESSION', got '{} {}'",
            currentToken.line, currentToken.column, tokenTypeToString.at(currentToken.type), currentToken.value));
}

// RELATIONAL EXPRESSION : NUMERICAL_EXPRESSION RELOP NUMERICAL_EXPRESSION
std::unique_ptr<ASTree> Parser::relationalExpression(){
    std::unique_ptr<ASTree> _leftOperand = numericalExpression();
    std::unique_ptr<ASTree> _relationalExpression = std::make_unique<ASTree>(currentToken, ASTNodeType::RELATIONAL_EXPRESSION);
    consume(TokenType::_RELOP);
    _relationalExpression->pushChild(std::move(_leftOperand));
    _relationalExpression->pushChild(numericalExpression()); // pushing right operand
    return _relationalExpression;
}

// FUNCTION_CALL : ID LPAREN ARGUMENT RPAREN
std::unique_ptr<ASTree> Parser::functionCall(){
    std::unique_ptr<ASTree> _functionCall = std::make_unique<ASTree>(currentToken, ASTNodeType::FUNCTION_CALL);
    consume(TokenType::_ID);
    
    consume(TokenType::_LPAREN);
    _functionCall->pushChild(argument());
    consume(TokenType::_RPAREN);
    
    return _functionCall;
}

// ARGUMENT : EMPTY 
//          | NUMERICAL_EXPRESSION 
//          | ARGUMENT COMMA NUMERICAL_EXPRESSION
std::unique_ptr<ASTree> Parser::argument(){
    std::unique_ptr<ASTree> _arguments = 
        std::make_unique<ASTree>(Token{"args", currentToken.line, currentToken.column}, ASTNodeType::ARGUMENT);
    
    while(currentToken.type != TokenType::_RPAREN){
        _arguments->pushChild(numericalExpression());
        if(currentToken.type == TokenType::_COMMA && lexer.peekAtNext().type != TokenType::_RPAREN){
            consume(TokenType::_COMMA);
        }
        else{
            break;
        }
    }
    return _arguments;
}

// ID : ID
std::unique_ptr<ASTree> Parser::id(){
    Token token{ currentToken };
    consume(TokenType::_ID);

    return std::make_unique<ASTree>(token, ASTNodeType::ID);
}

// LITERAL : LITERAL(unsigned) | LITERAL(int)
std::unique_ptr<ASTree> Parser::literal() const {
    if(currentToken.value.back() == 'u'){
        return std::make_unique<ASTree>(Token{currentToken}, Types::UNSIGNED, ASTNodeType::LITERAL);
    }
    return std::make_unique<ASTree>(Token{currentToken}, Types::INT, ASTNodeType::LITERAL);
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