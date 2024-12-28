#include "../include/parser.hpp"

Parser::Parser(Lexer& lexer, ScopeManager& scopeManager) : lexer(lexer), currentToken(lexer.nextToken()), analyzer(scopeManager) {}

Types Parser::getTypeFromToken(Token token){
    if(token.value == "int"){
        return Types::INT;
    }
    else if(token.value == "unsigned"){
        return Types::UNSIGNED;
    }
    else if(token.value == "void"){
        return Types::VOID;
    }
    else{
        return Types::NO_TYPE;
    }
}

void Parser::parseProgram(){
    root = std::make_shared<ASTree>(ASTNodeType::PROGRAM, Token("program", 0, 0));

    root->pushChild(functionList());
    if(currentToken.type != TokenType::_EOF){
        throw std::runtime_error("Line " + std::to_string(currentToken.line) + ", Column " + std::to_string(currentToken.column) 
            + " -> SYNTAX ERROR near: " + currentToken.value);
    }

    std::cout << "AST REPRESENTATION\n";
    root->traverse(1);
    analyzer.semanticCheck(root);
    std::cout << "\n\n";
    intermediateRepresentation.formIR(root);
    std::cout << "IR REPRESENTATION\n";
    intermediateRepresentation.getRoot()->traverse(1);
    codeGen.generateCode(intermediateRepresentation.getRoot());
}

void Parser::eat(TokenType type){
    if(currentToken.type == type){
        currentToken = lexer.nextToken();
    } 
    else{
        throw std::runtime_error("Line " + std::to_string(currentToken.line) + ", Column " + std::to_string(currentToken.column) 
            + " -> SYNTAX ERROR near: " + currentToken.value);
    }
}

std::shared_ptr<ASTree> Parser::functionList(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::FUNCTION_LIST, Token("function_list", 0, 0)); 
    while(currentToken.type == TokenType::_TYPE){
        currentNode->pushChild(function());
    }
    return currentNode;
}

std::shared_ptr<ASTree> Parser::function(){
    auto returnType = getTypeFromToken(currentToken);
    eat(TokenType::_TYPE);
    auto token = currentToken;
    eat(TokenType::_ID);
    
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::FUNCTION, Token(token), returnType);

    eat(TokenType::_LPAREN);
    currentNode->pushChild(parameter());
    eat(TokenType::_RPAREN);
    currentNode->pushChild(body());
    
    return currentNode;
}

std::shared_ptr<ASTree> Parser::parameter(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::PARAMETER, Token("params", currentToken.line, currentToken.column));
    while(currentToken.type == TokenType::_TYPE){
        Types type = getTypeFromToken(currentToken);
        eat(TokenType::_TYPE);
        auto token = currentToken;
        eat(TokenType::_ID);
        
        currentNode->pushChild(std::make_shared<ASTree>(ASTNodeType::PARAMETER, Token(token), type));

        if(currentToken.type == TokenType::_COMMA && lexer.peekAtNext().type == TokenType::_TYPE){
            eat(TokenType::_COMMA);
        }
    }
    return currentNode;
}

std::shared_ptr<ASTree> Parser::body(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::BODY, Token("body", currentToken.line, currentToken.column));
    eat(TokenType::_LBRACKET);
    currentNode->pushChild(variableList());
    currentNode->pushChild(statementList());
    eat(TokenType::_RBRACKET);
    return currentNode;
}

std::shared_ptr<ASTree> Parser::variableList(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::VARIABLE_LIST, Token("var_list", currentToken.line, currentToken.column));
    while(currentToken.type == TokenType::_TYPE){
        currentNode->pushChild(variable());
    }
    return currentNode;
}

std::shared_ptr<ASTree> Parser::variable(){
    Types type = getTypeFromToken(currentToken);
    eat(TokenType::_TYPE);
    auto token = currentToken;
    eat(TokenType::_ID);
    eat(TokenType::_SEMICOLON);

    return std::make_shared<ASTree>(ASTNodeType::VARIABLE, Token(token), type);
}

std::shared_ptr<ASTree> Parser::statementList(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::STATEMENT_LIST, Token("statement_list", currentToken.line, currentToken.column));
    while(currentToken.type != TokenType::_RBRACKET){
        currentNode->pushChild(statement());
    }
    return currentNode;
}

std::shared_ptr<ASTree> Parser::statement(){
    if(lexer.peekAtNext().type == TokenType::_ASSIGN){
        return assignmentStatement();
    }
    else if(currentToken.type == TokenType::_RETURN){
        return returnStatement();
    }
    else if(currentToken.type == TokenType::_IF){
        return ifStatement();
    }
    else if(currentToken.type == TokenType::_LBRACKET){
        return compoundStatement();
    }
    else{
        throw std::runtime_error("Line " + std::to_string(currentToken.line) + ", Column " + std::to_string(currentToken.column) 
            + " -> SYNTAX ERROR near: " + currentToken.value);
    }
}

std::shared_ptr<ASTree> Parser::compoundStatement(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::COMPOUND_STATEMENT, Token("cpnd_statement", currentToken.line, currentToken.column));

    eat(TokenType::_LBRACKET);
    currentNode->pushChild(statementList());
    eat(TokenType::_RBRACKET);
    
    return currentNode;
}

