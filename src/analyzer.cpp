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
        if(!scopeManager.pushSymbol(std::make_shared<Symbol>(child->getToken()->value, Kinds::FUN, type))){
            throw std::runtime_error("Line " + std::to_string(child->getToken()->line) + " Column " + std::to_string(child->getToken()->column)
                + ":SEMANTIC ERROR -> function redefined " + typeToString.at(type) + " " + child->getToken()->value);
        }
        functionCheck(child);
    }

    if(!scopeManager.getSymbolTable().lookupSymbol("main", {Kinds::FUN})){
        throw std::runtime_error("'main' function not found");
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
    
    auto returnType = scopeManager.getSymbolTable().getSymbol(activeFunction)->getType();
    if(returnType != Types::VOID && !returned){
        throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
            + ": SEMANTIC ERROR -> function " + typeToString.at(returnType) + " " + activeFunction + " returns " + typeToString.at(Types::VOID));
    }
    
    activeFunction = "";
    returned = false;
}

void Analyzer::parameterCheck(std::shared_ptr<ASTree> node){
    scopeManager.getSymbolTable().getSymbol(activeFunction)->setParameters(node);
    //scopeManager.printSymbolTable();
    for(const auto& child : node->getChildren()){
        auto type = child->getType().value();
        if(type == Types::VOID || type == Types::NO_TYPE){
            throw std::runtime_error("Line " + std::to_string(child->getToken()->line) + " Column " + std::to_string(child->getToken()->column)
                + ":SEMANTIC ERROR -> invalid type " + typeToString.at(type) + " " + child->getToken()->value);
        }
        if(!scopeManager.pushSymbol(std::make_shared<Symbol>(child->getToken()->value, Kinds::PAR, child->getType().value()))){
            throw std::runtime_error("Line " + std::to_string(child->getToken()->line) + " Column " + std::to_string(child->getToken()->column)
                + ": SEMANTIC ERROR -> redefined '" + child->getToken()->value + "'");
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
        if(!scopeManager.pushSymbol(std::make_shared<Symbol>(child->getToken()->value, Kinds::VAR, type))){
            throw std::runtime_error("Line " + std::to_string(child->getToken()->line) + " Column " + std::to_string(child->getToken()->column)
                + ": SEMANTIC ERROR -> redefined '" + child->getToken()->value + "'");
        }
    }
}

void Analyzer::checkStatements(std::shared_ptr<ASTree> node){
    for(const auto& child : node->getChildren()){
        checkStatement(child);
    }
}

void Analyzer::checkStatement(std::shared_ptr<ASTree> node){
    switch(node->getNodeType()){
        case ASTNodeType::IF_STATEMENT:
            checkIfStatement(node);
            break;
        case ASTNodeType::WHILE_STATEMENT:
            checkWhileStatement(node);
            break;
        case ASTNodeType::FOR_STATEMENT:
            checkForStatement(node);
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
        case ASTNodeType::DO_WHILE_STATEMENT:
            checkDoWhileStatement(node);
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

void Analyzer::checkWhileStatement(std::shared_ptr<ASTree> node){
    checkRelationalExpression(node->getChild(0));
    checkStatement(node->getChild(1));
}

void Analyzer::checkForStatement(std::shared_ptr<ASTree> node){
    checkAssignmentStatement(node->getChild(0));
    checkRelationalExpression(node->getChild(1));
    checkAssignmentStatement(node->getChild(2));

    auto lname = node->getChild(0)->getChild(1)->getToken()->value;
    auto rname = node->getChild(2)->getChild(1)->getToken()->value;
    
    if(lname != rname){
        throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
            + ": SEMANTIC ERROR -> variable mismatch '" + lname + "' ! '" + rname + "'");
    }

    checkStatement(node->getChild(3));
}

void Analyzer::checkDoWhileStatement(std::shared_ptr<ASTree> node){
    checkStatement(node->getChild(0));
    checkRelationalExpression(node->getChild(1));
}

void Analyzer::checkCompoundStatement(std::shared_ptr<ASTree> node){
    checkStatements(node->getChild(0));
}

void Analyzer::checkAssignmentStatement(std::shared_ptr<ASTree> node){
    auto lchild = node->getChild(0);
    auto rchild = node->getChild(1);
    
    checkNumericalExpression(lchild);
    Types ltype = lchild->getType().value();
    
    checkID(rchild);
    Types rtype = scopeManager.getSymbolTable().getSymbol(rchild->getToken()->value)->getType();
    
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
    auto returnType = scopeManager.getSymbolTable().getSymbol(activeFunction)->getType();
    if(returnType != type){
        throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
            + ": SEMANTIC ERROR -> " + typeToString.at(returnType) + " " + activeFunction + " returns " + typeToString.at(type));
    }
    returned = true;
}

void Analyzer::checkNumericalExpression(std::shared_ptr<ASTree> node){
    node->setType(getNumericalExpressionType(node));
}

Types Analyzer::getNumericalExpressionType(std::shared_ptr<ASTree> node){
    if(node->getNodeType() == ASTNodeType::ID){
        auto type = scopeManager.getSymbolTable().getSymbol(node->getToken()->value)->getType();
        node->setType(type);
        return type;
    }
    else if(node->getNodeType() == ASTNodeType::LITERAL){
        checkLiteral(node);
        return node->getType().value();
    }
    else if(node->getNodeType() == ASTNodeType::FUNCTION_CALL){
        checkFunctionCall(node);
        return node->getType().value();
    }
    if(node->getChildren().size() == 1){
        return getNumericalExpressionType(node->getChild(0));
    }else{
        auto ltype = getNumericalExpressionType(node->getChild(0));
        auto rtype = getNumericalExpressionType(node->getChild(1));
        if(ltype != rtype){
            throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
                + ": SEMANTIC ERROR -> type mismatch near " + node->getToken()->value);
        }
        
        return ltype;
    }
}

