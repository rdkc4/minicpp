#include "intermediate_representation.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <format>

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// INTERMEDIATE REPRESENTATION TREE BUILDING
// simplifying AST for easier code generation
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::formIR(ASTree* astRoot){
    std::unique_ptr<IRTree> root = std::make_unique<IRTree>(IRNodeType::PROGRAM);
    for(const auto& child: astRoot->getChild(0)->getChildren()){
        root->pushChild(function(child.get()));
    }
    return root;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION NODE - parameters, constructs  
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::function(ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::FUNCTION, node->getToken().value, "", node->getType());
    
    variableCount = 0;

    iChild->pushChild(parameter(node->getChild(0)));

    for(const auto& child : node->getChild(1)->getChildren()){
        iChild->pushChild(construct(child.get()));
    }

    // bytes allocated for local variables
    std::string varCountStr = std::to_string(variableCount * 8);
    iChild->setValue(varCountStr);

    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// PARAMETER NODES - default values 0
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::parameter(ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::PARAMETER);
    for(const auto& child : node->getChildren()){
        iChild->pushChild(std::make_unique<IRTree>(IRNodeType::ID, std::string(child->getToken().value), "0", child->getType()));
    }
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTRUCT - variable / statement
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::construct(ASTree* node){
    if(node->getNodeType() == ASTNodeType::VARIABLE){
        return variable(node);
    }
    else{
        return statement(node);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLE NODES - default values 0
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::variable(ASTree* node){
    std::unique_ptr<IRTree> variable = std::make_unique<IRTree>(IRNodeType::VARIABLE, std::string(node->getToken().value), "0", node->getType());
    if(node->getChildren().size() != 0){
        variable->pushChild(assignmentStatement(node->getChild(0)));
    }
    ++variableCount;

    return variable;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// STATEMENT NODE TYPES
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::statement(ASTree* node){
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
            throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> Invalid statement '{}'\n", 
                node->getToken().line, node->getToken().column, astNodeTypeToString.at(node->getNodeType())));
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// IF NODE - relexp, if constructs, relexp else if constructs (optional),  else constructs (optional)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::ifStatement(ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::IF);
    for(const auto& child : node->getChildren()){
        if(child->getNodeType() == ASTNodeType::RELATIONAL_EXPRESSION){
            iChild->pushChild(relationalExpression(child.get()));
        }
        else{
            iChild->pushChild(construct(child.get()));
        }
    }
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// COMPOUND NODE - constructs
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::compoundStatement(ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::COMPOUND);
    for(const auto& child : node->getChildren()){
        iChild->pushChild(construct(child.get()));
    }
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ASSIGN NODE - destination variable, numexp
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::assignmentStatement(ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::ASSIGN);
    iChild->pushChild(id(node->getChild(0)));
    iChild->pushChild(numericalExpression(node->getChild(1)));
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETURN NODE - numexp
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::returnStatement(ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::RETURN);
    if(node->getChildren().size() != 0){
        iChild->pushChild(numericalExpression(node->getChild(0)));
    }
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// WHILE NODE - relexp, constructs
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::whileStatement(ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::WHILE);
    iChild->pushChild(relationalExpression(node->getChild(0)));
    iChild->pushChild(construct(node->getChild(1)));
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FOR NODE - assign, relexp, assign, constructs
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::forStatement(ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::FOR);
    iChild->pushChild(assignmentStatement(node->getChild(0)));
    iChild->pushChild(relationalExpression(node->getChild(1)));
    iChild->pushChild(assignmentStatement(node->getChild(2)));
    iChild->pushChild(construct(node->getChild(3)));

    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DO_WHILE NODE - constructs, relexp
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::doWhileStatement(ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::DO_WHILE);
    iChild->pushChild(construct(node->getChild(0)));
    iChild->pushChild(relationalExpression(node->getChild(1)));

    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SWITCH NODE - cases
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::switchStatement(ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::SWITCH);
    iChild->pushChild(id(node->getChild(0)));
    for(size_t i = 1; i < node->getChildren().size(); i++){
        ASTree* child = node->getChild(i);
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
// CASE NODE - literal, constructs, break(optional)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::_case(ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::CASE);
    iChild->pushChild(literal(node->getChild(0)));
    for(const auto& child : node->getChild(1)->getChildren()){
        iChild->pushChild(construct(child.get()));
    }
    if(node->getChildren().size() == 3){
        iChild->pushChild(_break());
    }
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DEFAULT NODE - constructs
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::_default(ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::DEFAULT);
    for(const auto& child : node->getChild(0)->getChildren()){
        iChild->pushChild(construct(child.get()));
    }
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// BREAK NODE
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::_break(){
    return std::make_unique<IRTree>(IRNodeType::BREAK);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// NUMEXP - reduced to (id, literal, function call) or arithmetic operation (add, sub, mul, div)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::numericalExpression(ASTree* node){
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
        std::unique_ptr<IRTree> lchild = numericalExpression(node->getChild(0));
        std::unique_ptr<IRTree> rchild = numericalExpression(node->getChild(1));

        if(lchild->getNodeType() == rchild->getNodeType() && lchild->getNodeType() == IRNodeType::LITERAL){
            if(lchild->getType() == Types::INT){
                return mergeLiterals<int>(std::move(lchild), std::move(rchild), node);
            }
            else if(lchild->getType() == Types::UNSIGNED){
                return mergeLiterals<unsigned>(std::move(lchild), std::move(rchild), node);
            }
        }
        
        std::string val = node->getToken().value;
        Types type = lchild->getType();
        IRNodeType iNodeType = arithmeticOperators.find(val) != arithmeticOperators.end() ? stringToArop.at(val) : stringToBitop.at(val)[type == Types::UNSIGNED];
        std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(iNodeType);
        
        iChild->setType(type);
        iChild->pushChild(std::move(lchild));
        iChild->pushChild(std::move(rchild));
        return iChild;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// MERGE LITERALS - simplify arithmetic operations if both nodes are literals
// helper function mergeValues
//-----------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
std::unique_ptr<IRTree> IntermediateRepresentation::mergeLiterals(std::unique_ptr<IRTree>&& lchild, std::unique_ptr<IRTree>&& rchild, ASTree* node){
    T lval = (std::is_same<T, int>::value ? std::stoi(lchild->getValue()) : std::stoul(lchild->getValue()));
    T rval = (std::is_same<T, int>::value ? std::stoi(rchild->getValue()) : std::stoul(rchild->getValue()));
    T result = mergeValues<T>(lval, rval,node);

    Types type = std::is_same<T, int>::value ? Types::INT : Types::UNSIGNED;
    std::string suffix = type == Types::INT ? "" : "u";

    return std::make_unique<IRTree>(IRNodeType::LITERAL, "", std::to_string(result) + suffix, type);
}

template<typename T>
T IntermediateRepresentation::mergeValues(T l, T r, ASTree* node){
    std::string op = node->getToken().value;
    if(op == "+") 
        return l + r;
    else if(op == "-") 
        return l - r;
    else if(op == "*") 
        return l * r;
    else if(op == "/"){
        if(r == 0){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> division by ZERO",
                node->getToken().line, node->getToken().column));
        }
        return l / r;
    }
    else if(op == "&") 
        return l & r;
    else if(op == "|") 
        return l | r;
    else if(op == "^") 
        return l ^ r;
    else if(op == "<<")
        return l << r;
    else if(op == ">>")
        return l >> r;
    else
        throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> Invalid arithmetic operator '{}'",
            node->getToken().line, node->getToken().column, op));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// CMP NODE - numexp, numexp 
// stores rel operator as value 
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::relationalExpression(ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::CMP);
    iChild->setValue(node->getToken().value);
    iChild->pushChild(numericalExpression(node->getChild(0)));
    iChild->pushChild(numericalExpression(node->getChild(1)));

    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ID NODE - default val 0
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::id(ASTree* node){
    return std::make_unique<IRTree>(IRNodeType::ID, node->getToken().value, "0", node->getType());
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// LITERAL NODE - already has value
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::literal(ASTree* node){
    return std::make_unique<IRTree>(IRNodeType::LITERAL, "", node->getToken().value, node->getType());
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION CALL - argument node
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::functionCall(ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::CALL, node->getToken().value, "", node->getType());
    iChild->pushChild(argument(node));
    return iChild;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ARGUMENT NODE - arguments
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::unique_ptr<IRTree> IntermediateRepresentation::argument(ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::ARGUMENT);
    for(const auto& child : node->getChild(0)->getChildren()){
        iChild->pushChild(numericalExpression(child.get()));
    }
    return iChild;
}
