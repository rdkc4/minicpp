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
    genStart();

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
    genNewLine();
    genLabel(activeFunction);

    // function prologue
    genPush("%rbp");
    genMov("%rsp", "%rbp");
    
    // allocation of local variables
    if(node->getValue() != "0"){
        genOperation("sub", std::format("${}",node->getValue()), "%rsp");
    }

    genNewLine();

    generateParameter(node->getChild(0));

    for(size_t i = 1; i < node->getChildren().size(); i++){
        generateConstruct(node->getChild(i));
    }

    //function label
    genLabel(std::format("{}_end", activeFunction));
    
    // free local variables 
    genOperation("add", std::format("${}", node->getValue()), "%rsp");

    // function epilogue
    genMov("%rbp", "%rsp");
    genPop("%rbp");

    if(node->getName() != "main"){
        genRet();
    }
    else{
        // system call for exit
        genExit();
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
        genMov("$0", generateID(node), "q");
    }
    genNewLine();

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
    genNewLine();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// IF STATEMENT - cmp relexp, opposite jmp to else/end, if constructs, else construct
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateIfStatement(std::shared_ptr<IRTree> node){
    size_t labNum = getNextLabelNum();
    size_t size = node->getChildren().size();

    genLabel(std::format("if{}", labNum));
    generateRelationalExpression(node->getChild(0));

    int type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    std::string jmpLabel = "";

    if(size > 3){
        jmpLabel = std::format("elif{}_0\n", labNum);
    }
    else if(size == 3){
        jmpLabel = std::format("else{}\n", labNum);
    }
    else{
        jmpLabel = std::format("if_end{}\n", labNum);
    }

    genJmp(stringToOppJMP.at(node->getChild(0)->getValue())[type], jmpLabel);

    generateStatement(node->getChild(1));
    genJmp("jmp", std::format("if{}_end", labNum));
    if(node->getChildren().size() == 3) genNewLine();

    for(size_t i = 2; i < size; i+=2){
        if(node->getChild(i)->getNodeType() == IRNodeType::CMP){
            genNewLine();
            genLabel(std::format("elif{}_{}", labNum, i/2-1));
            generateRelationalExpression(node->getChild(i));

            type = node->getChild(i)->getChild(0)->getType() == Types::INT ? 0 : 1;
            std::string jmpLabel = "";
            if(i+2 <= size){
                jmpLabel = std::format("if{}_end", labNum);
            }
            else if(i+2 == size+1){
                jmpLabel = std::format("else{}", labNum);
            }
            else{
                jmpLabel = std::format("elif{}_{}", labNum, i/2); 
            }

            genJmp(stringToOppJMP.at(node->getChild(i)->getValue())[type], jmpLabel);

            generateConstruct(node->getChild(i+1));
            genJmp("jmp", std::format("if{}_end", labNum));
        }
    }

    if(size % 2 == 1){
        genLabel(std::format("else{}", labNum));
        generateConstruct(node->getChildren().back());
    }
    genLabel(std::format("if{}_end", labNum));

}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// WHILE STATEMENT - cmp relexp, opposite jump to end, while constructs
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateWhileStatement(std::shared_ptr<IRTree> node){
    size_t labNum = getNextLabelNum();

    genLabel(std::format("while{}", labNum));
    generateRelationalExpression(node->getChild(0));

    int type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    genJmp(stringToOppJMP.at(node->getChild(0)->getValue())[type], std::format("while{}_end", labNum));
    genNewLine();

    generateConstruct(node->getChild(1));
    genJmp("jmp", std::format("while{}", labNum));
    genNewLine();
    genLabel(std::format("while{}_end", labNum));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FOR STATEMENT - assign(init) statement (before loop), cmp relexp, opposite jmp, constructs, assign(inc) statement
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateForStatement(std::shared_ptr<IRTree> node){
    size_t labNum = getNextLabelNum();

    generateAssignmentStatement(node->getChild(0));
    genNewLine();
    genLabel(std::format("for{}", labNum));
    generateRelationalExpression(node->getChild(1));

    int type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    genJmp(stringToOppJMP.at(node->getChild(1)->getValue())[type], std::format("for{}_end", labNum));
    genNewLine();

    generateConstruct(node->getChild(3));
    generateAssignmentStatement(node->getChild(2));
    genJmp("jmp", std::format("for{}", labNum));

    genNewLine();
    genLabel(std::format("for{}_end", labNum));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DO_WHILE STATEMENT - constructs, relexp regular jmp
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateDoWhileStatement(std::shared_ptr<IRTree> node){
    size_t labNum = getNextLabelNum();

    genLabel(std::format("do_while{}", labNum));
    generateConstruct(node->getChild(0));
    
    generateRelationalExpression(node->getChild(1));
    int type = node->getChild(1)->getChild(0)->getType() == Types::INT ? 0 : 1;
    genJmp(stringToJMP.at(node->getChild(1)->getValue())[type], std::format("do_while{}", labNum));
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
    genMov(gpRegisters.at(gpFreeRegPos), generateID(node->getChild(0)), "q");
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETURN STATEMENT - store numexp result at register %rax
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateReturnStatement(std::shared_ptr<IRTree> node){
    if(node->getChildren().size() != 0){
        generateNumericalExpression(node->getChild(0));
        freeGpReg();
        genMov(gpRegisters.at(gpFreeRegPos), "%rax", "q");
    }else{
        genOperation("xor", "%rax", "%rax");
    }
    genJmp("jmp", std::format("{}_end", activeFunction));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SWITCH STATEMENT - generate cases, cmp each case, opposite jmp to next case/default/end
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateSwitchStatement(std::shared_ptr<IRTree> node){
    size_t labNum = getNextLabelNum();

    genLabel(std::format("switch{}", labNum));

    size_t i = 1;
    size_t size = node->getChildren().size();
    std::string var = node->getChild(0)->getName();
    bool hasDefault = node->getChildren().back()->getNodeType() == IRNodeType::DEFAULT;
    
    for(; i < size; i++){
        std::shared_ptr<IRTree> child = node->getChild(i);
        if(child->getNodeType() == IRNodeType::CASE){
            genLabel(std::format("switch{}_case{}", labNum, i-1));
            genMov(variableMap.at(var), "%rcx", "q");
            genMov(generateLiteral(child->getChild(0)), "%rdx", "q");
            genCmp("%rcx", "%rdx");
            
            std::string jmpLabel = "";
            if(hasDefault){
                jmpLabel = std::format("switch{}_{}", labNum, (i < size-2 ?  "case" + std::to_string(i) : "default"));
            }
            else{
                jmpLabel = std::format("switch{}_{}", labNum, (i < size-1 ?  "case" + std::to_string(i) : "end"));
            }
            genJmp("jne", jmpLabel);

            for(size_t j = 1; j < child->getChildren().size(); ++j){
                generateConstruct(child->getChild(j));
            }

            if(child->getChildren().size()==3){
                genJmp("jmp", std::format("switch{}_end", labNum));
            }

        }else{
            genLabel(std::format("switch{}_default", labNum));
            for(size_t j = 0; j < child->getChildren().size(); ++j){
                generateConstruct(child->getChild(j));
            }
        }
    }
    genLabel(std::format("switch{}_end", labNum));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// NUMERICAL EXPRESSION - evaluating equations using general-purpose registers r(8-15)
// -> maybe use queue instead of a tree for numexp, easier generation, better reg usage (TODO?)
// -> needs rework
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::generateNumericalExpression(std::shared_ptr<IRTree> node){
    if(node->getNodeType() == IRNodeType::ID){
        if(gpFreeRegPos < gpRegisters.size()){
            genMov(generateID(node), gpRegisters.at(gpFreeRegPos), "q");
        }
        else{
            genPush(generateID(node));
        }
        takeGpReg();
    }
    else if(node->getNodeType() == IRNodeType::LITERAL){
        if(gpFreeRegPos < gpRegisters.size()){
            genMov(generateLiteral(node), gpRegisters.at(gpFreeRegPos), "q");
        }
        else{
            genPush(generateLiteral(node));
        }
        takeGpReg();
    }else if (node->getNodeType() == IRNodeType::CALL){
        generateFunctionCall(node);
        if(gpFreeRegPos < gpRegisters.size()){
            genMov("%rax", gpRegisters.at(gpFreeRegPos), "q");
        }
        else{
            genPush("%rax");
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
            genPop(lreg);
        }
        else{
            lreg = gpRegisters.at(gpFreeRegPos);
        }
        freeGpReg();
        if(gpFreeRegPos >= gpRegisters.size()){
            rreg = "%rsi";
            genPop(rreg);
        }
        else{
            rreg = gpRegisters.at(gpFreeRegPos);
        }
        
        IRNodeType nodeType = node->getNodeType();
        if(nodeType == IRNodeType::MUL || nodeType == IRNodeType::DIV){ // result of MUL || DIV is in RDX:RAX
            genOperation("xor", "%rdx", "%rdx"); //add overflow check (TODO)
            genMov(rreg, "%rax", "q");
            if(node->getType().value() == Types::INT){
                genOperation(std::format("i{}", irNodeToString.at(nodeType)), lreg);
            }
            else{
                genOperation(irNodeToString.at(nodeType), lreg);
            }
            genMov("%rax", rreg, "q");
            genNewLine();
        }
        else if(nodeType == IRNodeType::AND || nodeType == IRNodeType::OR || nodeType == IRNodeType::XOR){
            genOperation(irNodeToString.at(nodeType), lreg, rreg);
        }
        else if(nodeType == IRNodeType::SHL || nodeType == IRNodeType::SAL || nodeType == IRNodeType::SHR || nodeType == IRNodeType::SAR){
            genMov(lreg, "%rcx", "q");
            genOperation(irNodeToString.at(nodeType), "%rcx", rreg);
        }
        else{
            genOperation(irNodeToString.at(nodeType), lreg, rreg);
        }
        if(gpFreeRegPos >= gpRegisters.size()){
            genPush(rreg);
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
    genCmp(lreg, rreg);
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

    genCall(node->getName());

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
        if(gpFreeRegPos < gpRegisters.size()){ // if >= gpRegisters.size() argument is already pushed
            genPush(gpRegisters.at(gpFreeRegPos));
        }
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ARGUMENT CLEAR - popping arguments
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CodeGenerator::clearArguments(std::shared_ptr<IRTree> node){
    // popping arguments of the stack
    for(size_t i = 0; i < node->getChildren().size(); i++){
        genPop("%rbx");
    }
}
