#include "parser.hpp"

#include <memory>
#include <stdexcept>
#include <format>

Parser::Parser(Lexer& lexer) : lexer{ lexer }, currentToken{ lexer.nextToken() } {}

std::unique_ptr<ASTree> Parser::parseProgram(){
    std::unique_ptr<ASTree> root = std::make_unique<ASTree>(Token{"program", 0, 0}, ASTNodeType::PROGRAM);
    root->pushChild(functionList());

    // check if input ends correctly
    if(currentToken.type != TokenType::_EOF){
        throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> near '{}'",
            currentToken.line, currentToken.column, currentToken.value));
    }
    return root;
}

void Parser::eat(TokenType type){
    if(currentToken.type == type){
        currentToken = lexer.nextToken();
    } 
    else{
        throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> expected '{}', got '{} {}'",
            currentToken.line, currentToken.column, tokenTypeToString.at(type), tokenTypeToString.at(currentToken.type), currentToken.value));
    }
}

// FUNCTION_LIST : FUNCTION 
//               | FUNCTION_LIST FUNCTION
std::unique_ptr<ASTree> Parser::functionList(){
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(Token{"function_list", 0, 0}, ASTNodeType::FUNCTION_LIST); 
    do{
        currentNode->pushChild(function());
    } while(currentToken.type == TokenType::_TYPE);

    return currentNode;
}

// FUNCTION : TYPE ID LPAREN PARAMETERS RPAREN BODY
std::unique_ptr<ASTree> Parser::function(){
    Types returnType{ stringToType.find(currentToken.value) != stringToType.end() ? stringToType.at(currentToken.value) : Types::NO_TYPE };
    eat(TokenType::_TYPE);
    Token token{ currentToken };
    eat(TokenType::_ID);
    
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(token, returnType, ASTNodeType::FUNCTION);

    eat(TokenType::_LPAREN);
    currentNode->pushChild(parameter());
    eat(TokenType::_RPAREN);
    currentNode->pushChild(body());
    
    return currentNode;
}

// PARAMETERS : EMPTY 
//            | PARAMETER 
//            | PARAMETERS COMMA PARAMETER
std::unique_ptr<ASTree> Parser::parameter(){
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(Token{"params", currentToken.line, currentToken.column}, ASTNodeType::PARAMETER);
    while(currentToken.type == TokenType::_TYPE){
        Types type{ stringToType.at(currentToken.value) };
        eat(TokenType::_TYPE);
        auto token{ currentToken };
        eat(TokenType::_ID);
        
        currentNode->pushChild(std::make_unique<ASTree>(token, type, ASTNodeType::PARAMETER));

        if(currentToken.type == TokenType::_COMMA && lexer.peekAtNext().type == TokenType::_TYPE){
            eat(TokenType::_COMMA);
        }
        else{
            break;
        }
    }
    return currentNode;
}

// BODY : LBRACKET CONSTRUCTS RBRACKET
std::unique_ptr<ASTree> Parser::body(){
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(Token{"body", currentToken.line, currentToken.column}, ASTNodeType::BODY);
    eat(TokenType::_LBRACKET);
    while(currentToken.type != TokenType::_RBRACKET){
        currentNode->pushChild(construct());
    }
    eat(TokenType::_RBRACKET);
    return currentNode;
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
    eat(TokenType::_TYPE);
    Token token{ currentToken };
    std::unique_ptr<ASTree> variable = std::make_unique<ASTree>(token, type, ASTNodeType::VARIABLE);
    
    lexer.peekAtNext().type == TokenType::_ASSIGN ? variable->pushChild(assignmentStatement()) : eat(TokenType::_ID);
    eat(TokenType::_SEMICOLON);

    return variable;
}

// STATEMENT : RETURN_STATEMENT
//           | IF_STATEMENT
//           | COMPOUND_STATEMENT
//           | WHILE_STATEMENT
//           | FOR_STATEMENT
//           | DO_WHILE_STATEMENT
//           | SWITCH_STATEMENT
std::unique_ptr<ASTree> Parser::statement(){
    if(lexer.peekAtNext().type == TokenType::_ASSIGN){
        std::unique_ptr<ASTree> node = assignmentStatement();
        eat(TokenType::_SEMICOLON);
        return node;
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
    else
        throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> near '{}'",
            currentToken.line, currentToken.column, currentToken.value));
}

// COMPOUND_STATEMENT : LBRACKET CONSTRUCTS RBRACKET
std::unique_ptr<ASTree> Parser::compoundStatement(){
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(Token{"cpnd_statement", currentToken.line, currentToken.column}, ASTNodeType::COMPOUND_STATEMENT);

    eat(TokenType::_LBRACKET);
    while(currentToken.type != TokenType::_RBRACKET){
        currentNode->pushChild(construct());
    }
    eat(TokenType::_RBRACKET);
    
    return currentNode;
}

