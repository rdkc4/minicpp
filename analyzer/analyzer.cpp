#include "analyzer.hpp"

#include <memory>
#include <stdexcept>
#include <format>

Analyzer::Analyzer(ScopeManager& scopeManager) : scopeManager{scopeManager}, activeFunction{""}, returned{false} {}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SEMANTIC CHECK
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::semanticCheck(ASTree* root){
    ASTree* flist = root->getChildren().back().get();

    // global scope
    scopeManager.pushScope();

    for(const auto& child : flist->getChildren()){
        checkFunction(child.get());
    }

    // main function existence check
    if(!scopeManager.getSymbolTable().lookupSymbol("main", {Kinds::FUN})){
        throw std::runtime_error("'main' function not found");
    }

    scopeManager.popScope();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION: type validation, parameter validation, body validation, return validation
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkFunction(ASTree* node){
    Types returnType = node->getType().value();

    // function type check
    if(returnType == Types::NO_TYPE){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
            node->getToken().line, node->getToken().column, typeToString.at(returnType), node->getToken().value));
    }
    else if(returnType == Types::AUTO){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction cannot be performed on function '{} {}'", 
            node->getToken().line, node->getToken().column, typeToString.at(returnType), node->getToken().value));
    }

    // function redefinition check
    if(!scopeManager.pushSymbol(Symbol{node->getToken().value, Kinds::FUN, returnType})){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> function redefined '{} {}'", 
            node->getToken().line, node->getToken().column, typeToString.at(returnType), node->getToken().value));
    }

    activeFunction = node->getToken().value;
    returned = false;
    
    // function scope
    scopeManager.pushScope();
    checkParameter(node->getChild(0));
    checkBody(node->getChild(1));
    scopeManager.popScope();
    
    // function return type check
    if(returnType != Types::VOID && !returned){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> function '{} {}' returns '{}'", 
            node->getToken().line, node->getToken().column, typeToString.at(returnType), node->getToken().value, typeToString.at(Types::VOID)));
    }
    
    activeFunction = "";
    returned = false;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// PARAMETER CHECK - type check, variable redefinition
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkParameter(ASTree* node){
    // pointer to parameters for easier function call type checking
    scopeManager.getSymbolTable().getSymbol(activeFunction).setParameters(node);
    
    // parameter check for main
    if(activeFunction == "main" && node->getChildren().size() > 0){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> function 'main' cannot take parameters", 
            node->getToken().line, node->getToken().column));
    }

    for(const auto& child : node->getChildren()){
        // parameter type check
        Types type = child->getType().value();
        if(type == Types::VOID || type == Types::NO_TYPE){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
                child->getToken().line, child->getToken().column, typeToString.at(type), child->getToken().value));
        }
        else if(type == Types::AUTO){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction cannot be performed on parameters '{} {}'", 
                child->getToken().line, child->getToken().column, typeToString.at(type), child->getToken().value));
        }

        // parameter redefinition check
        if(!scopeManager.pushSymbol(Symbol{child->getToken().value, Kinds::PAR, child->getType().value()})){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> variable redefined '{}'", 
                child->getToken().line, child->getToken().column, child->getToken().value));
        }
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// BODY CHECK - variable check, construct check
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkBody(ASTree* node){
    for(const auto& child : node->getChildren()){
        checkConstruct(child.get());
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTRUCT CHECK - variable / statement check
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkConstruct(ASTree* node){
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
void Analyzer::checkVariable(ASTree* node){
    // variable type check
    Types type = node->getType().value();
    if(type == Types::VOID || type == Types::NO_TYPE){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{} {}'", 
            node->getToken().line, node->getToken().column, typeToString.at(type), node->getToken().value));
    }
    else if(type == Types::AUTO && node->getChildren().empty()){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> type deduction failed '{} {}'", 
            node->getToken().line, node->getToken().column, typeToString.at(type), node->getToken().value));
    }

    // variable redefinition check
    if(!scopeManager.pushSymbol(Symbol{node->getToken().value, Kinds::VAR, type})){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> variable redefined '{}'", 
            node->getToken().line, node->getToken().column, node->getToken().value));
    }

    // direct initialization
    if(node->getChildren().size() != 0){
        checkStatement(node->getChild(0));
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// STATEMENT CHECK - based on statement type
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkStatement(ASTree* node){
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
                node->getToken().line, node->getToken().column, astNodeTypeToString.at(node->getNodeType())));
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// IF STATEMENT CHECK - relational expression check
// child 0 - if relexp, child 1 - if statement
// for child > 1 -> child % 2 == 0 ? relexp : statement
// if there is odd number of children, else statement is at the back
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkIfStatement(ASTree* node){
    for(const auto& child : node->getChildren()){
        if(child->getNodeType() == ASTNodeType::RELATIONAL_EXPRESSION){
            checkRelationalExpression(child.get());
        }
        else{
            checkConstruct(child.get());
        }
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// WHILE STATEMENT CHECK - relational expression check
// child 0 - relexp
// child 1 - constructs
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkWhileStatement(ASTree* node){
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
void Analyzer::checkForStatement(ASTree* node){
    checkAssignmentStatement(node->getChild(0));
    checkRelationalExpression(node->getChild(1));
    checkAssignmentStatement(node->getChild(2));

    std::string lname = node->getChild(0)->getChild(0)->getToken().value;
    std::string rname = node->getChild(2)->getChild(0)->getToken().value;

    // for statement variable check (init & inc)   
    if(lname != rname){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid for statement - variable mismatch '{}' != '{}'", 
            node->getToken().line, node->getToken().column, lname, rname));
    }

    checkConstruct(node->getChild(3));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DO WHILE STATEMENT CHECK - construct check, relational expression check
// child 0 - constructs
// child 1 - relexp
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkDoWhileStatement(ASTree* node){
    checkConstruct(node->getChild(0));
    checkRelationalExpression(node->getChild(1));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SWITCH STATEMENT CHECK - id check, cases check (duplicate check, type check)
// child 0 - id
// child 1 - cases / default
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkSwitchStatement(ASTree* node){
    checkID(node->getChild(0));

    // case check
    if(node->getChild(0)->getType().value() == Types::INT){
        checkSwitchStatementCases<int>(node);
    }
    else if(node->getChild(0)->getType().value() == Types::UNSIGNED){
        checkSwitchStatementCases<unsigned>(node);
    }
    else{
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid type '{}'", 
            node->getToken().line, node->getToken().column, node->getChild(0)->getToken().value));
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
void Analyzer::checkSwitchStatementCases(ASTree* node){
    std::unordered_set<T> set;
    Types expectedType = std::is_same<T, int>::value ? Types::INT : Types::UNSIGNED;

    for(size_t i = 1; i < node->getChildren().size(); i++){
        ASTree* child = node->getChild(i);
        if(child->getNodeType() == ASTNodeType::CASE){
            checkLiteral(child->getChild(0));

            T val;
            // case type check
            Types type = child->getChild(0)->getType().value();
            if(child->getChild(0)->getType().value() != expectedType){
                throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid case - type mismatch: expected '{}', got '{}'", 
                    child->getToken().line, child->getToken().column, typeToString.at(expectedType), typeToString.at(type)));
            }
            // case duplicate check
            else if(set.find(val = (std::is_same<T, int>::value ? std::stoi(child->getChild(0)->getToken().value) 
                                                                : std::stoul(child->getChild(0)->getToken().value))) != set.end()){

                throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> duplicate case '{}'", 
                    child->getToken().line, child->getToken().column, child->getChild(0)->getToken().value));
            }
            else{
                for(const auto& _child : child->getChild(1)->getChildren()){
                    checkConstruct(_child.get());
                }
                set.insert(val);
            }
        }
        else{
            for(const auto& _child : child->getChild(0)->getChildren()){
                checkConstruct(_child.get());
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// COMPOUND STATEMENT CHECK - check constructs
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkCompoundStatement(ASTree* node){
    // compound scope
    scopeManager.pushScope();
    for(const auto& child : node->getChildren()){
        checkConstruct(child.get());
    }
    scopeManager.popScope();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ASSIGNMENT STATEMENT CHECK - type checking
// child 0 - destination variable
// child 1 - numexp
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkAssignmentStatement(ASTree* node){
    ASTree* lchild = node->getChild(0);
    ASTree* rchild = node->getChild(1);
    
    checkNumericalExpression(rchild);
    Types rtype = rchild->getType().value();
    
    checkID(lchild);
    Types ltype = scopeManager.getSymbolTable().getSymbol(lchild->getToken().value).getType();
    
    // assignment type check
    if(rtype != ltype && ltype != Types::AUTO){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid assignment statement - type mismatch: expected '{}', got '{}'", 
            node->getToken().line, node->getToken().column, typeToString.at(ltype), typeToString.at(rtype)));
    }

    if(ltype == Types::AUTO){
        scopeManager.getSymbolTable().getSymbol(lchild->getToken().value).setType(rtype);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETURN STATEMENT CHECK - type check (ret val, expected val)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkReturnStatement(ASTree* node){
    Types type = Types::NO_TYPE;
    if(node->getChildren().empty()){
        type = Types::VOID;
    }
    else{
        type = getNumericalExpressionType(node->getChild(0));
    }

    // return type check
    Types returnType = scopeManager.getSymbolTable().getSymbol(activeFunction).getType();
    if(returnType != type){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid return statement - type mismatch: '{} {}' returns '{}'", 
            node->getToken().line, node->getToken().column, typeToString.at(returnType), activeFunction, typeToString.at(type)));
    }
    returned = true;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// NUMERICAL EXPRESSION CHECK - set type to numexp node for easier type check
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkNumericalExpression(ASTree* node){
    node->setType(getNumericalExpressionType(node));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// NUMEXP TYPE CHECKING - each id/literal of same type, each id must exist in symbol table
//-----------------------------------------------------------------------------------------------------------------------------------------------------
Types Analyzer::getNumericalExpressionType(ASTree* node){
    if(node->getNodeType() == ASTNodeType::ID){
        checkID(node);
        return node->getType().value();
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
        // numerical expression type check
        Types ltype = getNumericalExpressionType(node->getChild(0));
        Types rtype = getNumericalExpressionType(node->getChild(1));
        if(ltype != rtype){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid expression - type mismatch: expected '{}', got '{}'", 
                node->getToken().line, node->getToken().column, typeToString.at(ltype), typeToString.at(rtype)));
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
void Analyzer::checkRelationalExpression(ASTree* node){
    ASTree* lchild = node->getChild(0);
    ASTree* rchild = node->getChild(1);

    checkNumericalExpression(lchild);
    Types ltype = lchild->getType().value();
    checkNumericalExpression(rchild);
    Types rtype = rchild->getType().value();
    
    // relational expression type check
    if(ltype != rtype){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> type mismatch: expected '{}', got '{}'", 
            node->getToken().line, node->getToken().column, typeToString.at(ltype), typeToString.at(rtype)));
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ID CHECK - variable redefinition check
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkID(ASTree* node){
    std::string name = node->getToken().value;
    if(!scopeManager.getSymbolTable().lookupSymbol(name, {Kinds::VAR, Kinds::PAR})){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> undefined variable '{}'", 
            node->getToken().line, node->getToken().column, name));
    }
    node->setType(scopeManager.getSymbolTable().getSymbol(name).getType());
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// LITERAL CHECK - literal validation
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkLiteral(ASTree* node){
    std::string name = node->getToken().value;
    // invalid literal check
    if((node->getType().value() == Types::UNSIGNED && (name.back() != 'u' || name.front() == '-')) || (node->getType().value() == Types::INT && name.back() == 'u')){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid literal '{}'", 
            node->getToken().line, node->getToken().column, name));
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION CALL CHECK - argument check
// child 0 - arguments
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkFunctionCall(ASTree* node){
    // function existence
    if(!scopeManager.getSymbolTable().lookupSymbol(node->getToken().value, {Kinds::FUN})){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> undefined function '{}'", 
            node->getToken().line, node->getToken().column, node->getToken().value));
    }
    
    // check if main is called
    if(node->getToken().value == "main"){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> 'main' is not callable function", 
            node->getToken().line, node->getToken().column));
    }

    // comparation of given parameter count with expected parameter count
    size_t expectedParams = scopeManager.getSymbolTable().getSymbol(node->getToken().value).getParameters()->getChildren().size();
    if(node->getChild(0)->getChildren().size() != expectedParams){
        throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid function call '{}': provided parameters '{}', expected '{}'", 
            node->getToken().line, node->getToken().column, node->getToken().value, node->getChild(0)->getChildren().size(), expectedParams));
    }
    node->setType(scopeManager.getSymbolTable().getSymbol(node->getToken().value).getType());
    checkArgument(node);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ARGUMENT CHECK - type comparison between called function and function call
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Analyzer::checkArgument(ASTree* node){
    ASTree* functionParameters = scopeManager.getSymbolTable().getSymbol(node->getToken().value).getParameters();
    ASTree* arguments = node->getChild(0);

    size_t i = 0;
    for(const auto& child : arguments->getChildren()){
        checkNumericalExpression(child.get());

        // type check of corresponding parameters in function and function call
        Types ltype = child->getType().value();
        Types rtype = functionParameters->getChild(i)->getType().value();
        if(ltype != rtype){
            throw std::runtime_error(std::format("Line {}, Column {}: SEMANTIC ERROR -> invalid argument - type mismatch: expected '{}', got '{} {}'",
                child->getToken().line, child->getToken().column, typeToString.at(rtype), typeToString.at(ltype), child->getToken().value));
        }
        ++i;
    }
}