#include "intermediate_representation.hpp"

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <exception>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <format>
#include <thread>

#include "../thread-pool/thread_pool.hpp"
#include "defs/ir_defs.hpp"

IntermediateRepresentation::IntermediateRepresentation() = default;

thread_local IRThreadContext IntermediateRepresentation::irContext;

std::unique_ptr<IRTree> IntermediateRepresentation::formIR(const ASTree* astRoot){
    std::unique_ptr<IRTree> root = std::make_unique<IRTree>(IRNodeType::PROGRAM);
    const ASTree* astFlist = astRoot->getChild(0);
    
    size_t total = astFlist->getChildren().size();
    root->resizeChildren(total);

    std::atomic<size_t> done{};
    std::condition_variable doneCv;
    std::mutex doneMtx;

    ThreadPool threadPool{ std::thread::hardware_concurrency() };
    std::mutex irMtx;

    std::vector<std::string> exceptions;
    std::mutex exceptionMtx;

    for(size_t i = 0; i < total; ++i){
        threadPool.enqueue([&, root=root.get(), i, child=astFlist->getChild(i)]{
            
            try{
                // generating ir of a function
                std::unique_ptr<IRTree> irFunc = function(child);

                {
                    // setting function on it's position
                    std::lock_guard<std::mutex> lock(irMtx);
                    root->setChild(std::move(irFunc), i);
                }
            }
            catch(std::exception& ex){
                std::lock_guard<std::mutex> lock(exceptionMtx);
                exceptions.push_back(ex.what());
            }

            // check if all ir functions are generated
            if(done.fetch_add(1) + 1 == total){
                doneCv.notify_all();
            }
        });
    }

    {
        // wait untill all ir functions are generated
        std::unique_lock<std::mutex> lock(doneMtx);
        doneCv.wait(lock, [&]{ return done == total; });
    }

    // check for exceptions, currently not in order and only 1 per function
    if(!exceptions.empty()){
        std::string err{ ""};
        for(const auto& ex : exceptions){
            err += ex + "\n";
        }
        throw std::runtime_error(err);
    }

    root->traverse(1);
    return root;
}

std::unique_ptr<IRTree> IntermediateRepresentation::function(const ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(node->getToken().value, "", node->getType(), IRNodeType::FUNCTION);
    
    irContext.init();

    iChild->pushChild(parameter(node->getChild(0)));

    for(const auto& child : node->getChild(1)->getChildren()){
        iChild->pushChild(construct(child.get()));
    }

    // bytes allocated for local variables
    std::string varCountStr{ std::to_string(irContext.requiredMemory(regSize)) };
    iChild->setValue(varCountStr);

    return iChild;
}

std::unique_ptr<IRTree> IntermediateRepresentation::parameter(const ASTree* node) const {
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::PARAMETER);
    for(const auto& child : node->getChildren()){
        iChild->pushChild(std::make_unique<IRTree>(std::string{child->getToken().value}, "0", child->getType(), IRNodeType::ID));
    }
    return iChild;
}

std::unique_ptr<IRTree> IntermediateRepresentation::construct(const ASTree* node){
    if(node->getNodeType() == ASTNodeType::VARIABLE){
        return variable(node);
    }
    else{
        return statement(node);
    }
}

std::unique_ptr<IRTree> IntermediateRepresentation::variable(const ASTree* node){
    std::unique_ptr<IRTree> variable = std::make_unique<IRTree>(std::string{node->getToken().value}, "0", node->getType(), IRNodeType::VARIABLE);
    if(node->getChildren().size() != 0){
        variable->pushChild(assignmentStatement(node->getChild(0)));
    }
    ++irContext.variableCount;

    return variable;
}

std::unique_ptr<IRTree> IntermediateRepresentation::statement(const ASTree* node){
    switch(node->getNodeType()){
        case ASTNodeType::PRINTF:
            return printfStatement(node);
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
            // unreachable, added just to surpress warnings
            throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> Invalid statement '{}'\n", 
                node->getToken().line, node->getToken().column, astNodeTypeToString.at(node->getNodeType())));
    }
}

std::unique_ptr<IRTree> IntermediateRepresentation::printfStatement(const ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::PRINTF);

    // extracting function calls to temporary variables
    auto temps{ initiateTemporaries(node->getChild(0)) };
    if(temps){
        iChild->pushChild(std::move(temps));
    }
    iChild->pushChild(numericalExpression(node->getChild(0)));

    return iChild;
}

std::unique_ptr<IRTree> IntermediateRepresentation::ifStatement(const ASTree* node){
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

std::unique_ptr<IRTree> IntermediateRepresentation::compoundStatement(const ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::COMPOUND);
    for(const auto& child : node->getChildren()){
        iChild->pushChild(construct(child.get()));
    }
    return iChild;
}