// ASSIGNMENT_STATEMENT : ID ASSIGN NUMERICAL_EXPRESSION SEMICOLON
std::unique_ptr<ASTree> Parser::assignmentStatement(){
    if(currentToken.type == TokenType::_ID){
        std::unique_ptr<ASTree> lchild = std::make_unique<ASTree>(Token{currentToken}, ASTNodeType::VARIABLE);
        eat(TokenType::_ID);
        std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(Token{"=", currentToken.line, currentToken.column}, ASTNodeType::ASSIGNMENT_STATEMENT);
        eat(TokenType::_ASSIGN);
        std::unique_ptr<ASTree> rchild = numericalExpression();
        
        currentNode->pushChild(std::move(lchild));
        currentNode->pushChild(std::move(rchild));
        return currentNode;
    }
    else{
        throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> invalid assignment statement: expected 'ID', got '{} {}'",
            currentToken.line, currentToken.column, tokenTypeToString.at(currentToken.type), currentToken.value));
    }
}

// RETURN_STATEMENT : RETURN SEMICOLON 
//                  | RETURN NUMERICAL_EXPRESSION SEMICOLON
std::unique_ptr<ASTree> Parser::returnStatement(){
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(Token{"return_statement", currentToken.line, currentToken.column}, ASTNodeType::RETURN_STATEMENT);
    eat(TokenType::_RETURN);

    if(currentToken.type == TokenType::_SEMICOLON){
        currentNode->setType(Types::VOID);
        eat(TokenType::_SEMICOLON);
    }
    else{
        currentNode->pushChild(numericalExpression());
        eat(TokenType::_SEMICOLON);
    }

    return currentNode;   
}

// IF_STATEMENT : IF LPAREN RELATIONAL_EXPRESSION RPAREN CONSTRUCT
//              | IF_STATEMENT ELSE CONSTRUCT 
//              | IF_STATEMENT ELSE IF CONSTRUCT
std::unique_ptr<ASTree> Parser::ifStatement(){
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(Token{"if_stat", currentToken.line, currentToken.column}, ASTNodeType::IF_STATEMENT);
    eat(TokenType::_IF);
    eat(TokenType::_LPAREN);
    currentNode->pushChild(relationalExpression());
    eat(TokenType::_RPAREN);
    
    currentNode->pushChild(construct());

    while(currentToken.type == TokenType::_ELSE && lexer.peekAtNext().type == TokenType::_IF){
        eat(TokenType::_ELSE);
        eat(TokenType::_IF);

        eat(TokenType::_LPAREN);
        currentNode->pushChild(relationalExpression());
        eat(TokenType::_RPAREN);
        
        currentNode->pushChild(construct());
    }

    if(currentToken.type == TokenType::_ELSE){
        eat(TokenType::_ELSE);
        currentNode->pushChild(construct());
    }
    
    return currentNode;
}

// WHILE_STATEMENT : WHILE LPAREN RELATIONAL_EXPRESSION RPAREN CONSTRUCT
std::unique_ptr<ASTree> Parser::whileStatement(){
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(Token{"while_stat", currentToken.line, currentToken.column}, ASTNodeType::WHILE_STATEMENT);
    eat(TokenType::_WHILE);
    
    eat(TokenType::_LPAREN);
    currentNode->pushChild(relationalExpression());
    eat(TokenType::_RPAREN);
    
    currentNode->pushChild(construct());

    return currentNode;
}

// FOR_STATEMENT : FOR LPAREN ASSIGN_STATEMENT SEMICOLON RELATIONAL_EXPRESSION SEMICOLON ASSIGNMENT_STATEMENT RPAREN CONSTRUCT
std::unique_ptr<ASTree> Parser::forStatement(){
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(Token{"for_stat", currentToken.line, currentToken.column}, ASTNodeType::FOR_STATEMENT);
    eat(TokenType::_FOR);

    eat(TokenType::_LPAREN);
    currentNode->pushChild(assignmentStatement());
    eat(TokenType::_SEMICOLON);
    currentNode->pushChild(relationalExpression());
    eat(TokenType::_SEMICOLON);
    currentNode->pushChild(assignmentStatement());
    eat(TokenType::_RPAREN);

    currentNode->pushChild(construct());
    
    return currentNode;
}

// DO_WHILE_STATEMENT : DO CONSTRUCT WHILE LPAREN RELATIONAL_EXPRESSION RPAREN SEMICOLON
std::unique_ptr<ASTree> Parser::doWhileStatement(){
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(Token{"dowhile_stat", currentToken.line, currentToken.column}, ASTNodeType::DO_WHILE_STATEMENT);
    eat(TokenType::_DO);
    currentNode->pushChild(construct());
    eat(TokenType::_WHILE);

    eat(TokenType::_LPAREN);
    currentNode->pushChild(relationalExpression());
    eat(TokenType::_RPAREN);
    eat(TokenType::_SEMICOLON);

    return currentNode;
}

