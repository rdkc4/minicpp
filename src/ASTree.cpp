#include "../include/ASTree.hpp"

ASTree::ASTree(ASTNodeType nodeType, const Token& token) : nodeType(nodeType), token(std::make_shared<Token>(token)){}

ASTree::ASTree(ASTNodeType nodeType, const Token& token, Types type) : nodeType(nodeType), token(std::make_shared<Token>(token)), type(type){}

ASTree::~ASTree() = default;

const std::unordered_map<ASTNodeType, std::string> astNodeTypeToString = {
    {ASTNodeType::PROGRAM, "PROGRAM"},
    {ASTNodeType::FUNCTION_LIST, "FUNCTION_LIST"},
    {ASTNodeType::FUNCTION, "FUNCTION"},
    {ASTNodeType::PARAMETER, "PARAMETER"},
    {ASTNodeType::BODY, "BODY"},
    {ASTNodeType::VARIABLE_LIST, "VARIABLE_LIST"},
    {ASTNodeType::VARIABLE, "VARIABLE"},
    {ASTNodeType::STATEMENT_LIST, "STATEMENT_LIST"},
    {ASTNodeType::STATEMENT, "STATEMENT"},
    {ASTNodeType::COMPOUND_STATEMENT, "COMPOUND_STATEMENT"},
    {ASTNodeType::ASSIGNMENT_STATEMENT, "ASSIGNMENT_STATEMENT"},
    {ASTNodeType::RETURN_STATEMENT, "RETURN_STATEMENT"},
    {ASTNodeType::IF_STATEMENT, "IF_STATEMENT"},
    {ASTNodeType::WHILE_STATEMENT, "WHILE_STATEMENT"},
    {ASTNodeType::FOR_STATEMENT, "FOR_STATEMENT"},
    {ASTNodeType::DO_WHILE_STATEMENT, "DO_WHILE_STATEMENT"},
    {ASTNodeType::SWITCH_STATEMENT, "SWITCH_STATEMENT"},
    {ASTNodeType::CASE, "CASE"},
    {ASTNodeType::DEFAULT, "DEFAULT"},
    {ASTNodeType::BREAK, "BREAK"},
    {ASTNodeType::NUMERICAL_EXPRESSION, "NUMERICAL_EXPRESSION"},
    {ASTNodeType::EXPRESSION, "EXPRESSION"},
    {ASTNodeType::RELATIONAL_EXPRESSION, "RELATIONAL_EXPRESSION"},
    {ASTNodeType::FUNCTION_CALL, "FUNCTION_CALL"},
    {ASTNodeType::ARGUMENT, "ARGUMENT"},
    {ASTNodeType::LITERAL, "LITERAL"},
    {ASTNodeType::ID, "ID"},
    {ASTNodeType::CASE, "CASE"},
    {ASTNodeType::DEFAULT, "DEFAULT"}
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ADD CHILD TO AST TREE
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void ASTree::pushChild(std::shared_ptr<ASTree> child){
    children.push_back(child);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETURN CHILD AT GIVEN INDEX
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> ASTree::getChild(size_t index){
    if(index < children.size()){
        return children.at(index);
    }
    return nullptr;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETURN ALL CHILDREN
//-----------------------------------------------------------------------------------------------------------------------------------------------------
const std::vector<std::shared_ptr<ASTree>>& ASTree::getChildren() const{
    return children;
}

void ASTree::setType(Types t){
    type = t;
}

ASTNodeType ASTree::getNodeType() const{
    return nodeType;
}

std::optional<Types> ASTree::getType() const{
    return type;
}

const std::shared_ptr<Token> ASTree::getToken() const {
    return token;
}

std::string ASTree::toString(){
    return "|" + astNodeTypeToString.at(nodeType) + " | " + token->value + "\n";
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DISPLAY AST TREE (debbuging purposes)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void ASTree::traverse(size_t offset){
    std::cout << std::string(offset*2, ' ') + "|->" + toString();
    for(const auto& child:children){
        child->traverse(offset+1);
    }

}
