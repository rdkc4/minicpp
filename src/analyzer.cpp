#include "../include/analyzer.hpp"

Analyzer::Analyzer(ScopeManager& scopeManager) : scopeManager(scopeManager), activeFunction(""), returned(false){}

void Analyzer::semanticCheck(std::shared_ptr<ASTree> root){
    auto flist = root;
    flist = flist->getChildren().back();
    scopeManager.pushScope();

    for(const auto& child : flist->getChildren()){
        auto type = child->getType().value();
        if(type == Types::NO_TYPE){
            throw std::runtime_error("Line " + std::to_string(child->getToken()->line) + " Column " + std::to_string(child->getToken()->column)
                + ":SEMANTIC ERROR -> invalid type " + typeToString.at(type) + " " + child->getToken()->value);
        }
        scopeManager.pushSymbol(Symbol(child->getToken()->value, Kinds::FUN, type, 0, 0));
    }

    if(!scopeManager.getSymbolTable().lookupSymbol("main", {Kinds::FUN})){
        throw std::runtime_error("'main' function not found");
    }
    for(const auto& child : flist->getChildren()){
        functionCheck(child);
    }
    scopeManager.popScope();
}

void Analyzer::functionCheck(std::shared_ptr<ASTree> node){
    activeFunction = node->getToken()->value;
    returned = false;

    scopeManager.pushScope();
    parameterCheck(node->getChild(0));
    bodyCheck(node->getChild(1));
    scopeManager.popScope();

    activeFunction = "";
    returned = false;
}

void Analyzer::parameterCheck(std::shared_ptr<ASTree> node){
    for(const auto& child : node->getChildren()){
        auto type = child->getType().value();
        if(type == Types::VOID || type == Types::NO_TYPE){
            throw std::runtime_error("Line " + std::to_string(child->getToken()->line) + " Column " + std::to_string(child->getToken()->column)
                + ":SEMANTIC ERROR -> invalid type " + typeToString.at(type) + " " + child->getToken()->value);
        }
        if(!scopeManager.pushSymbol(Symbol(child->getToken()->value, Kinds::PAR, child->getType().value(), 0, 0))){
            throw std::runtime_error("Line " + std::to_string(child->getToken()->line) + " Column " + std::to_string(child->getToken()->column)
                + ": SEMANTIC ERROR -> redefined variable " + child->getToken()->value);
        }
    }
}

void Analyzer::bodyCheck(std::shared_ptr<ASTree> node){
    checkVariables(node->getChild(0));
    checkStatements(node->getChild(1));
}

void Analyzer::checkVariables(std::shared_ptr<ASTree> node){
    for(const auto& child : node->getChildren()){
        auto type = child->getType().value();
        if(type == Types::VOID || type == Types::NO_TYPE){
            throw std::runtime_error("Line " + std::to_string(child->getToken()->line) + " Column " + std::to_string(child->getToken()->column)
                + ":SEMANTIC ERROR -> invalid type " + typeToString.at(type) + " " + child->getToken()->value);
        }
        if(!scopeManager.pushSymbol(Symbol(child->getToken()->value, Kinds::VAR, type))){
            throw std::runtime_error("Line " + std::to_string(child->getToken()->line) + " Column " + std::to_string(child->getToken()->column)
                + ": SEMANTIC ERROR -> redefined variable " + child->getToken()->value);
        }
    }
}

void Analyzer::checkStatements(std::shared_ptr<ASTree> node){
    for(const auto& child : node->getChildren()){
       checkStatement(child);
       child->getChild(1);
    }
}

void Analyzer::checkStatement(std::shared_ptr<ASTree> node){
    switch(node->getNodeType()){
        case ASTNodeType::IF_STATEMENT:
            checkIfStatement(node);
            break;
        case ASTNodeType::ASSIGNMENT_STATEMENT:
            checkAssignmentStatement(node);
            break;
        case ASTNodeType::COMPOUND_STATEMENT:
            checkCompoundStatement(node);
            break;
        case ASTNodeType::RETURN_STATEMENT:
            checkReturnStatement(node);
            break;
        default:
            return;
    }
}

void Analyzer::checkIfStatement(std::shared_ptr<ASTree> node){
    checkRelationalExpression(node->getChild(0));
    for(size_t i = 1; i < node->getChildren().size(); i++){
        checkStatement(node->getChild(i));
    }
}

