#include "code_generator.hpp"

CodeGenerator::CodeGenerator() : generatedCode("output.s"){}

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
    generatedCode << "\n" << activeFunction << ":\n";
    generatedCode << "\tpush %rbp\n";
    generatedCode << "\tmov %rsp, %rbp\n";

    generateParameter(node->getChild(0));

    generateVariable(node->getChild(1));

    for(size_t i = 2; i < node->getChildren().size(); i++){
        generateStatement(node->getChild(i));
    }

    generatedCode << activeFunction << "_end:\n";
    
    size_t varCount = node->getChild(1)->getChildren().size();
    if(varCount > 0){
        generatedCode << "\tadd $" << varCount*8 << ", %rsp\n";
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
        variableMapping.insert({parameter->getName(), std::to_string(i*8) + "(%rbp)"});
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
    generatedCode << "\tsub $" << varCount*8 << ", %rsp\n\n";

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
            throw std::runtime_error("Invalid statement " + irNodeToString.at(node->getNodeType()));
    }
    generatedCode << "\n";
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// IF STATEMENT - cmp relexp, opposite jmp to else/end, if statement, else statement
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateIfStatement(std::shared_ptr<IRTree> node){
    auto labNum = getNextLabelNum();
    auto size = node->getChildren().size();

    generatedCode << "if" << labNum << ":\n";
    generateRelationalExpression(node->getChild(0));

    auto type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    generatedCode << "\t" + stringToOppJMP.at(node->getChild(0)->getValue())[type];
    if(size > 3){
        generatedCode << " elif" << labNum << "_0\n";
    }
    else if(size == 3){
        generatedCode << " else" << labNum << "\n";
    }
    else{
        generatedCode << " if_end" << labNum << "\n";
    }
    generatedCode << "\n";

    generateStatement(node->getChild(1));
    generatedCode << "\tjmp if" << labNum << "_end\n";

    for(size_t i = 2; i < size; i+=2){
        if(node->getChild(i)->getNodeType() == IRNodeType::CMP){
            generatedCode << "\nelif" << labNum << "_" << i/2-1 << ":\n";
            generateRelationalExpression(node->getChild(i));

            type = node->getChild(i)->getChild(0)->getType() == Types::INT ? 0 : 1;
            generatedCode << "\t" << stringToOppJMP.at(node->getChild(i)->getValue())[type];
            if(i+2 <= size){
                generatedCode << " if" << labNum << "_end\n";
            }
            else if(i+2 == size+1){
                generatedCode << " else" << labNum << "\n";
            }
            else{
                generatedCode << " elif" << labNum << "_" << i/2 << "\n"; 
            }

            generateStatement(node->getChild(i+1));
            generatedCode << "\tjmp if" << labNum << "_end\n"; 
        }
    }

    if(size % 2 == 1){
        generatedCode << "\nelse" << labNum << ":\n";
        generateStatement(node->getChildren().back());
    }

    generatedCode << "if" << labNum << "_end:\n";

}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// WHILE STATEMENT - cmp relexp, opposite jump to end, while statements
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateWhileStatement(std::shared_ptr<IRTree> node){
    auto labNum = getNextLabelNum();

    generatedCode << "while" << labNum << ":\n";
    generateRelationalExpression(node->getChild(0));

    auto type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    generatedCode << "\t" << stringToOppJMP.at(node->getChild(0)->getValue())[type] << " while" << labNum << "_end\n\n";

    generateStatement(node->getChild(1));
    generatedCode << "\tjmp while" << labNum << "\n";
    generatedCode << "\nwhile" << labNum << "_end:\n";
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FOR STATEMENT - assign(init) statement (before loop), cmp relexp, opposite jmp, statements, assign(inc) statement
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateForStatement(std::shared_ptr<IRTree> node){
    auto labNum = getNextLabelNum();

    generateAssignmentStatement(node->getChild(0));
    generatedCode << "\nfor" << labNum << ":\n";
    generateRelationalExpression(node->getChild(1));

    auto type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    generatedCode << "\t" << stringToOppJMP.at(node->getChild(1)->getValue())[type] << " for" << labNum << "_end\n\n";

    generateStatement(node->getChild(3));
    generateAssignmentStatement(node->getChild(2));
    generatedCode << "\tjmp for" << labNum << "\n";

    generatedCode << "\nfor" << labNum << "_end:\n";  

}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DO_WHILE STATEMENT - statements, relexp regular jmp
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateDoWhileStatement(std::shared_ptr<IRTree> node){
    auto labNum = getNextLabelNum();

    generatedCode << "do_while" << labNum << ":\n";
    generateStatement(node->getChild(0));
    
    generateRelationalExpression(node->getChild(1));
    auto type = node->getChild(1)->getChild(0)->getType() == Types::INT ? 0 : 1;
    generatedCode << "\t" << stringToJMP.at(node->getChild(1)->getValue())[type] << " do_while" << labNum << "\n";
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
    generatedCode << "\tmovq " << gpRegisters.at(gpFreeRegPos) << ", ";
    generateID(node->getChild(0));
    generatedCode << "\n";
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETURN STATEMENT - store numexp result at register %rax
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateReturnStatement(std::shared_ptr<IRTree> node){
    if(node->getChildren().size() != 0){
        generateNumericalExpression(node->getChild(0));
        freeGpReg();
        generatedCode << "\tmovq " << gpRegisters.at(gpFreeRegPos) << ", %rax\n";
    }else{
        generatedCode << "\txor %rax, %rax\n";
    }
    generatedCode << "\tjmp " << activeFunction << "_end\n";
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SWITCH STATEMENT - generate cases, cmp each case, opposite jmp to next case/default/end
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateSwitchStatement(std::shared_ptr<IRTree> node){
    auto labNum = getNextLabelNum();

    generatedCode << "switch" << labNum << ":\n";

    size_t i = 1;
    auto size = node->getChildren().size();
    auto var = node->getChild(0)->getName();
    bool hasDefault = node->getChildren().back()->getNodeType() == IRNodeType::DEFAULT;
    
    for(; i < size; i++){
        auto child = node->getChild(i);
        if(child->getNodeType() == IRNodeType::CASE){
            generatedCode << "switch" << labNum << "_case" << i-1 << ":\n";
            generatedCode << "\tmovq " << variableMapping.at(var) << ", %rcx\n";
            generatedCode << "\tmovq ";
            generateLiteral(child->getChild(0));
            generatedCode << ", %rdx\n";
            generatedCode << "\tcmp %rcx, %rdx\n";
            
            if(hasDefault){
                generatedCode << "\tjne switch" << labNum << "_" << (i < size-2 ?  "case" + std::to_string(i) : "default") << '\n';
            }
            else{
                generatedCode << "\tjne switch" << labNum << "_" << (i < size-1 ?  "case" + std::to_string(i) : "end") << '\n';
            }

            generateStatement(child->getChild(1));
            if(child->getChildren().size()==3){
                generatedCode << "\tjmp switch" << labNum << "_end\n";
            }

        }else{
            generatedCode << "switch" << labNum << "_default:\n";
            generateStatement(node->getChild(i)->getChild(0));
        }
    }
    generatedCode << "switch" << labNum << "_end:\n";
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// NUMERICAL EXPRESSION - evaluating equations using general-purpose registers r(8-15)
// -> maybe use queue instead of a tree for numexp, easier generation, better reg usage (TODO?)
// -> needs rework, possible invalid allocation -> fix (TODO) 
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateNumericalExpression(std::shared_ptr<IRTree> node){
    if(node->getNodeType() == IRNodeType::ID){
        if(gpFreeRegPos < gpRegisters.size()){
            generatedCode << "\tmovq ";
            generateID(node);
            generatedCode << ", " << gpRegisters.at(gpFreeRegPos) << "\n";
        }
        else{
            generatedCode << "\tpush ";
            generateID(node);
            generatedCode << "\n";
        }
        takeGpReg();
    }
    else if(node->getNodeType() == IRNodeType::LITERAL){
        if(gpFreeRegPos < gpRegisters.size()){
            generatedCode << "\tmovq ";
            generateLiteral(node);
            generatedCode << ", " << gpRegisters.at(gpFreeRegPos) << "\n";
        }
        else{
            generatedCode << "\tpush ";
            generateLiteral(node);
            generatedCode << "\n";
        }
        takeGpReg();
    }else if (node->getNodeType() == IRNodeType::CALL){
        generateFunctionCall(node);
        if(gpFreeRegPos < gpRegisters.size()){
            generatedCode << "\tmovq %rax, " << gpRegisters.at(gpFreeRegPos) << "\n";
        }
        else{
            generatedCode << "\tpush %rax\n";
        }
        takeGpReg();
    }
    else{
        std::string rbxsub;
        std::string raxsub;

        if(irOperators.find(node->getChild(0)->getNodeType()) == irOperators.end() && irOperators.find(node->getChild(1)->getNodeType()) != irOperators.end()){
            generateNumericalExpression(node->getChild(1));
            generateNumericalExpression(node->getChild(0));

            freeGpReg();
            if(gpFreeRegPos >= gpRegisters.size()){
                raxsub = "%rsi";
                generatedCode << "\tpop %rsi\n";
            }
            else{
                raxsub = gpRegisters.at(gpFreeRegPos);
            }
            freeGpReg();
            if(gpFreeRegPos >= gpRegisters.size()){
                rbxsub = "%rdi";
                generatedCode << "\tpop %rdi\n";
            }
            else{
                rbxsub = gpRegisters.at(gpFreeRegPos);
            }
        }
        else{
            generateNumericalExpression(node->getChild(0));
            generateNumericalExpression(node->getChild(1));
            
            freeGpReg();
            if(gpFreeRegPos >= gpRegisters.size()){
                rbxsub = "%rdi";
                generatedCode << "\tpop %rdi\n";
            }
            else{
                rbxsub = gpRegisters.at(gpFreeRegPos);
            }
            freeGpReg();
            if(gpFreeRegPos >= gpRegisters.size()){
                raxsub = "%rsi";
                generatedCode << "\tpop %rsi\n";
            }
            else{
                raxsub = gpRegisters.at(gpFreeRegPos);
            }
        }

        if(node->getNodeType() == IRNodeType::MUL || node->getNodeType() == IRNodeType::DIV){ // result of MUL || DIV is in RDX:RAX
            generatedCode << "\txor %rdx, %rdx\n"; //add overflow check (TODO)
            generatedCode << "\tmovq " << raxsub << ", %rax\n";
            if(node->getType().value() == Types::INT){
                generatedCode << "\ti" << irNodeToString.at(node->getNodeType()) << " " << rbxsub << "\n"; 
            }
            else{
                generatedCode << "\t" << irNodeToString.at(node->getNodeType()) << " " << rbxsub << "\n";
            }
            generatedCode << "\tmovq %rax, " << raxsub << "\n\n";
        }
        else if(node->getNodeType() == IRNodeType::AND || node->getNodeType() == IRNodeType::OR || node->getNodeType() == IRNodeType::XOR){
            generatedCode << "\t" << irNodeToString.at(node->getNodeType()) << " " << rbxsub << ", " << raxsub << "\n";
        }
        else{
            generatedCode << "\t" << irNodeToString.at(node->getNodeType()) << " " << rbxsub << ", " << raxsub << "\n";
        }
        if(gpFreeRegPos >= gpRegisters.size()){
            generatedCode << "\tpush " << raxsub << "\n";
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
    generatedCode << "\tcmp " << gpRegisters.at(gpFreeRegPos) << ", ";
    freeGpReg();
    generatedCode << gpRegisters.at(gpFreeRegPos) << "\n";
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ID - retrieve address from variableMapping
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateID(std::shared_ptr<IRTree> node){
    generatedCode << variableMapping.at(node->getName());
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// LITERAL - generate $literal
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateLiteral(std::shared_ptr<IRTree> node){
    std::string val = node->getValue();
    if(node->getType() == Types::UNSIGNED){
        val.pop_back();
    }
    generatedCode << "$" << val;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION CALL - push arguments to stack, call function, pop arguments
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateFunctionCall(std::shared_ptr<IRTree> node){
    
    generateArgument(node->getChild(0));

    generatedCode << "\tcall " << node->getName() << '\n';

    clearArguments(node->getChild(0));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ARGUMENT GENERATE - pushing arguments (numexp) 
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateArgument(std::shared_ptr<IRTree> node){
    for(int i = (int)node->getChildren().size()-1; i >= 0; i--){
        generateNumericalExpression(node->getChild(i));
        freeGpReg();
        generatedCode << "\tpush " << gpRegisters.at(gpFreeRegPos) << "\n";
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ARGUMENT CLEAR - popping arguments
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::clearArguments(std::shared_ptr<IRTree> node){
    for(size_t i = 0; i < node->getChildren().size(); i++){
        generatedCode << "\tpop " << gpRegisters.at(gpFreeRegPos) << "\n";
    }
}