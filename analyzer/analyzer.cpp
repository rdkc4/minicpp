#include "analyzer.hpp"

#include <memory>
#include <stdexcept>
#include <format>

Analyzer::Analyzer(ScopeManager& scopeManager) : scopeManager(scopeManager), activeFunction(""), returned(false){}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SEMANTIC CHECK
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::semanticCheck(std::shared_ptr<ASTree> root){
    std::shared_ptr<ASTree> flist = root;
    flist = flist->getChildren().back();
    scopeManager.pushScope();

    for(const auto& child : flist->getChildren()){
        Types type = child->getType().value();

        if(type == Types::NO_TYPE){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                child->getToken()->line, child->getToken()->column, typeToString.at(type), child->getToken()->value));
        }
        else if(type == Types::AUTO){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction cannot be performed on function '{} {}'", 
                child->getToken()->line, child->getToken()->column, typeToString.at(type), child->getToken()->value));
        }

        if(!scopeManager.pushSymbol(std::make_shared<Symbol>(child->getToken()->value, Kinds::FUN, type))){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> function redefined '{} {}'", 
                child->getToken()->line, child->getToken()->column, typeToString.at(type), child->getToken()->value));
        }
        checkFunction(child);
    }

    if(!scopeManager.getSymbolTable().lookupSymbol("main", {Kinds::FUN})){
        throw std::runtime_error("'main' function not found");
    }

    scopeManager.popScope();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION: type validation, parameter validation, body validation, return validation
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkFunction(std::shared_ptr<ASTree> node){
    activeFunction = node->getToken()->value;
    returned = false;
    
    scopeManager.pushScope();
    checkParameter(node->getChild(0));
    checkBody(node->getChild(1));
    scopeManager.popScope();
    
    Types returnType = scopeManager.getSymbolTable().getSymbol(activeFunction)->getType();
    if(returnType != Types::VOID && !returned){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> function '{} {}' returns '{}'", 
            node->getToken()->line, node->getToken()->column, typeToString.at(returnType), node->getToken()->value, typeToString.at(Types::VOID)));
    }
    
    activeFunction = "";
    returned = false;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// PARAMETER CHECK - type check, variable redefinition
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkParameter(std::shared_ptr<ASTree> node){
    scopeManager.getSymbolTable().getSymbol(activeFunction)->setParameters(node); //pointer to parameters for easier function call type checking
    
    if(activeFunction == "main" && node->getChildren().size() > 0){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> function 'main' cannot take parameters", 
            node->getToken()->line, node->getToken()->column));
    }

    for(const auto& child : node->getChildren()){
        Types type = child->getType().value();
        if(type == Types::VOID || type == Types::NO_TYPE){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                child->getToken()->line, child->getToken()->column, typeToString.at(type), child->getToken()->value));
        }
        else if(type == Types::AUTO){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction cannot be performed on parameters '{} {}'", 
                child->getToken()->line, child->getToken()->column, typeToString.at(type), child->getToken()->value));
        }

        if(!scopeManager.pushSymbol(std::make_shared<Symbol>(child->getToken()->value, Kinds::PAR, child->getType().value()))){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> variable redefined '{}'", 
                child->getToken()->line, child->getToken()->column, child->getToken()->value));
        }
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// BODY CHECK - variable check, construct check
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkBody(std::shared_ptr<ASTree> node){
    for(const auto& child : node->getChildren()){
        checkConstruct(child);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTRUCT CHECK - variable / statement check
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkConstruct(std::shared_ptr<ASTree> node){
    if(node->getNodeType() == ASTNodeType::VARIABLE){
        checkVariable(node);
    }
    else{
        checkStatement(node);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLE CHECK - type check, variable redefinition
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkVariable(std::shared_ptr<ASTree> node){
    Types type = node->getType().value();
    if(type == Types::VOID || type == Types::NO_TYPE){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
            node->getToken()->line, node->getToken()->column, typeToString.at(type), node->getToken()->value));
    }
    else if(type == Types::AUTO && node->getChildren().empty()){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction failed '{} {}'", 
            node->getToken()->line, node->getToken()->column, typeToString.at(type), node->getToken()->value));
    }

    if(!scopeManager.pushSymbol(std::make_shared<Symbol>(node->getToken()->value, Kinds::VAR, type))){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> variable redefined '{}'", 
            node->getToken()->line, node->getToken()->column, node->getToken()->value));
    }
    if(node->getChildren().size() != 0){
        checkStatement(node->getChild(0));
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// STATEMENT CHECK - based on statement type
//-----------------------------------------------------------------------------------------------------------------------------------------------------
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
        case ASTNodeType::SWITCH_STATEMENT:
            checkSwitchStatement(node);
            break;
        default:
            throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> expected 'STATEMENT' got '{}'",
                node->getToken()->line, node->getToken()->column, astNodeTypeToString.at(node->getNodeType())));
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// IF STATEMENT CHECK - relational expression check
// child 0 - if relexp, child 1 - if statement
// for child > 1 -> child % 2 == 0 ? relexp : statement
// if there is odd number of children, else statement is at the back
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkIfStatement(std::shared_ptr<ASTree> node){
    for(const auto& child : node->getChildren()){
        if(child->getNodeType() == ASTNodeType::RELATIONAL_EXPRESSION){
            checkRelationalExpression(child);
        }
        else{
            checkConstruct(child);
        }
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// WHILE STATEMENT CHECK - relational expression check
// child 0 - relexp
// child 1 - constructs
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkWhileStatement(std::shared_ptr<ASTree> node){
    checkRelationalExpression(node->getChild(0));
    checkConstruct(node->getChild(1));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FOR STATEMENT CHECK - type check, relational expression check, assignment check; id assign_st(1) == id assign_st(2)
// child 0 - assignment statement (init)
// child 1 - relexp
// child 2 - assignment statement (inc)
// child 3 - for statements
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkForStatement(std::shared_ptr<ASTree> node){
    checkAssignmentStatement(node->getChild(0));
    checkRelationalExpression(node->getChild(1));
    checkAssignmentStatement(node->getChild(2));

    std::string lname = node->getChild(0)->getChild(0)->getToken()->value;
    std::string rname = node->getChild(2)->getChild(0)->getToken()->value;
    
    if(lname != rname){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid for statement - variable mismatch '{}' != '{}'", 
            node->getToken()->line, node->getToken()->column, lname, rname));
    }

    checkConstruct(node->getChild(3));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DO WHILE STATEMENT CHECK - construct check, relational expression check
// child 0 - constructs
// child 1 - relexp
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkDoWhileStatement(std::shared_ptr<ASTree> node){
    checkConstruct(node->getChild(0));
    checkRelationalExpression(node->getChild(1));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SWITCH STATEMENT CHECK - id check, cases check (duplicate check, type check)
// child 0 - id
// child 1 - cases / default
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkSwitchStatement(std::shared_ptr<ASTree> node){
    checkID(node->getChild(0));
    if(node->getChild(0)->getType().value() == Types::INT){
        checkSwitchStatementCases<int>(node);
    }
    else if(node->getChild(0)->getType().value() == Types::UNSIGNED){
        checkSwitchStatementCases<unsigned>(node);
    }
    else{
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{}'", 
            node->getToken()->line, node->getToken()->column, node->getChild(0)->getToken()->value));
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SWITCH STATEMENT CASE/DEFAULT CHECK - duplicate check / type check
// cases child 0 - literal
// cases child 1 - constructs
// cases child 2 - break (optional)
// default child 0 - constructs (break can exist, but it will be ignored)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
template<typename T>
void Analyzer::checkSwitchStatementCases(std::shared_ptr<ASTree> node){
    std::unordered_set<T> set;
    Types expectedType = std::is_same<T, int>::value ? Types::INT : Types::UNSIGNED;

    for(size_t i = 1; i < node->getChildren().size(); i++){
        std::shared_ptr<ASTree> child = node->getChild(i);
        if(child->getNodeType() == ASTNodeType::CASE){
            checkLiteral(child->getChild(0));

            T val;
            Types type = child->getChild(0)->getType().value();
            if(child->getChild(0)->getType().value() != expectedType){
                throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid case - type mismatch: expected '{}', got '{}'", 
                    child->getToken()->line, child->getToken()->column, typeToString.at(expectedType), typeToString.at(type)));
            }
            else if(set.find(val = (std::is_same<T, int>::value ? std::stoi(child->getChild(0)->getToken()->value) 
                                                                : std::stoul(child->getChild(0)->getToken()->value))) != set.end()){

                throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> duplicate case '{}'", 
                    child->getToken()->line, child->getToken()->column, child->getChild(0)->getToken()->value));
            }
            else{
                for(const auto& _child : child->getChild(1)->getChildren()){
                    checkConstruct(_child);
                }
                set.insert(val);
            }
        }
        else{
            for(const auto& _child : child->getChild(0)->getChildren()){
                checkConstruct(_child);
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// COMPOUND STATEMENT CHECK - check constructs
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkCompoundStatement(std::shared_ptr<ASTree> node){
    scopeManager.pushScope();
    for(const auto& child : node->getChildren()){
        checkConstruct(child);
    }
    scopeManager.popScope();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ASSIGNMENT STATEMENT CHECK - type checking
// child 0 - destination variable
// child 1 - numexp
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkAssignmentStatement(std::shared_ptr<ASTree> node){
    std::shared_ptr<ASTree> lchild = node->getChild(0);
    std::shared_ptr<ASTree> rchild = node->getChild(1);
    
    checkNumericalExpression(rchild);
    Types rtype = rchild->getType().value();
    
    checkID(lchild);
    Types ltype = scopeManager.getSymbolTable().getSymbol(lchild->getToken()->value)->getType();
    
    if(rtype != ltype && ltype != Types::AUTO){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid assignment statement - type mismatch: expected '{}', got '{}'", 
            node->getToken()->line, node->getToken()->column, typeToString.at(ltype), typeToString.at(rtype)));
    }

    if(ltype == Types::AUTO){
        scopeManager.getSymbolTable().getSymbol(lchild->getToken()->value)->setType(rtype);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETURN STATEMENT CHECK - type check (ret val, expected val)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkReturnStatement(std::shared_ptr<ASTree> node){
    Types type = Types::NO_TYPE;
    if(node->getChildren().empty()){
        type = Types::VOID;
    }
    else{
        type = getNumericalExpressionType(node->getChild(0));
    }

    Types returnType = scopeManager.getSymbolTable().getSymbol(activeFunction)->getType();
    if(returnType != type){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid return statement - type mismatch: '{} {}' returns '{}'", 
            node->getToken()->line, node->getToken()->column, typeToString.at(returnType), activeFunction, typeToString.at(type)));
    }
    returned = true;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// NUMERICAL EXPRESSION CHECK - set type to numexp node for easier type check
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkNumericalExpression(std::shared_ptr<ASTree> node){
    node->setType(getNumericalExpressionType(node));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// NUMEXP TYPE CHECKING - each id/literal of same type, each id must exist in symbol table
//-----------------------------------------------------------------------------------------------------------------------------------------------------
Types Analyzer::getNumericalExpressionType(std::shared_ptr<ASTree> node){
    if(node->getNodeType() == ASTNodeType::ID){
        if(!scopeManager.getSymbolTable().lookupSymbol(node->getToken()->value, {Kinds::VAR, Kinds::PAR})){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> undefined variable '{}'", 
                node->getToken()->line, node->getToken()->column, node->getToken()->value));
        }
        Types type = scopeManager.getSymbolTable().getSymbol(node->getToken()->value)->getType();
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
        Types ltype = getNumericalExpressionType(node->getChild(0));
        Types rtype = getNumericalExpressionType(node->getChild(1));
        if(ltype != rtype){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid expression - type mismatch: expected '{}', got '{}'", 
                node->getToken()->line, node->getToken()->column, typeToString.at(ltype), typeToString.at(rtype)));
        }
        
        return ltype;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RELATIONAL EXPRESSION CHECK - type check
// node - contains rel operator
// child 0 - left operand
// child 1 - right operand
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkRelationalExpression(std::shared_ptr<ASTree> node){
    std::shared_ptr<ASTree> lchild = node->getChild(0);
    std::shared_ptr<ASTree> rchild = node->getChild(1);

    checkNumericalExpression(lchild);
    Types ltype = lchild->getType().value();
    checkNumericalExpression(rchild);
    Types rtype = rchild->getType().value();
    
    if(ltype != rtype){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> type mismatch: expected '{}', got '{}'", 
            node->getToken()->line, node->getToken()->column, typeToString.at(ltype), typeToString.at(rtype)));
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ID CHECK - variable redefinition check
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkID(std::shared_ptr<ASTree> node){
    std::string name = node->getToken()->value;
    if(!scopeManager.getSymbolTable().lookupSymbol(name, {Kinds::VAR, Kinds::PAR})){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> undefined variable '{}'", 
            node->getToken()->line, node->getToken()->column, name));
    }
    node->setType(scopeManager.getSymbolTable().getSymbol(name)->getType());
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// LITERAL CHECK - literal validation
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkLiteral(std::shared_ptr<ASTree> node){
    std::string name = node->getToken()->value;
    if((node->getType().value() == Types::UNSIGNED && (name.back() != 'u' || name.front() == '-')) || (node->getType().value() == Types::INT && name.back() == 'u')){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid literal '{}'", 
            node->getToken()->line, node->getToken()->column, name));
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION CALL CHECK - argument check
// child 0 - arguments
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkFunctionCall(std::shared_ptr<ASTree> node){
    if(!scopeManager.getSymbolTable().lookupSymbol(node->getToken()->value, {Kinds::FUN})){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> undefined function '{}'", 
            node->getToken()->line, node->getToken()->column, node->getToken()->value));
    }
    
    if(node->getToken()->value == "main"){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> 'main' is not callable function", 
            node->getToken()->line, node->getToken()->column));
    }

    size_t expectedParams = scopeManager.getSymbolTable().getSymbol(node->getToken()->value)->getParameters()->getChildren().size();
    if(node->getChild(0)->getChildren().size() != expectedParams){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid function call '{}': provided parameters '{}', expected '{}'", 
            node->getToken()->line, node->getToken()->column, node->getToken()->value, node->getChild(0)->getChildren().size(), expectedParams));
    }
    node->setType(scopeManager.getSymbolTable().getSymbol(node->getToken()->value)->getType());
    checkArgument(node);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ARGUMENT CHECK - type comparison between called function and function call
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkArgument(std::shared_ptr<ASTree> node){
    std::shared_ptr<ASTree> functionParameters = scopeManager.getSymbolTable().getSymbol(node->getToken()->value)->getParameters();
    std::shared_ptr<ASTree> arguments = node->getChild(0);

    size_t i = 0;
    for(const auto& child : arguments->getChildren()){
        checkNumericalExpression(child);

        Types ltype = child->getType().value();
        Types rtype = functionParameters->getChild(i)->getType().value();
        if(ltype != rtype){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid argument - type mismatch: expected '{}', got '{} {}'",
                child->getToken()->line, child->getToken()->column, typeToString.at(rtype), typeToString.at(ltype), child->getToken()->value));
        }
        ++i;
    }
}