std::unique_ptr<IRTree> IntermediateRepresentation::assignmentStatement(const ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::ASSIGN);

    // extracting function calls to temporary variables
    auto temps{ initiateTemporaries(node->getChild(1)) };
    if(temps){
        iChild->pushChild(std::move(temps));
    }
    
    iChild->pushChild(id(node->getChild(0)));
    iChild->pushChild(numericalExpression(node->getChild(1)));
    return iChild;
}

std::unique_ptr<IRTree> IntermediateRepresentation::returnStatement(const ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::RETURN);
    if(node->getChildren().size() != 0){
        // extracting function calls to temporary variables
        auto temps{ initiateTemporaries(node->getChild(0)) };
        if(temps){
            iChild->pushChild(std::move(temps));
        }

        iChild->pushChild(numericalExpression(node->getChild(0)));
    }
    return iChild;
}

std::unique_ptr<IRTree> IntermediateRepresentation::whileStatement(const ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::WHILE);
    iChild->pushChild(relationalExpression(node->getChild(0)));
    iChild->pushChild(construct(node->getChild(1)));
    return iChild;
}

std::unique_ptr<IRTree> IntermediateRepresentation::forStatement(const ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::FOR);
    iChild->pushChild(assignmentStatement(node->getChild(0)));
    iChild->pushChild(relationalExpression(node->getChild(1)));
    iChild->pushChild(assignmentStatement(node->getChild(2)));
    iChild->pushChild(construct(node->getChild(3)));

    return iChild;
}

std::unique_ptr<IRTree> IntermediateRepresentation::doWhileStatement(const ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::DO_WHILE);
    iChild->pushChild(construct(node->getChild(0)));
    iChild->pushChild(relationalExpression(node->getChild(1)));

    return iChild;
}

std::unique_ptr<IRTree> IntermediateRepresentation::switchStatement(const ASTree* node){
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

std::unique_ptr<IRTree> IntermediateRepresentation::_case(const ASTree* node){
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

std::unique_ptr<IRTree> IntermediateRepresentation::_default(const ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::DEFAULT);
    for(const auto& child : node->getChild(0)->getChildren()){
        iChild->pushChild(construct(child.get()));
    }
    return iChild;
}

std::unique_ptr<IRTree> IntermediateRepresentation::_break() const {
    return std::make_unique<IRTree>(IRNodeType::BREAK);
}

// NUMEXP - reduced to (id, literal, function call) or arithmetic operation (add, sub, mul, div)
std::unique_ptr<IRTree> IntermediateRepresentation::numericalExpression(const ASTree* node){
    if(node->getNodeType() == ASTNodeType::ID){
        return id(node);
    }
    else if(node->getNodeType() == ASTNodeType::LITERAL){
        return literal(node);
    }
    else if(node->getNodeType() == ASTNodeType::FUNCTION_CALL){
        return replaceFunctionCall(node);
    }
    if(node->getChildren().size() == 1){
        return numericalExpression(node->getChild(0));
    }
    else{
        std::unique_ptr<IRTree> lchild{ numericalExpression(node->getChild(0)) };
        std::unique_ptr<IRTree> rchild{ numericalExpression(node->getChild(1)) };

        if(lchild->getNodeType() == rchild->getNodeType() && lchild->getNodeType() == IRNodeType::LITERAL){
            if(lchild->getType() == Types::INT){
                return mergeLiterals<int>(std::move(lchild), std::move(rchild), node);
            }
            else if(lchild->getType() == Types::UNSIGNED){
                return mergeLiterals<unsigned>(std::move(lchild), std::move(rchild), node);
            }
        }
        
        std::string val{ node->getToken().value };
        Types type{ lchild->getType() };
        IRNodeType iNodeType{ arithmeticOperators.find(val) != arithmeticOperators.end() ? stringToArop.at(val) : stringToBitop.at(val)[type == Types::UNSIGNED] };
        std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(iNodeType);
        
        iChild->setType(type);
        iChild->pushChild(std::move(lchild));
        iChild->pushChild(std::move(rchild));
        return iChild;
    }
}

// reducing the depth of the tree if both children are literals
template<typename T>
std::unique_ptr<IRTree> IntermediateRepresentation::mergeLiterals(std::unique_ptr<IRTree>&& lchild, std::unique_ptr<IRTree>&& rchild, const ASTree* node) const {
    T lval = (std::is_same<T, int>::value ? std::stoi(lchild->getValue()) : std::stoul(lchild->getValue()));
    T rval = (std::is_same<T, int>::value ? std::stoi(rchild->getValue()) : std::stoul(rchild->getValue()));
    T result{ mergeValues<T>(lval, rval,node) };

    Types type{ std::is_same<T, int>::value ? Types::INT : Types::UNSIGNED };
    std::string suffix{ type == Types::INT ? "" : "u" };

    return std::make_unique<IRTree>("", std::to_string(result) + suffix, type, IRNodeType::LITERAL);
}

template<typename T>
T IntermediateRepresentation::mergeValues(T l, T r, const ASTree* node) const {
    std::string op{ node->getToken().value };
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
        // unreachable
        throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> Invalid arithmetic operator '{}'",
            node->getToken().line, node->getToken().column, op));
}