void Analyzer::checkCompoundStatement(std::shared_ptr<ASTree> node){
    checkStatements(node->getChild(0));
}

void Analyzer::checkAssignmentStatement(std::shared_ptr<ASTree> node){
    auto lchild = node->getChild(0);
    auto rchild = node->getChild(1);
    Types ltype;
    switch(lchild->getNodeType()){
        case ASTNodeType::NUMERICAL_EXPRESSION:
            checkNumericalExpression(lchild);
            ltype = lchild->getType().value();
            break;
        case ASTNodeType::ID:
            checkID(lchild);
            ltype = scopeManager.getSymbolTable().getSymbol(lchild->getToken()->value).getType();
            break;
        case ASTNodeType::LITERAL:
            ltype = Types::INT; //temporary
            break;
        default:
            break;
    }
    checkID(rchild);
    Types rtype = scopeManager.getSymbolTable().getSymbol(rchild->getToken()->value).getType();
    if(rtype != ltype){
        throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
            + ": SEMANTIC ERROR -> type mismatch near " + node->getToken()->value);
    }
}

void Analyzer::checkReturnStatement(std::shared_ptr<ASTree> node){
    Types type = Types::NO_TYPE;
    if(node->getChildren().empty()){
        type = Types::VOID;
    }
    else{
        type = getNumericalExpressionType(node->getChild(0));
    }
    auto returnType = scopeManager.getSymbolTable().getSymbol(activeFunction).getType();
    if(returnType != type){
        throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
            + ": SEMANTIC ERROR -> " + typeToString.at(returnType) + " " + activeFunction + " returns " + typeToString.at(type));
    } 
}

void Analyzer::checkNumericalExpression(std::shared_ptr<ASTree> node){
    node->setType(getNumericalExpressionType(node));
}

Types Analyzer::getNumericalExpressionType(std::shared_ptr<ASTree> node){
    if(node->getNodeType() == ASTNodeType::ID){
        return scopeManager.getSymbolTable().getSymbol(node->getToken()->value).getType();
    }
    else if(node->getNodeType() == ASTNodeType::LITERAL){
        return Types::INT; //temporary
    }
    else if(node->getNodeType() == ASTNodeType::FUNCTION_CALL){
        checkFunctionCall(node);
        return node->getType().value();
    }
    if(node->getChildren().size() == 1){
        return getNumericalExpressionType(node->getChild(0));
    }else{
        auto lchild = node->getChild(0);
        auto rchild = node->getChild(1);
        if(getNumericalExpressionType(lchild) != getNumericalExpressionType(rchild)){
            throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
                + ": SEMANTIC ERROR -> type mismatch near " + node->getToken()->value);
        }
        return scopeManager.getSymbolTable().getSymbol(lchild->getToken()->value).getType();
    }
}

void Analyzer::checkRelationalExpression(std::shared_ptr<ASTree> node){
    auto lchild = node->getChild(0);
    auto rchild = node->getChild(1);
    if(lchild->getToken()->type == TokenType::_ID){
        checkID(lchild);
    }
    if(rchild->getToken()->type == TokenType::_ID){
        checkID(rchild);
    }
    auto ltype = scopeManager.getSymbolTable().getSymbol(lchild->getToken()->value).getType();
    auto rtype = scopeManager.getSymbolTable().getSymbol(rchild->getToken()->value).getType();
    if(ltype != rtype){
        throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
            + ": SEMANTIC ERROR -> type mismatch " + node->getToken()->value);
    }
}

void Analyzer::checkID(std::shared_ptr<ASTree> node){
    auto name = node->getToken()->value;
    if(!scopeManager.getSymbolTable().lookupSymbol(name, {Kinds::VAR, Kinds::PAR})){
        throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
            + ":SEMANTIC ERROR -> undefined variable " + name);
    }
}

void Analyzer::checkFunctionCall(std::shared_ptr<ASTree> node){
    if(!scopeManager.getSymbolTable().lookupSymbol(node->getToken()->value, {Kinds::FUN})){
        throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
            + ":SEMANTIC ERROR -> undefined function " + node->getToken()->value);
    }
    node->setType(scopeManager.getSymbolTable().getSymbol(node->getToken()->value).getType());
    checkArgument(node->getChild(0));
}

void Analyzer::checkArgument(std::shared_ptr<ASTree> node){
    for(const auto& child : node->getChildren()){
        checkNumericalExpression(child);
    }
}