std::shared_ptr<ASTree> Parser::assignmentStatement(){
    if(currentToken.type == TokenType::_ID){
        auto rchild = std::make_shared<ASTree>(ASTNodeType::VARIABLE, Token(currentToken));
        eat(TokenType::_ID);
        auto currentNode = std::make_shared<ASTree>(ASTNodeType::ASSIGNMENT_STATEMENT, Token("=", currentToken.line, currentToken.column));
        eat(TokenType::_ASSIGN);
        auto lchild = numericalExpression();
        eat(TokenType::_SEMICOLON);
        
        currentNode->pushChild(lchild);
        currentNode->pushChild(rchild);
        return currentNode;
    }
    else{
        throw std::runtime_error("Line " + std::to_string(currentToken.line) + ", Column " + std::to_string(currentToken.column) 
            + " -> SYNTAX ERROR near: " + currentToken.value);
    }
}

std::shared_ptr<ASTree> Parser::returnStatement(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::RETURN_STATEMENT, Token("return_statement", currentToken.line, currentToken.column));
    if(currentToken.type == TokenType::_RETURN){
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
    else{
        throw std::runtime_error("Line " + std::to_string(currentToken.line) + ", Column " + std::to_string(currentToken.column) 
            + " -> SYNTAX ERROR near: " + currentToken.value);
    }
}

std::shared_ptr<ASTree> Parser::ifStatement(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::IF_STATEMENT, Token("ifstat", currentToken.line, currentToken.column));
    eat(TokenType::_IF);
    eat(TokenType::_LPAREN);
    currentNode->pushChild(relationalExpression());
    
    eat(TokenType::_RPAREN);
    
    currentNode->pushChild(statement());
    if(currentToken.type == TokenType::_ELSE){
        eat(TokenType::_ELSE);
        currentNode->pushChild(statement());
    }
    
    return currentNode;
}

std::shared_ptr<ASTree> Parser::numericalExpression(){
    auto child = expression();

    std::stack<std::shared_ptr<ASTree>> st;
    st.push(child);
    while(currentToken.type == TokenType::_AROP){
        auto arOperator = std::make_shared<ASTree>(ASTNodeType::NUMERICAL_EXPRESSION, Token(currentToken));
        eat(TokenType::_AROP);
        child = expression();
        st.push(child);
        st.push(arOperator);
    }

    auto head = st.top();
    st.pop();
    auto current = head;
    while(!st.empty()){
        auto r = st.top();
        st.pop();
        auto l = st.top();
        st.pop();
        current->pushChild(l);
        current->pushChild(r);
        current = l;
    }
    return head;
}

std::shared_ptr<ASTree> Parser::expression(){
    if(currentToken.type == TokenType::_LITERAL){
        auto node = literal();
        eat(TokenType::_LITERAL);
        return node;
    }
    else if(currentToken.type == TokenType::_ID && lexer.peekAtNext().type == TokenType::_LPAREN){
        return functionCall();
    }
    else if(currentToken.type == TokenType::_ID){
        auto token = currentToken;
        eat(TokenType::_ID);
        return std::make_shared<ASTree>(ASTNodeType::ID, token);
    }
    else if(currentToken.type == TokenType::_LPAREN){
        eat(TokenType::_LPAREN);
        auto node = numericalExpression();
        eat(TokenType::_RPAREN);
        return node;
    }
    throw std::runtime_error("Line " + std::to_string(currentToken.line) + ", Column " + std::to_string(currentToken.column) 
        + " -> SYNTAX ERROR near: " + currentToken.value);
}

std::shared_ptr<ASTree> Parser::relationalExpression(){
    auto lchild = numericalExpression();
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::RELATIONAL_EXPRESSION, currentToken);
    eat(TokenType::_RELOP);
    currentNode->pushChild(lchild);
    currentNode->pushChild(numericalExpression());
    return currentNode;
}

std::shared_ptr<ASTree> Parser::functionCall(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::FUNCTION_CALL, currentToken);
    eat(TokenType::_ID);
    eat(TokenType::_LPAREN);
    currentNode->pushChild(argument());
    eat(TokenType::_RPAREN);
    return currentNode;
}

std::shared_ptr<ASTree> Parser::argument(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::ARGUMENT, Token("arg", currentToken.line, currentToken.column));
    while(currentToken.type != TokenType::_RPAREN){
        currentNode->pushChild(numericalExpression());
        if(currentToken.type == TokenType::_COMMA && lexer.peekAtNext().type != TokenType::_RPAREN){
            eat(TokenType::_COMMA);
        }
    }
    return currentNode;
}

std::shared_ptr<ASTree> Parser::literal(){
    if(currentToken.value.back() == 'u'){
        return std::make_shared<ASTree>(ASTNodeType::LITERAL, Token(currentToken), Types::UNSIGNED);
    }
    return std::make_shared<ASTree>(ASTNodeType::LITERAL, Token(currentToken), Types::INT);
}