// SWITCH_STATEMENT : SWITCH LPAREN ID RPAREN LBRACKET CASES _DEFAULT RBRACKET
std::unique_ptr<ASTree> Parser::switchStatement(){
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(Token{"switch_stat", currentToken.line, currentToken.column}, ASTNodeType::SWITCH_STATEMENT);
    eat(TokenType::_SWITCH);

    eat(TokenType::_LPAREN);
    currentNode->pushChild(id());
    eat(TokenType::_RPAREN);

    eat(TokenType::_LBRACKET);
    
    currentNode->pushChild(_case());
    while(currentToken.type == TokenType::_CASE){
        currentNode->pushChild(_case());
    }
    
    if(currentToken.type == TokenType::_DEFAULT){
        currentNode->pushChild(_default());
    }

    eat(TokenType::_RBRACKET);
    return currentNode;
}

// SWITCH_CASE_CONSTRUCT : CONSTRUCT 
//                       | SWITCH_CASE_CONSTRUCT CONSTRUCT
std::unique_ptr<ASTree> Parser::switchCaseConstruct(){
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(Token{"statement_list", currentToken.line, currentToken.column}, ASTNodeType::STATEMENT_LIST);
    while(currentToken.type != TokenType::_CASE && currentToken.type != TokenType::_DEFAULT && 
            currentToken.type != TokenType::_RBRACKET && currentToken.type != TokenType::_BREAK){
        
        currentNode->pushChild(construct());
    }

    return currentNode;
}

// _CASE : CASE LITERAL COLON SWITCH_CASE_CONSTRUCT
//       | CASE LITERAL COLON SWITCH_CASE_CONSTRUCT _BREAK
std::unique_ptr<ASTree> Parser::_case(){
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(Token{"case", currentToken.line, currentToken.column}, ASTNodeType::CASE);
    eat(TokenType::_CASE);
    currentNode->pushChild(literal());
    eat(TokenType::_LITERAL);
    eat(TokenType::_COLON);
    
    currentNode->pushChild(switchCaseConstruct());
    if(currentToken.type == TokenType::_BREAK){
        currentNode->pushChild(_break());
    }

    return currentNode;
}

// _DEFAULT : DEFAULT COLON SWITCH_CASE_CONSTRUCT
//          | DEFAULT COLON SWITCH_CASE_CONSTRUCT _BREAK
std::unique_ptr<ASTree> Parser::_default(){
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(Token{"default", currentToken.line, currentToken.column}, ASTNodeType::DEFAULT);
    eat(TokenType::_DEFAULT);
    eat(TokenType::_COLON);
    
    currentNode->pushChild(switchCaseConstruct());
    if(currentToken.type == TokenType::_BREAK){
        eat(TokenType::_BREAK);
        eat(TokenType::_SEMICOLON);
    }

    return currentNode;
}

// _BREAK : BREAK SEMICOLON
std::unique_ptr<ASTree> Parser::_break(){
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(Token{"break", currentToken.line, currentToken.column}, ASTNodeType::BREAK);
    eat(TokenType::_BREAK);
    eat(TokenType::_SEMICOLON);
    return currentNode;
}

// NUMERICAL_EXPRESSION : EXPRESSION 
//                      | NUMERICAL_EXPRESSION AROP EXPRESSION
// organized in a tree as reverse polish notation (RPN)
std::unique_ptr<ASTree> Parser::numericalExpression(){
    std::unique_ptr<ASTree> child = expression();
    std::stack<std::unique_ptr<ASTree>> rpn;
    std::stack<std::unique_ptr<ASTree>> weakOperators;
    
    rpn.push(std::move(child));
    while(currentToken.type == TokenType::_AROP || currentToken.type == TokenType::_BITWISE){
        std::unique_ptr<ASTree> arOperator = std::make_unique<ASTree>(Token{currentToken}, ASTNodeType::NUMERICAL_EXPRESSION);
        if(currentToken.type == TokenType::_BITWISE){
            eat(TokenType::_BITWISE);
        }else{
            eat(TokenType::_AROP);
        }
        child = expression();
        rpn.push(std::move(child));

        weakOperators.push(std::move(arOperator));
        while(getPrecedence(weakOperators.top()->getToken().value) < getPrecedence(currentToken.value)){
            std::unique_ptr<ASTree> strongArOp = std::make_unique<ASTree>(Token{currentToken}, ASTNodeType::NUMERICAL_EXPRESSION);
            if(currentToken.type == TokenType::_BITWISE){
                eat(TokenType::_BITWISE);
            }else{
                eat(TokenType::_AROP);
            }
            std::unique_ptr<ASTree> strongChild = expression();
            rpn.push(std::move(strongChild));
            if(getPrecedence(strongArOp->getToken().value) < getPrecedence(currentToken.value))
                weakOperators.push(std::move(strongArOp));
            else
                rpn.push(std::move(strongArOp));
        }
        while(!weakOperators.empty() && getPrecedence(weakOperators.top()->getToken().value) >= getPrecedence(currentToken.value)){
            rpn.push(std::move(weakOperators.top()));
            weakOperators.pop();
        }
    }

    std::unique_ptr<ASTree> root = std::move(rpn.top());
    rpn.pop();

    return rpnToTree(rpn, root);
}

