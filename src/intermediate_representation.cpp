#include "../include/intermediate_representation.hpp"

void IntermediateRepresentation::formIR(std::shared_ptr<ASTree> astRoot){
    root = std::make_shared<IRTree>(IRNodeType::PROGRAM);
    for(const auto& child: astRoot->getChild(0)->getChildren()){
        root->pushChild(function(child));
    }
}

std::shared_ptr<IRTree> IntermediateRepresentation::getRoot() const{
    return root;
}

std::shared_ptr<IRTree> IntermediateRepresentation::function(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::FUNCTION, node->getToken()->value, "", node->getType().value());

    for(const auto& child : node->getChild(0)->getChildren()){
        iChild->pushChild(parameter(child));
    }

    for(const auto& child : node->getChild(1)->getChild(0)->getChildren()){
        iChild->pushChild(variable(child));
    }

    for(const auto& child : node->getChild(1)->getChild(1)->getChildren()){
        iChild->pushChild(statement(child));
    }

    return iChild;
}

std::shared_ptr<IRTree> IntermediateRepresentation::parameter(std::shared_ptr<ASTree> node){
    return std::make_shared<IRTree>(IRNodeType::PARAMETER, std::string(node->getToken()->value), "", node->getType().value());
}

std::shared_ptr<IRTree> IntermediateRepresentation::variable(std::shared_ptr<ASTree> node){
    return std::make_shared<IRTree>(IRNodeType::VARIABLE, std::string(node->getToken()->value), "", node->getType().value());
}

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
        default:
            throw std::runtime_error("Invalid statement\n");
    }
}

std::shared_ptr<IRTree> IntermediateRepresentation::ifStatement(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::IF);
    iChild->pushChild(relationalExpression(node->getChild(0)));
    for(size_t i = 1; i < node->getChildren().size(); i++){
        iChild->pushChild(statement(node->getChild(i)));
    }
    return iChild;
}

std::shared_ptr<IRTree> IntermediateRepresentation::compoundStatement(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::COMPOUND);
    for(const auto& child : node->getChild(0)->getChildren()){
        iChild->pushChild(statement(child));
    }
    return iChild;
}

std::shared_ptr<IRTree> IntermediateRepresentation::assignmentStatement(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::ASSIGN);
    iChild->pushChild(id(node->getChild(1)));
    iChild->pushChild(numericalExpression(node->getChild(0)));
    return iChild;
}

std::shared_ptr<IRTree> IntermediateRepresentation::returnStatement(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::RETURN);
    if(node->getChildren().size() != 0){
        iChild->pushChild(numericalExpression(node->getChild(0)));
    }
    return iChild;
}

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

        auto iChild = std::make_shared<IRTree>(stringToArop.at(node->getToken()->value));
        iChild->pushChild(lchild);
        iChild->pushChild(rchild);
        return iChild;
    }
}

std::shared_ptr<IRTree> IntermediateRepresentation::mergeLiterals(std::shared_ptr<IRTree> lchild, std::shared_ptr<IRTree> rchild, std::string& arop){
    if(lchild->getType().value() == Types::INT){
        int lval = std::stoi(lchild->getValue().value());
        int rval = std::stoi(rchild->getValue().value());
        int result = mergeValues<int>(lval, rval, arop);
        return std::make_shared<IRTree>(IRNodeType::LITERAL, "", std::to_string(result), Types::INT);
    }
    else{
        unsigned lval = std::stoul(lchild->getValue().value());
        unsigned rval = std::stoul(rchild->getValue().value());
        unsigned result = mergeValues<unsigned>(lval, rval, arop);
        return std::make_shared<IRTree>(IRNodeType::LITERAL, "", std::to_string(result), Types::UNSIGNED);
    }
}

template<typename T>
T IntermediateRepresentation::mergeValues(T l, T r, std::string& arop){
    if(arop == "+"){
        return l + r;
    }
    else if(arop == "-"){ //arop == "-"
        if(r > l && std::is_unsigned<T>::value){
            throw std::runtime_error("Invalid operation");
        }
        return l - r;
    }
    else{
        throw std::runtime_error("Invalid arithmetic operator");
    }
}

std::shared_ptr<IRTree> IntermediateRepresentation::relationalExpression(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::CMP);
    iChild->setValue(node->getToken()->value);
    iChild->pushChild(numericalExpression(node->getChild(0)));
    iChild->pushChild(numericalExpression(node->getChild(1)));

    return iChild;
}

std::shared_ptr<IRTree> IntermediateRepresentation::id(std::shared_ptr<ASTree> node){
    return std::make_shared<IRTree>(IRNodeType::VARIABLE, node->getToken()->value, "0", node->getType().value());
}

std::shared_ptr<IRTree> IntermediateRepresentation::literal(std::shared_ptr<ASTree> node){
    return std::make_shared<IRTree>(IRNodeType::LITERAL, "", node->getToken()->value, node->getType().value());
}

std::shared_ptr<IRTree> IntermediateRepresentation::functionCall(std::shared_ptr<ASTree> node){
    auto iChild = std::make_shared<IRTree>(IRNodeType::CALL, node->getToken()->value, "", node->getType().value());
    for(const auto& child : node->getChild(0)->getChildren()){
        iChild->pushChild(argument(child));
    }
    return iChild;
}

std::shared_ptr<IRTree> IntermediateRepresentation::argument(std::shared_ptr<ASTree> node){
    return numericalExpression(node);
}