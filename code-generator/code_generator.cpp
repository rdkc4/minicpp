#include "code_generator.hpp"
#include <memory>
#include <string>
#include <format>

#include "defs/code_generator_defs.hpp"

CodeGenerator::CodeGenerator(std::string& output) : generatedCode(output), labelNum(0), gpFreeRegPos(0){}

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

    // start of asm code
    generatedCode << ".global _start\n";
    generatedCode << ".text\n\n";
    generatedCode << "_start:\n";
    generatedCode << "\tjmp main\n";

    for(const auto& child : root->getChildren()){
        variableNum = 1;
        generateFunction(child);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// HANDLING STACK - obtaining variables, parameters, generating statements
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateFunction(std::shared_ptr<IRTree> node){
    activeFunction = node->getName();
    generatedCode << std::format("\n{}:\n", activeFunction);

    // function prologue
    generatedCode << "\tpush %rbp\n";
    generatedCode << "\tmov %rsp, %rbp\n";
    
    // allocation of local variables
    if(node->getValue() != "0"){
        generatedCode << std::format("\tsub ${}, %rsp\n\n", node->getValue());
    }

    generateParameter(node->getChild(0));

    for(size_t i = 1; i < node->getChildren().size(); i++){
        generateConstruct(node->getChild(i));
    }

    generatedCode << std::format("{}_end:\n", activeFunction);
    
    // free local variables 
    generatedCode << std::format("\tadd ${}, %rsp\n", node->getValue());

    // function epilogue
    generatedCode << "\tmov %rbp, %rsp\n";
    generatedCode << "\tpop %rbp\n";

    if(node->getName() != "main"){
        generatedCode << "\tret\n";
    }
    else{
        // system call for exit
        generatedCode << "\tmov %rax, %rdi\n"; // return value to rdi
        generatedCode << "\tmovq $60, %rax\n";
        generatedCode << "\tsyscall\n";
    }

    variableMap.clear();

}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// INSERTING PARAMETERS
// -> maybe switch to 32bit registers instead of 64 (TODO?)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateParameter(std::shared_ptr<IRTree> node){
    size_t i = 2;
    for(const auto& parameter : node->getChildren()){
        // mapping parameter to address relative to %rbp (+n(%rbp))
        variableMap.insert({parameter->getName(), std::format("{}(%rbp)", i*8)});
        ++i;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// generating variable / statement
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateConstruct(std::shared_ptr<IRTree> node){
    if(node->getNodeType() == IRNodeType::VARIABLE){
        generateVariable(node);
    }
    else{
        generateStatement(node);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// INSERTING LOCAL VARIABLES - assigning default values (0) or given values (n)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateVariable(std::shared_ptr<IRTree> node){
    // mapping local variable to address relative to %rbp (-n(%rbp))
    // if not successful it means that variable with the given name existed but went out of scope, so it overwrites it with new memory location
    auto [varPtr, success] = variableMap.insert({node->getName(), std::format("-{}(%rbp)", variableNum*8)});
    if(!success){
        varPtr->second = std::format("-{}(%rbp)", variableNum*8);
    }
    ++variableNum;

    // direct initialization / default value assignation
    if(node->getChildren().size() != 0){
        generateAssignmentStatement(node->getChild(0));
    }
    else{
        generatedCode << std::format("\tmovq $0, {}\n", generateID(node));
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
// IF STATEMENT - cmp relexp, opposite jmp to else/end, if constructs, else construct
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateIfStatement(std::shared_ptr<IRTree> node){
    size_t labNum = getNextLabelNum();
    size_t size = node->getChildren().size();

    generatedCode << std::format("if{}:\n", labNum);
    generateRelationalExpression(node->getChild(0));

    int type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
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

            generateConstruct(node->getChild(i+1));
            generatedCode << std::format("\tjmp if{}_end\n", labNum); 
        }
    }

    if(size % 2 == 1){
        generatedCode << std::format("\nelse{}:\n", labNum);
        generateConstruct(node->getChildren().back());
    }

    generatedCode << std::format("if{}_end:\n", labNum);

}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// WHILE STATEMENT - cmp relexp, opposite jump to end, while constructs
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateWhileStatement(std::shared_ptr<IRTree> node){
    size_t labNum = getNextLabelNum();

    generatedCode << std::format("while{}:\n", labNum);
    generateRelationalExpression(node->getChild(0));

    int type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    generatedCode << std::format("\t{} while{}_end\n\n", stringToOppJMP.at(node->getChild(0)->getValue())[type], labNum);

    generateConstruct(node->getChild(1));
    generatedCode << std::format("\tjmp while{}\n", labNum);
    generatedCode << std::format("\nwhile{}_end:\n", labNum);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FOR STATEMENT - assign(init) statement (before loop), cmp relexp, opposite jmp, constructs, assign(inc) statement
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateForStatement(std::shared_ptr<IRTree> node){
    size_t labNum = getNextLabelNum();

    generateAssignmentStatement(node->getChild(0));
    generatedCode << std::format("\nfor{}:\n", labNum);
    generateRelationalExpression(node->getChild(1));

    int type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    generatedCode << std::format("\t{} for{}_end\n\n", stringToOppJMP.at(node->getChild(1)->getValue())[type], labNum);

    generateConstruct(node->getChild(3));
    generateAssignmentStatement(node->getChild(2));
    generatedCode << std::format("\tjmp for{}\n", labNum);

    generatedCode << std::format("\nfor{}_end:\n", labNum);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DO_WHILE STATEMENT - constructs, relexp regular jmp
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateDoWhileStatement(std::shared_ptr<IRTree> node){
    size_t labNum = getNextLabelNum();

    generatedCode << std::format("do_while{}:\n", labNum);
    generateConstruct(node->getChild(0));
    
    generateRelationalExpression(node->getChild(1));
    int type = node->getChild(1)->getChild(0)->getType() == Types::INT ? 0 : 1;
    generatedCode << std::format("\t{} do_while{}\n", stringToJMP.at(node->getChild(1)->getValue())[type], labNum);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// COMPOUND STATEMENT - constructs
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateCompoundStatement(std::shared_ptr<IRTree> node){
    for(const auto& child : node->getChildren()){
        generateConstruct(child);
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
    size_t labNum = getNextLabelNum();

    generatedCode << std::format("switch{}:\n", labNum);

    size_t i = 1;
    size_t size = node->getChildren().size();
    std::string var = node->getChild(0)->getName();
    bool hasDefault = node->getChildren().back()->getNodeType() == IRNodeType::DEFAULT;
    
    for(; i < size; i++){
        std::shared_ptr<IRTree> child = node->getChild(i);
        if(child->getNodeType() == IRNodeType::CASE){
            generatedCode << std::format("switch{}_case{}:\n", labNum, i-1);
            generatedCode << std::format("\tmovq {}, %rcx\n", variableMap.at(var));
            generatedCode << std::format("\tmovq {}, %rdx\n", generateLiteral(child->getChild(0)));
            generatedCode << "\tcmp %rcx, %rdx\n";
            
            if(hasDefault){
                generatedCode << std::format("\tjne switch{}_{}\n", labNum, (i < size-2 ?  "case" + std::to_string(i) : "default"));
            }
            else{
                generatedCode << std::format("\tjne switch{}_{}\n", labNum, (i < size-1 ?  "case" + std::to_string(i) : "end"));
            }

            for(size_t j = 1; j < child->getChildren().size(); ++j){
                generateConstruct(child->getChild(j));
            }

            if(child->getChildren().size()==3){
                generatedCode << std::format("\tjmp switch{}_end\n", labNum);
            }

        }else{
            generatedCode << std::format("switch{}_default:\n", labNum);
            for(size_t j = 0; j < child->getChildren().size(); ++j){
                generateConstruct(child->getChild(j));
            }
        }
    }
    generatedCode << std::format("switch{}_end:\n", labNum);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// NUMERICAL EXPRESSION - evaluating equations using general-purpose registers r(8-15)
// -> maybe use queue instead of a tree for numexp, easier generation, better reg usage (TODO?)
// -> needs rework
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
        
        IRNodeType nodeType = node->getNodeType();
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
    std::string lreg = gpRegisters.at(gpFreeRegPos);
    freeGpReg();
    std::string rreg = gpRegisters.at(gpFreeRegPos);
    generatedCode << std::format("\tcmp {}, {}\n", lreg, rreg);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ID - retrieve address from variableMap
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::string CodeGenerator::generateID(std::shared_ptr<IRTree> node){
    return variableMap.at(node->getName());
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
    // pushing arguments onto stack
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
    // popping arguments of the stack
    for(size_t i = 0; i < node->getChildren().size(); i++){
        generatedCode << std::format("\tpop {}\n", gpRegisters.at(gpFreeRegPos));
    }
}
