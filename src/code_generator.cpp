#include "../include/code_generator.hpp"

CodeGenerator::CodeGenerator() : generatedCode("output.s"){}

CodeGenerator::~CodeGenerator(){
    generatedCode.close();
}

size_t CodeGenerator::getNextLabelNum(){
    return labelNum++;
}

void CodeGenerator::generateCode(std::shared_ptr<IRTree> root){
    if(!generatedCode.is_open()){
        throw std::runtime_error("File failed to generate");
    }
    generatedCode << ".global _start\n";
    generatedCode << ".text\n\n";
    generatedCode << "_start:\n";
    generatedCode << "\tjmp main\n";

    for(const auto& child : root->getChildren()){
        generateFunction(child);
    }
}

void CodeGenerator::generateFunction(std::shared_ptr<IRTree> node){
    activeFunction = node->getName();
    generatedCode << "\n" + activeFunction + ":\n";
    generatedCode << "\tpush %rbp\n";
    generatedCode << "\tmov %rsp, %rbp\n";

    generateParameter(node->getChild(0));

    generateVariable(node->getChild(1));

    for(size_t i = 2; i < node->getChildren().size(); i++){
        generateStatement(node->getChild(i));
    }

    generatedCode << activeFunction + "_end:\n";
    
    size_t varCount = node->getChild(1)->getChildren().size();
    if(varCount > 0){
        generatedCode << "\tadd $" + std::to_string(varCount*8) + ", %rsp\n";
    }
    
    generatedCode << "\tmov %rbp, %rsp\n";
    generatedCode << "\tpop %rbp\n";

    if(node->getName() != "main"){
        generatedCode << "\tret\n";
    }
    else{
        generatedCode << "\tmov %rax, %rdi\n";
        generatedCode << "\tmovq $60, %rax\n";
        generatedCode << "\tsyscall\n";
    }

    variableMapping.clear();

}

void CodeGenerator::generateParameter(std::shared_ptr<IRTree> node){
    size_t i = 2;
    for(const auto& parameter : node->getChildren()){
        variableMapping.insert({parameter->getName(), std::to_string(i*8) + "(%rbp)"});
        ++i;
    }
}

void CodeGenerator::generateVariable(std::shared_ptr<IRTree> node){
    size_t varCount = node->getChildren().size();
    if(varCount == 0){
        return;
    }
    generatedCode << "\tsub $" + std::to_string(varCount*8) + ", %rsp\n\n";

    size_t i = 1;
    for(const auto& var : node->getChildren()){
        variableMapping.insert({var->getName(), "-" + std::to_string(i*8) + "(%rbp)"});
        generatedCode << "\tmovq $0, ";
        generateID(var);
        generatedCode << "\n";
        ++i;
    }
    generatedCode << "\n";
}

void CodeGenerator::generateStatement(std::shared_ptr<IRTree> node){
    switch(node->getNodeType()){
        case IRNodeType::IF:
            generateIfStatement(node);
            generatedCode << "\n";
            break;
        case IRNodeType::COMPOUND:
            generateCompoundStatement(node);
            break;
        case IRNodeType::ASSIGN:
            generateAssignmentStatement(node);
            generatedCode << "\n";
            break;
        case IRNodeType::RETURN:
            generateReturnStatement(node);
            generatedCode << "\n";
            break;
        case IRNodeType::WHILE:
            generateWhileStatement(node);
            generatedCode << "\n";
            break;
        case IRNodeType::FOR:
            generateForStatement(node);
            generatedCode << "\n";
            break;
        case IRNodeType::DO_WHILE:
            generateDoWhileStatement(node);
            generatedCode << "\n";
            break;
        default:
            throw std::runtime_error("Invalid statement" + iNodeToString.at(node->getNodeType()));
    }
}

void CodeGenerator::generateIfStatement(std::shared_ptr<IRTree> node){
    auto labNum = getNextLabelNum();
    std::string labNumStr = std::to_string(labNum);

    generatedCode << "if" + std::to_string(labNum) + ":\n";
    generateRelationalExpression(node->getChild(0));

    auto type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    generatedCode << "\t" + stringToOppJMP.at(node->getChild(0)->getValue())[type] << " else" + labNumStr + "\n";

    generateStatement(node->getChild(1));
    generatedCode << "\tjmp if" + labNumStr + "_end\n";

    generatedCode << "\nelse" + labNumStr + ":\n";
    if(node->getChildren().size() > 2){
        generateStatement(node->getChild(2));
    }

    generatedCode << "if" + labNumStr + "_end:\n";

}

