#include "intermediate_representation.hpp"

#include <stdexcept>

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// INTERMEDIATE REPRESENTATION TREE BUILDING
// simplifying AST for easier code generation
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void IntermediateRepresentation::formIR(std::shared_ptr<ASTree> astRoot){
    root = std::make_shared<IRTree>(IRNodeType::PROGRAM);
    for(const auto& child: astRoot->getChild(0)->getChildren()){
        root->pushChild(function(child));
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// GET ENTRY POINT OF IR TREE
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::getRoot() const{
    return root;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION NODE - PARAMETERS, VARIABLE, STATEMENTS (on same level)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::function(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::FUNCTION, node->getToken()->value, "", node->getType().value());

    iChild->pushChild(parameter(node->getChild(0)));

    iChild->pushChild(variable(node->getChild(1)->getChild(0)));

    for(const auto& child : node->getChild(1)->getChild(1)->getChildren()){
        iChild->pushChild(statement(child));
    }

    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// PARAMETER NODES - default values 0
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::parameter(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::PARAMETER);
    for(const auto& child : node->getChildren()){
        iChild->pushChild(std::make_shared<IRTree>(IRNodeType::ID, std::string(child->getToken()->value), "0", child->getType().value()));
    }
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLE NODES - default values 0
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::variable(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::VARIABLE);
    for(const auto& child : node->getChildren()){
        iChild->pushChild(std::make_shared<IRTree>(IRNodeType::ID, std::string(child->getToken()->value), "0", child->getType().value()));
    }
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// STATEMENT NODE TYPES
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::statement(std::shared_ptr<ASTree> node){
    switch(node->getNodeType()){
        case ASTNodeType::IF_STATEMENT:
            return ifStatement(node);
        case ASTNodeType::COMPOUND_STATEMENT:
            return compoundStatement(node);
        case ASTNodeType::ASSIGNMENT_STATEMENT:
            return assignmentStatement(node);
        case ASTNodeType::RETURN_STATEMENT:
            return returnStatement(node);
        case ASTNodeType::WHILE_STATEMENT:
            return whileStatement(node);
        case ASTNodeType::FOR_STATEMENT:
            return forStatement(node);
        case ASTNodeType::DO_WHILE_STATEMENT:
            return doWhileStatement(node);
        case ASTNodeType::SWITCH_STATEMENT:
            return switchStatement(node);
        default:
            throw std::runtime_error("Invalid statement\n");
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// IF NODE - relexp, if statement, relexp else if statements (optional),  else statement(optional)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::ifStatement(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::IF);
    for(const auto& child : node->getChildren()){
        if(child->getNodeType() == ASTNodeType::RELATIONAL_EXPRESSION){
            iChild->pushChild(relationalExpression(child));
        }
        else{
            iChild->pushChild(statement(child));
        }
    }
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// COMPOUND NODE - statements
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::compoundStatement(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::COMPOUND);
    for(const auto& child : node->getChild(0)->getChildren()){
        iChild->pushChild(statement(child));
    }
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ASSIGN NODE - destination variable, numexp
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::assignmentStatement(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::ASSIGN);
    iChild->pushChild(id(node->getChild(0)));
    iChild->pushChild(numericalExpression(node->getChild(1)));
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETURN NODE - numexp
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::returnStatement(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::RETURN);
    if(node->getChildren().size() != 0){
        iChild->pushChild(numericalExpression(node->getChild(0)));
    }
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// WHILE NODE - relexp, statements
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::whileStatement(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::WHILE);
    iChild->pushChild(relationalExpression(node->getChild(0)));
    iChild->pushChild(statement(node->getChild(1)));
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FOR NODE - assign, relexp, assign, statement
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::forStatement(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::FOR);
    iChild->pushChild(assignmentStatement(node->getChild(0)));
    iChild->pushChild(relationalExpression(node->getChild(1)));
    iChild->pushChild(assignmentStatement(node->getChild(2)));
    iChild->pushChild(statement(node->getChild(3)));

    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DO_WHILE NODE - statement, relexp
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::doWhileStatement(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::DO_WHILE);
    iChild->pushChild(statement(node->getChild(0)));
    iChild->pushChild(relationalExpression(node->getChild(1)));

    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SWITCH NODE - cases
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::switchStatement(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::SWITCH);
    iChild->pushChild(id(node->getChild(0)));
    size_t i = 1;
    for(; i < node->getChildren().size(); i++){
        auto child = node->getChild(i);
        if(child->getNodeType() == ASTNodeType::CASE){
            iChild->pushChild(_case(child));
        }
        else{
            iChild->pushChild(_default(child));
        }
    }
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// CASE NODE - literal, statements, break(optional)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::_case(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::CASE);
    iChild->pushChild(literal(node->getChild(0)));
    for(const auto& child : node->getChild(1)->getChildren()){
        iChild->pushChild(statement(child));
    }
    if(node->getChildren().size() == 3){
        iChild->pushChild(_break());
    }
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DEFAULT NODE - statements
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::_default(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::DEFAULT);
    for(const auto& child : node->getChild(0)->getChildren()){
        iChild->pushChild(statement(child));
    }
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// BREAK NODE
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::_break(){
    return std::make_shared<IRTree>(IRNodeType::BREAK);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// NUMEXP - reduced to (id, literal, function call) or arithmetic operation (add, sub, mul, div)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::numericalExpression(std::shared_ptr<ASTree> node){
    if(node->getNodeType() == ASTNodeType::ID){
        return id(node);
    }
    else if(node->getNodeType() == ASTNodeType::LITERAL){
        return literal(node);
    }
    else if(node->getNodeType() == ASTNodeType::FUNCTION_CALL){
        return functionCall(node);
    }
    if(node->getChildren().size() == 1){
        return numericalExpression(node->getChild(1));
    }
    else{
        auto lchild = numericalExpression(node->getChild(0));
        auto rchild = numericalExpression(node->getChild(1));

        if(lchild->getNodeType() == rchild->getNodeType() && lchild->getNodeType() == IRNodeType::LITERAL){
            return mergeLiterals(lchild, rchild, node->getToken()->value);
        }
        
        auto val = node->getToken()->value;
        auto type = lchild->getType().value();
        IRNodeType iNodeType = arithmeticOperators.find(val) != arithmeticOperators.end() ? stringToArop.at(val) : stringToBitop.at(val)[type == Types::UNSIGNED];
        auto iChild = std::make_shared<IRTree>(iNodeType);
        
        iChild->setType(type);
        iChild->pushChild(lchild);
        iChild->pushChild(rchild);
        return iChild;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// MERGE LITERALS - simplify arithmetic operations if both nodes are literals
// helper function mergeValues
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::mergeLiterals(std::shared_ptr<IRTree> lchild, std::shared_ptr<IRTree> rchild, std::string& op){
    if(lchild->getType().value() == Types::INT){
        int lval = std::stoi(lchild->getValue());
        int rval = std::stoi(rchild->getValue());
        int result = mergeValues<int>(lval, rval, op);
        return std::make_shared<IRTree>(IRNodeType::LITERAL, "", std::to_string(result), Types::INT);
    }
    else{
        unsigned lval = std::stoul(lchild->getValue());
        unsigned rval = std::stoul(rchild->getValue());
        unsigned result = mergeValues<unsigned>(lval, rval, op);
        return std::make_shared<IRTree>(IRNodeType::LITERAL, "", std::to_string(result)+"u", Types::UNSIGNED);
    }
}

template<typename T>
T IntermediateRepresentation::mergeValues(T l, T r, std::string& op){
    if(op == "+") 
        return l + r;
    else if(op == "-") 
        return l - r;
    else if(op == "*") 
        return l * r;
    else if(op == "/"){
        if(r == 0){
            throw std::runtime_error("SEMANTIC ERROR - division by ZERO");
        }
        return l / r;
    }
    else if(op == "&") 
        return l & r;
    else if(op == "|") 
        return l | r;
    else if(op == "^") 
        return l ^ r;
    else
        throw std::runtime_error("Invalid arithmetic operator");
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// CMP NODE - numexp, numexp 
// stores rel operator as value 
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::relationalExpression(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::CMP);
    iChild->setValue(node->getToken()->value);
    iChild->pushChild(numericalExpression(node->getChild(0)));
    iChild->pushChild(numericalExpression(node->getChild(1)));

    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ID NODE - default val 0
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::id(std::shared_ptr<ASTree> node){
    return std::make_shared<IRTree>(IRNodeType::ID, node->getToken()->value, "0", node->getType().value());
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// LITERAL NODE - already has value
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::literal(std::shared_ptr<ASTree> node){
    return std::make_shared<IRTree>(IRNodeType::LITERAL, "", node->getToken()->value, node->getType().value());
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION CALL - argument node
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::functionCall(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::CALL, node->getToken()->value, "", node->getType().value());
    iChild->pushChild(argument(node));
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ARGUMENT NODE - arguments
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IntermediateRepresentation::argument(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::ARGUMENT);
    for(const auto& child : node->getChild(0)->getChildren()){
        iChild->pushChild(numericalExpression(child));
    }
    return iChild;
}