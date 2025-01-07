#include "code_generator.hpp"
#include <string>
#include <format>

CodeGenerator::CodeGenerator(std::string& output) : generatedCode(output){}

CodeGenerator::~CodeGenerator(){
    generatedCode.close();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// allocating general-purpose register r(8-15)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::takeGpReg(){
    ++gpFreeRegPos;
    
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// releasing general-purpose register r(8-15)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::freeGpReg(){
    --gpFreeRegPos;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// label name distinction 
//-----------------------------------------------------------------------------------------------------------------------------------------------------
size_t CodeGenerator::getNextLabelNum(){
    return labelNum++;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ASM CODE GENERATION - x86_64 (linux)
// -> optimize generation so it relies more on registers, rather than stack (TODO)
// > as -o something.o something.s
// > ld -o something something.o
// > ./something
// > echo $? <- check return value
//-----------------------------------------------------------------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// HANDLING STACK - obtaining variables, parameters, generating statements
// variableMapping - unordered_map which maps variable name to memory location on stack relative to %rbp
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateFunction(std::shared_ptr<IRTree> node){
    activeFunction = node->getName();
    generatedCode << std::format("\n{}:\n", activeFunction);
    generatedCode << "\tpush %rbp\n";
    generatedCode << "\tmov %rsp, %rbp\n";

    generateParameter(node->getChild(0));

    generateVariable(node->getChild(1));

    for(size_t i = 2; i < node->getChildren().size(); i++){
        generateStatement(node->getChild(i));
    }

    generatedCode << std::format("{}_end:\n", activeFunction);
    
    size_t varCount = node->getChild(1)->getChildren().size();
    if(varCount > 0){
        generatedCode << std::format("\tadd ${}, %rsp\n", varCount*8);
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

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// INSERTING PARAMETERS
// -> maybe switch to 32bit registers instead of 64 (TODO?)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateParameter(std::shared_ptr<IRTree> node){
    size_t i = 2;
    for(const auto& parameter : node->getChildren()){
        variableMapping.insert({parameter->getName(), std::format("{}(%rbp)", i*8)});
        ++i;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// INSERTING LOCAL VARIABLES - moving stack pointer, assigning default values (0)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateVariable(std::shared_ptr<IRTree> node){
    size_t varCount = node->getChildren().size();
    if(varCount == 0){
        return;
    }
    generatedCode << std::format("\tsub ${}, %rsp\n\n", varCount*8);

    size_t i = 1;
    for(const auto& var : node->getChildren()){
        variableMapping.insert({var->getName(), std::format("-{}(%rbp)", i*8)});
        generatedCode << std::format("\tmovq $0, {}\n", generateID(var));
        ++i;
    }
    generatedCode << "\n";
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// STATEMENT TYPES 
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateStatement(std::shared_ptr<IRTree> node){
    switch(node->getNodeType()){
        case IRNodeType::IF:
            generateIfStatement(node);
            break;
        case IRNodeType::COMPOUND:
            generateCompoundStatement(node);
            return;
        case IRNodeType::ASSIGN:
            generateAssignmentStatement(node);
            break;
        case IRNodeType::RETURN:
            generateReturnStatement(node);
            break;
        case IRNodeType::WHILE:
            generateWhileStatement(node);
            break;
        case IRNodeType::FOR:
            generateForStatement(node);
            break;
        case IRNodeType::DO_WHILE:
            generateDoWhileStatement(node);
            break;
        case IRNodeType::SWITCH:
            generateSwitchStatement(node);
            break;
        default:
            throw std::runtime_error(std::format("Invalid statement {}", irNodeToString.at(node->getNodeType())));
    }
    generatedCode << "\n";
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// IF STATEMENT - cmp relexp, opposite jmp to else/end, if statement, else statement
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateIfStatement(std::shared_ptr<IRTree> node){
    auto labNum = getNextLabelNum();
    auto size = node->getChildren().size();

    generatedCode << std::format("if{}:\n", labNum);
    generateRelationalExpression(node->getChild(0));

    auto type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    generatedCode << std::format("\t{}", stringToOppJMP.at(node->getChild(0)->getValue())[type]);

    if(size > 3){
        generatedCode << std::format(" elif{}_0\n", labNum);
    }
    else if(size == 3){
        generatedCode << std::format(" else{}\n", labNum);
    }
    else{
        generatedCode << std::format("if_end{}\n", labNum);
    }
    generatedCode << "\n";

    generateStatement(node->getChild(1));
    generatedCode << std::format("\tjmp if{}_end\n", labNum);

    for(size_t i = 2; i < size; i+=2){
        if(node->getChild(i)->getNodeType() == IRNodeType::CMP){
            generatedCode << std::format("\nelif{}_{}:\n", labNum, i/2-1);
            generateRelationalExpression(node->getChild(i));

            type = node->getChild(i)->getChild(0)->getType() == Types::INT ? 0 : 1;
            generatedCode << std::format("\t{}", stringToOppJMP.at(node->getChild(i)->getValue())[type]);
            if(i+2 <= size){
                generatedCode << std::format(" if{}_end\n", labNum);
            }
            else if(i+2 == size+1){
                generatedCode << std::format(" else{}\n", labNum);
            }
            else{
                generatedCode << std::format(" elif{}_{}\n", labNum, i/2); 
            }

            generateStatement(node->getChild(i+1));
            generatedCode << std::format("\tjmp if{}_end\n", labNum); 
        }
    }

    if(size % 2 == 1){
        generatedCode << std::format("\nelse{}:\n", labNum);
        generateStatement(node->getChildren().back());
    }

    generatedCode << std::format("if{}_end:\n", labNum);

}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// WHILE STATEMENT - cmp relexp, opposite jump to end, while statements
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateWhileStatement(std::shared_ptr<IRTree> node){
    auto labNum = getNextLabelNum();

    generatedCode << std::format("while{}:\n", labNum);
    generateRelationalExpression(node->getChild(0));

    auto type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    generatedCode << std::format("\t{} while{}_end\n\n", stringToOppJMP.at(node->getChild(0)->getValue())[type], labNum);

    generateStatement(node->getChild(1));
    generatedCode << std::format("\tjmp while{}\n", labNum);
    generatedCode << std::format("\nwhile{}_end:\n", labNum);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FOR STATEMENT - assign(init) statement (before loop), cmp relexp, opposite jmp, statements, assign(inc) statement
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateForStatement(std::shared_ptr<IRTree> node){
    auto labNum = getNextLabelNum();

    generateAssignmentStatement(node->getChild(0));
    generatedCode << std::format("\nfor{}:\n", labNum);
    generateRelationalExpression(node->getChild(1));

    auto type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    generatedCode << std::format("\t{} for{}_end\n\n", stringToOppJMP.at(node->getChild(1)->getValue())[type], labNum);

    generateStatement(node->getChild(3));
    generateAssignmentStatement(node->getChild(2));
    generatedCode << std::format("\tjmp for{}\n", labNum);

    generatedCode << std::format("\nfor{}_end:\n", labNum);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DO_WHILE STATEMENT - statements, relexp regular jmp
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateDoWhileStatement(std::shared_ptr<IRTree> node){
    auto labNum = getNextLabelNum();

    generatedCode << std::format("do_while{}:\n", labNum);
    generateStatement(node->getChild(0));
    
    generateRelationalExpression(node->getChild(1));
    auto type = node->getChild(1)->getChild(0)->getType() == Types::INT ? 0 : 1;
    generatedCode << std::format("\t{} do_while{}\n", stringToJMP.at(node->getChild(1)->getValue())[type], labNum);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// COMPOUND STATEMENT - statements
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateCompoundStatement(std::shared_ptr<IRTree> node){
    for(const auto& child : node->getChildren()){
        generateStatement(child);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ASSIGNMENT STATEMENT - numexp (calculate rvalue first), assign to destination variable
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateAssignmentStatement(std::shared_ptr<IRTree> node){
    generateNumericalExpression(node->getChild(1));
    freeGpReg();
    generatedCode << std::format("\tmovq {}, {}\n", gpRegisters.at(gpFreeRegPos), generateID(node->getChild(0)));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETURN STATEMENT - store numexp result at register %rax
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateReturnStatement(std::shared_ptr<IRTree> node){
    if(node->getChildren().size() != 0){
        generateNumericalExpression(node->getChild(0));
        freeGpReg();
        generatedCode << std::format("\tmovq {}, %rax\n", gpRegisters.at(gpFreeRegPos));
    }else{
        generatedCode << "\txor %rax, %rax\n";
    }
    generatedCode << std::format("\tjmp {}_end\n", activeFunction);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SWITCH STATEMENT - generate cases, cmp each case, opposite jmp to next case/default/end
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateSwitchStatement(std::shared_ptr<IRTree> node){
    auto labNum = getNextLabelNum();

    generatedCode << std::format("switch{}:\n", labNum);

    size_t i = 1;
    auto size = node->getChildren().size();
    auto var = node->getChild(0)->getName();
    bool hasDefault = node->getChildren().back()->getNodeType() == IRNodeType::DEFAULT;
    
    for(; i < size; i++){
        auto child = node->getChild(i);
        if(child->getNodeType() == IRNodeType::CASE){
            generatedCode << std::format("switch{}_case{}:\n", labNum, i-1);
            generatedCode << std::format("\tmovq {}, %rcx\n", variableMapping.at(var));
            generatedCode << std::format("\tmovq {}, %rdx\n", generateLiteral(child->getChild(0)));
            generatedCode << "\tcmp %rcx, %rdx\n";
            
            if(hasDefault){
                generatedCode << std::format("\tjne switch{}_{}\n", labNum, (i < size-2 ?  "case" + std::to_string(i) : "default"));
            }
            else{
                generatedCode << std::format("\tjne switch{}_{}\n", labNum, (i < size-1 ?  "case" + std::to_string(i) : "end"));
            }

            generateStatement(child->getChild(1));
            if(child->getChildren().size()==3){
                generatedCode << std::format("\tjmp switch{}_end\n", labNum);
            }

        }else{
            generatedCode << std::format("switch{}_default:\n", labNum);
            generateStatement(node->getChild(i)->getChild(0));
        }
    }
    generatedCode << std::format("switch{}_end:\n", labNum);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// NUMERICAL EXPRESSION - evaluating equations using general-purpose registers r(8-15)
// -> maybe use queue instead of a tree for numexp, easier generation, better reg usage (TODO?)
// -> needs rework, possible invalid allocation -> fix (TODO) 
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateNumericalExpression(std::shared_ptr<IRTree> node){
    if(node->getNodeType() == IRNodeType::ID){
        if(gpFreeRegPos < gpRegisters.size()){
            generatedCode << std::format("\tmovq {}, {}\n", generateID(node), gpRegisters.at(gpFreeRegPos));
        }
        else{
            generatedCode << std::format("\tpush {}\n", generateID(node));
        }
        takeGpReg();
    }
    else if(node->getNodeType() == IRNodeType::LITERAL){
        if(gpFreeRegPos < gpRegisters.size()){
            generatedCode << std::format("\tmovq {}, {}\n", generateLiteral(node), gpRegisters.at(gpFreeRegPos));
        }
        else{
            generatedCode << std::format("\tpush {}\n", generateLiteral(node));
        }
        takeGpReg();
    }else if (node->getNodeType() == IRNodeType::CALL){
        generateFunctionCall(node);
        if(gpFreeRegPos < gpRegisters.size()){
            generatedCode << std::format("\tmovq %rax, {}\n", gpRegisters.at(gpFreeRegPos));
        }
        else{
            generatedCode << "\tpush %rax\n";
        }
        takeGpReg();
    }
    else{
        std::string lreg;
        std::string rreg;
        
        generateNumericalExpression(node->getChild(0));
        generateNumericalExpression(node->getChild(1));
        
        freeGpReg();
        if(gpFreeRegPos >= gpRegisters.size()){
            lreg = "%rdi";
            generatedCode << "\tpop %rdi\n";
        }
        else{
            lreg = gpRegisters.at(gpFreeRegPos);
        }
        freeGpReg();
        if(gpFreeRegPos >= gpRegisters.size()){
            rreg = "%rsi";
            generatedCode << "\tpop %rsi\n";
        }
        else{
            rreg = gpRegisters.at(gpFreeRegPos);
        }
        
        auto nodeType = node->getNodeType();
        if(nodeType == IRNodeType::MUL || nodeType == IRNodeType::DIV){ // result of MUL || DIV is in RDX:RAX
            generatedCode << "\txor %rdx, %rdx\n"; //add overflow check (TODO)
            generatedCode << std::format("\tmovq {}, %rax\n", rreg);
            if(node->getType().value() == Types::INT){
                generatedCode << std::format("\ti{} {}\n", irNodeToString.at(nodeType), lreg);
            }
            else{
                generatedCode << std::format("\t{} {}\n", irNodeToString.at(nodeType), lreg);
            }
            generatedCode << std::format("\tmovq %rax, {}\n\n", rreg);
        }
        else if(nodeType == IRNodeType::AND || nodeType == IRNodeType::OR || nodeType == IRNodeType::XOR){
            generatedCode << std::format("\t{} {}, {}\n", irNodeToString.at(nodeType), lreg, rreg);
        }
        else if(nodeType == IRNodeType::SHL || nodeType == IRNodeType::SAL || nodeType == IRNodeType::SHR || nodeType == IRNodeType::SAR){
            generatedCode << std::format("\tmovq {}, %rcx\n", lreg);
            generatedCode << std::format("\t{} %rcx, {}\n", irNodeToString.at(nodeType), rreg);
        }
        else{
            generatedCode << std::format("\t{} {}, {}\n", irNodeToString.at(nodeType), lreg, rreg);
        }
        if(gpFreeRegPos >= gpRegisters.size()){
            generatedCode << std::format("\tpush {}\n", rreg);
        }
        takeGpReg();
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RELATIONAL EXPRESSION - generate cmp
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateRelationalExpression(std::shared_ptr<IRTree> node){
    generateNumericalExpression(node->getChild(0));
    generateNumericalExpression(node->getChild(1));
    
    freeGpReg();
    auto lreg = gpRegisters.at(gpFreeRegPos);
    freeGpReg();
    auto rreg = gpRegisters.at(gpFreeRegPos);
    generatedCode << std::format("\tcmp {}, {}\n", lreg, rreg);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ID - retrieve address from variableMapping
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::string CodeGenerator::generateID(std::shared_ptr<IRTree> node){
    return variableMapping.at(node->getName());
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// LITERAL - generate $literal
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::string CodeGenerator::generateLiteral(std::shared_ptr<IRTree> node){
    std::string val = node->getValue();
    if(node->getType() == Types::UNSIGNED){
        val.pop_back();
    }
    return std::format("${}", val);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION CALL - push arguments to stack, call function, pop arguments
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateFunctionCall(std::shared_ptr<IRTree> node){
    
    generateArgument(node->getChild(0));

    generatedCode << std::format("\tcall {}\n", node->getName());

    clearArguments(node->getChild(0));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ARGUMENT GENERATE - pushing arguments (numexp) 
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateArgument(std::shared_ptr<IRTree> node){
    for(int i = (int)node->getChildren().size()-1; i >= 0; i--){
        generateNumericalExpression(node->getChild(i));
        freeGpReg();
        generatedCode << std::format("\tpush {}\n", gpRegisters.at(gpFreeRegPos));
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ARGUMENT CLEAR - popping arguments
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::clearArguments(std::shared_ptr<IRTree> node){
    for(size_t i = 0; i < node->getChildren().size(); i++){
        generatedCode << std::format("\tpop {}\n", gpRegisters.at(gpFreeRegPos));
    }
}