void CodeGenerator::generateWhileStatement(std::shared_ptr<IRTree> node){
    auto labNum = getNextLabelNum();
    std::string labNumStr = std::to_string(labNum);

    generatedCode << "while" + labNumStr + ":\n";
    generateRelationalExpression(node->getChild(0));

    auto type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    generatedCode << "\t" + stringToOppJMP.at(node->getChild(0)->getValue())[type] << " while" + labNumStr + "_end\n\n";

    generateStatement(node->getChild(1));
    generatedCode << "\tjmp while" + labNumStr + "\n";
    generatedCode << "\nwhile" + labNumStr + "_end:\n";
}

void CodeGenerator::generateForStatement(std::shared_ptr<IRTree> node){
    auto labNum = getNextLabelNum();
    std::string labNumStr = std::to_string(labNum);

    generateAssignmentStatement(node->getChild(0));
    generatedCode << "\nfor" + labNumStr + ":\n";
    generateRelationalExpression(node->getChild(1));

    auto type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    generatedCode << "\t" + stringToOppJMP.at(node->getChild(1)->getValue())[type] << " for" + labNumStr + "_end\n\n";

    generateStatement(node->getChild(3));
    generateAssignmentStatement(node->getChild(2));
    generatedCode << "\tjmp for" + labNumStr + "\n";

    generatedCode << "\nfor" + labNumStr + "_end:\n";  

}

void CodeGenerator::generateDoWhileStatement(std::shared_ptr<IRTree> node){
    auto labNum = getNextLabelNum();
    std::string labNumStr = std::to_string(labNum);

    generatedCode << "do_while" + labNumStr + ":\n";
    generateStatement(node->getChild(0));
    
    generateRelationalExpression(node->getChild(1));
    auto type = node->getChild(1)->getChild(0)->getType() == Types::INT ? 0 : 1;
    generatedCode << "\t" + stringToJMP.at(node->getChild(1)->getValue())[type] + " do_while" + labNumStr + "\n";
}

void CodeGenerator::generateCompoundStatement(std::shared_ptr<IRTree> node){
    for(const auto& child : node->getChildren()){
        generateStatement(child);
    }
}

void CodeGenerator::generateAssignmentStatement(std::shared_ptr<IRTree> node){
    generateNumericalExpression(node->getChild(1));
    generatedCode << "\tpop %rax\n";
    generatedCode << "\tmov %rax, ";
    generateID(node->getChild(0));
    generatedCode << "\n";
}

void CodeGenerator::generateReturnStatement(std::shared_ptr<IRTree> node){
    if(node->getChildren().size() != 0){
        generateNumericalExpression(node->getChild(0));
        generatedCode << "\tpop %rax\n";
    }else{
        generatedCode << "\txor %rax, %rax\n";
    }
    generatedCode << "\tjmp " + activeFunction + "_end\n";
}

void CodeGenerator::generateNumericalExpression(std::shared_ptr<IRTree> node){
    if(node->getNodeType() == IRNodeType::ID){
        generatedCode << "\tpush ";
        generateID(node);
        generatedCode << "\n";
    }
    else if(node->getNodeType() == IRNodeType::LITERAL){
        generatedCode << "\tpush ";
        generateLiteral(node);
        generatedCode << "\n";
    }else if (node->getNodeType() == IRNodeType::CALL){
        generateFunctionCall(node);
        generatedCode << "\tpush %rax\n";
    }
    else{
        generateNumericalExpression(node->getChild(0));
        generateNumericalExpression(node->getChild(1));
        generatedCode << "\tpop %rbx\n";
        generatedCode << "\tpop %rax\n";
        generatedCode << "\t" + iNodeToString.at(node->getNodeType()) + " %rbx, %rax\n";
        generatedCode << "\tpush %rax\n";
    }
}

void CodeGenerator::generateRelationalExpression(std::shared_ptr<IRTree> node){
    generateNumericalExpression(node->getChild(0));
    generateNumericalExpression(node->getChild(1));
    generatedCode << "\tpop %rdx\n";
    generatedCode << "\tpop %rcx\n";

    generatedCode << "\tcmp %rdx, %rcx\n";
}

void CodeGenerator::generateID(std::shared_ptr<IRTree> node){
    generatedCode << variableMapping.at(node->getName());
}

void CodeGenerator::generateLiteral(std::shared_ptr<IRTree> node){
    std::string val = node->getValue();
    if(node->getType() == Types::UNSIGNED){
        val.pop_back();
    }
    generatedCode << "$" + val;
}

void CodeGenerator::generateFunctionCall(std::shared_ptr<IRTree> node){
    
    generateArgument(node->getChild(0));

    generatedCode << "\tcall " + node->getName() + '\n';

    clearArguments(node->getChild(0));
}

void CodeGenerator::generateArgument(std::shared_ptr<IRTree> node){
    for(const auto& arg : node->getChildren()){
        generateNumericalExpression(arg);
    }
}

void CodeGenerator::clearArguments(std::shared_ptr<IRTree> node){
    for(size_t i = 0; i < node->getChildren().size(); i++){
        generatedCode << "\tpop %r9\n";
    }
}