void Analyzer::checkRelationalExpression(std::shared_ptr<ASTree> node){
    auto lchild = node->getChild(0);
    auto rchild = node->getChild(1);

    checkNumericalExpression(lchild);
    auto ltype = lchild->getType().value();
    checkNumericalExpression(rchild);
    auto rtype = rchild->getType().value();
    
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
    node->setType(scopeManager.getSymbolTable().getSymbol(node->getToken()->value)->getType());
}

void Analyzer::checkLiteral(std::shared_ptr<ASTree> node){
    auto name = node->getToken()->value;
    if(node->getType().value() == Types::UNSIGNED && (name.back() != 'u' || name.front() == '-')){
        throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
            + ":SEMANTIC ERROR -> invalid literal " + name);
    }
    else if(node->getType().value() == Types::INT && name.back() == 'u'){
        throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
            + ":SEMANTIC ERROR -> invalid literal " + name);
    }
}

void Analyzer::checkFunctionCall(std::shared_ptr<ASTree> node){
    if(!scopeManager.getSymbolTable().lookupSymbol(node->getToken()->value, {Kinds::FUN})){
        throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
            + ":SEMANTIC ERROR -> undefined function " + node->getToken()->value);
    }

    if(node->getChild(0)->getChildren().size() != scopeManager.getSymbolTable().getSymbol(node->getToken()->value)->getParameters()->getChildren().size()){
        throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
            + ":SEMANTIC ERROR -> invalid function call " + node->getToken()->value);
    }
    node->setType(scopeManager.getSymbolTable().getSymbol(node->getToken()->value)->getType());
    checkArgument(node);
}

void Analyzer::checkArgument(std::shared_ptr<ASTree> node){
    auto functionParameters = scopeManager.getSymbolTable().getSymbol(node->getToken()->value)->getParameters();
    auto arguments = node->getChild(0);
    for(size_t i = 0; i < arguments->getChildren().size(); i++){
        checkNumericalExpression(arguments->getChild(i));
        if(arguments->getChild(i)->getType().value() != functionParameters->getChild(i)->getType().value()){
            throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
                + ": SEMANTIC ERROR -> type mismatch " + node->getToken()->value);
        }
    }
}