// recursive top-down tree building from rpn based stack
// child is not numerical expression - leaf
// parent has two children - subtree is handled
std::unique_ptr<ASTree> Parser::rpnToTree(std::stack<std::unique_ptr<ASTree>>& rpn, std::unique_ptr<ASTree>& parent) const {
    if(parent->getNodeType() != ASTNodeType::NUMERICAL_EXPRESSION || !parent->getChildren().empty()){
        return std::move(parent);
    }
    std::unique_ptr<ASTree> r = std::move(rpn.top());
    rpn.pop();
    std::unique_ptr<ASTree> rchild = rpnToTree(rpn,r);

    std::unique_ptr<ASTree> l = std::move(rpn.top());
    rpn.pop();
    std::unique_ptr<ASTree> lchild = rpnToTree(rpn, l);

    parent->pushChild(std::move(lchild));
    parent->pushChild(std::move(rchild));

    return std::move(parent);
}

// EXPRESSION : LITERAL 
//            | FUNCTION_CALL 
//            | ID 
//            | LPAREN NUMERICAL_EXPRESSION RPAREN
std::unique_ptr<ASTree> Parser::expression(){
    if(currentToken.type == TokenType::_LITERAL){
        std::unique_ptr<ASTree> node = literal();
        eat(TokenType::_LITERAL);
        return node;
    }
    else if(currentToken.type == TokenType::_ID && lexer.peekAtNext().type == TokenType::_LPAREN){
        return functionCall();
    }
    else if(currentToken.type == TokenType::_ID){
        return id();    
    }
    else if(currentToken.type == TokenType::_LPAREN){
        eat(TokenType::_LPAREN);
        std::unique_ptr<ASTree> node = numericalExpression();
        eat(TokenType::_RPAREN);
        return node;
    }
    throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> expected 'EXPRESSION', got '{} {}'",
            currentToken.line, currentToken.column, tokenTypeToString.at(currentToken.type), currentToken.value));
}

// RELATIONAL EXPRESSION : NUMERICAL_EXPRESSION RELOP NUMERICAL_EXPRESSION
std::unique_ptr<ASTree> Parser::relationalExpression(){
    std::unique_ptr<ASTree> lchild = numericalExpression();
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(currentToken, ASTNodeType::RELATIONAL_EXPRESSION);
    eat(TokenType::_RELOP);
    currentNode->pushChild(std::move(lchild));
    currentNode->pushChild(numericalExpression());
    return currentNode;
}

// FUNCTION_CALL : ID LPAREN ARGUMENT RPAREN
std::unique_ptr<ASTree> Parser::functionCall(){
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(currentToken, ASTNodeType::FUNCTION_CALL);
    eat(TokenType::_ID);
    eat(TokenType::_LPAREN);
    currentNode->pushChild(argument());
    eat(TokenType::_RPAREN);
    return currentNode;
}

// ARGUMENT : EMPTY 
//          | NUMERICAL_EXPRESSION 
//          | ARGUMENT COMMA NUMERICAL_EXPRESSION
std::unique_ptr<ASTree> Parser::argument(){
    std::unique_ptr<ASTree> currentNode = std::make_unique<ASTree>(Token{"arg", currentToken.line, currentToken.column}, ASTNodeType::ARGUMENT);
    while(currentToken.type != TokenType::_RPAREN){
        currentNode->pushChild(numericalExpression());
        if(currentToken.type == TokenType::_COMMA && lexer.peekAtNext().type != TokenType::_RPAREN){
            eat(TokenType::_COMMA);
        }
        else{
            break;
        }
    }
    return currentNode;
}

// ID : ID
std::unique_ptr<ASTree> Parser::id(){
    Token token{ currentToken };
    eat(TokenType::_ID);

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
// -> PRINT (TODO)
// -> ANONYMOUS FUNCTIONS (TODO)
// -> INCLUDE / HANDLING MULTIPLE FILES (TODO)
// -> PRE/POST INCREMENT/DECREMENT (TODO)
// -> BITWISE OPERATORS (!, ~) (TODO)
// -> TERNARY OPERATOR ? : (TODO)