std::unique_ptr<IRTree> IntermediateRepresentation::relationalExpression(const ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::CMP);
    iChild->setValue(node->getToken().value); // stores relational operator as value

    // extracting function calls to temporary variables
    auto temps{ initiateTemporaries(node->getChild(0)) };
    if(temps){
        iChild->pushChild(std::move(temps));
    }
    temps = initiateTemporaries(node->getChild(1));
    if(temps){
        iChild->pushChild(std::move(temps));
    }

    iChild->pushChild(numericalExpression(node->getChild(0)));
    iChild->pushChild(numericalExpression(node->getChild(1)));

    return iChild;
}

std::unique_ptr<IRTree> IntermediateRepresentation::id(const ASTree* node) const {
    return std::make_unique<IRTree>(node->getToken().value, "0", node->getType(), IRNodeType::ID);
}

std::unique_ptr<IRTree> IntermediateRepresentation::literal(const ASTree* node) const {
    return std::make_unique<IRTree>("", node->getToken().value, node->getType(), IRNodeType::LITERAL);
}

std::unique_ptr<IRTree> IntermediateRepresentation::functionCall(const ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(node->getToken().value, "", node->getType(), IRNodeType::CALL);
    iChild->pushChild(argument(node));
    return iChild;
}

std::unique_ptr<IRTree> IntermediateRepresentation::argument(const ASTree* node){
    std::unique_ptr<IRTree> iChild = std::make_unique<IRTree>(IRNodeType::ARGUMENT);
    for(const auto& child : node->getChild(0)->getChildren()){
        auto tmps = initiateTemporaries(child.get());
        if(tmps){
            iChild->pushChild(std::move(tmps));
        }
    }
    for(const auto& child : node->getChild(0)->getChildren()){
        iChild->pushChild(numericalExpression(child.get()));
    }
    return iChild;
}

// counting the number of function calls that should be replaced by temporary variables
size_t IntermediateRepresentation::countTemporaries(const ASTree* node) const {
    if(node->getNodeType() == ASTNodeType::FUNCTION_CALL){
        return 1;
    }
    else if(node->getNodeType() == ASTNodeType::LITERAL || node->getNodeType() == ASTNodeType::ID || node->getNodeType() == ASTNodeType::VARIABLE){
        return 0;
    }
    else if(node->getChildren().size() == 1){
        return countTemporaries(node->getChild(0));
    }
    else{
        return countTemporaries(node->getChild(0)) + countTemporaries(node->getChild(1));
    }
}

// generating temporary variables
std::unique_ptr<IRTree> IntermediateRepresentation::generateTemporaries(){
    std::string name = std::format("_t{}", ++irContext.temporaries);
    irContext.temporaryNames.push(name);
    return std::make_unique<IRTree>(name, "0", Types::NO_TYPE, IRNodeType::VARIABLE);
}

// assigning a returned value to temporary variables
void IntermediateRepresentation::assignFunctionCalls(IRTree* irNode, const ASTree* node, size_t& idx){
    if(node->getNodeType() == ASTNodeType::FUNCTION_CALL){
        auto temporary = irNode->getChild(idx);
        temporary->pushChild(std::make_unique<IRTree>(IRNodeType::ASSIGN));
        temporary->getChild(0)->pushChild(std::make_unique<IRTree>(temporary->getName(), "0", node->getType(), IRNodeType::ID));
        temporary->getChild(0)->pushChild(functionCall(node));
        irNode->setType(node->getType());
        ++idx;
    }
    else if(node->getNodeType() == ASTNodeType::LITERAL || node->getNodeType() == ASTNodeType::VARIABLE || node->getNodeType() == ASTNodeType::ID){
        return;
    }
    else if(node->getChildren().size() == 1){
        assignFunctionCalls(irNode, node->getChild(0), idx);
    }
    else{
        assignFunctionCalls(irNode, node->getChild(0), idx);
        assignFunctionCalls(irNode, node->getChild(1), idx);
    }
}

// replacing function calls with temporary variables in numerical expression
std::unique_ptr<IRTree> IntermediateRepresentation::replaceFunctionCall(const ASTree* node){
    assert(!irContext.temporaryNames.empty());
    std::string name = irContext.temporaryNames.top();
    irContext.temporaryNames.pop();
    return std::make_unique<IRTree>(name, "0", node->getType(), IRNodeType::ID);
}

std::unique_ptr<IRTree> IntermediateRepresentation::initiateTemporaries(const ASTree* node){
    size_t tmpCount = countTemporaries(node);
    if(tmpCount > 0){
        std::unique_ptr<IRTree> temporaryRoot = std::make_unique<IRTree>(IRNodeType::TEMPORARY);
        size_t firstTemporaryIndex = 0;
        for(size_t i = 0; i < tmpCount; ++i){
            temporaryRoot->pushChild(generateTemporaries());
        }
        assignFunctionCalls(temporaryRoot.get(), node, firstTemporaryIndex);
        return temporaryRoot;
    }
    return nullptr;
}