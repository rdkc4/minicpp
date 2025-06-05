#include "code_generator.hpp"
#include <memory>
#include <string>
#include <format>

#include "../defs/code_generator_defs.hpp"

CodeGenerator::CodeGenerator(std::string& file) : _asm{ file }, labelNum{ 0 }, gpFreeRegPos{ 0 } {}

// allocating general-purpose register r(8-15)
void CodeGenerator::takeGpReg() noexcept {
    ++gpFreeRegPos;    
}

// releasing general-purpose register r(8-15)
void CodeGenerator::freeGpReg() noexcept {
    --gpFreeRegPos;
}

size_t CodeGenerator::getNextLabelNum() noexcept {
    return labelNum++;
}

void CodeGenerator::generateCode(const IRTree* root){
    if(!_asm.isOpen()){
        throw std::runtime_error("File failed to generate");
    }

    // start of asm code
    _asm.genStart();

    for(const auto& child : root->getChildren()){
        variableNum = 1;
        generateFunction(child.get());
    }
}

void CodeGenerator::generateFunction(const IRTree* node){
    activeFunction = node->getName();
    _asm.genNewLine();

    // function label
    _asm.genLabel(activeFunction);

    _asm.genFuncPrologue();
    
    // allocation of local variables
    if(node->getValue() != "0"){
        _asm.genOperation("sub", std::format("${}",node->getValue()), "%rsp");
    }

    _asm.genNewLine();

    generateParameter(node->getChild(0));

    for(size_t i = 1; i < node->getChildren().size(); i++){
        generateConstruct(node->getChild(i));
    }

    // function end label
    _asm.genLabel(std::format("{}_end", activeFunction));
    
    // free local variables 
    if(node->getValue() != "0"){
        _asm.genOperation("add", std::format("${}", node->getValue()), "%rsp");
    }

    _asm.genFuncEpilogue();

    if(node->getName() != "main"){
        _asm.genRet();
    }
    else{
        _asm.genExit();
    }

    variableMap.clear();
}

void CodeGenerator::generateParameter(const IRTree* node){
    size_t i{ 2 };
    for(const auto& parameter : node->getChildren()){
        // mapping parameter to address relative to %rbp (+n(%rbp))
        variableMap.insert({parameter->getName(), std::format("{}(%rbp)", i * regSize)});
        ++i;
    }
}

void CodeGenerator::generateConstruct(const IRTree* node){
    if(node->getNodeType() == IRNodeType::VARIABLE){
        generateVariable(node);
    }
    else{
        generateStatement(node);
    }
}

void CodeGenerator::generateVariable(const IRTree* node){
    // mapping local variable to address relative to %rbp (-n(%rbp))
    // if not successful it means that variable with the given name existed but went out of scope, so it overwrites it with new memory location
    auto [varPtr, success]{ variableMap.insert({node->getName(), std::format("-{}(%rbp)", variableNum * regSize)}) };
    if(!success){
        varPtr->second = std::format("-{}(%rbp)", variableNum * regSize);
    }
    ++variableNum;

    // direct initialization / default value assignation
    if(node->getChildren().size() != 0){
        generateAssignmentStatement(node->getChild(0));
    }
    else{
        // default value 
        _asm.genMov("$0", generateID(node), "q");
    }
    _asm.genNewLine();

}

void CodeGenerator::generateStatement(const IRTree* node){
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
    _asm.genNewLine();
}

void CodeGenerator::generateIfStatement(const IRTree* node){
    size_t labNum{ getNextLabelNum() };
    size_t size{ node->getChildren().size() };

    _asm.genLabel(std::format("if{}", labNum));
    generateRelationalExpression(node->getChild(0));

    int type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    std::string jmpLabel{ "" };

    if(size > 3){
        jmpLabel = std::format("elif{}_0\n", labNum);
    }
    else if(size == 3){
        jmpLabel = std::format("else{}\n", labNum);
    }
    else{
        jmpLabel = std::format("if_end{}\n", labNum);
    }

    _asm.genJmp(stringToOppJMP.at(node->getChild(0)->getValue())[type], jmpLabel);

    generateStatement(node->getChild(1));
    _asm.genJmp("jmp", std::format("if{}_end", labNum));
    if(node->getChildren().size() == 3) _asm.genNewLine();

    for(size_t i = 2; i < size; i+=2){
        if(node->getChild(i)->getNodeType() == IRNodeType::CMP){
            _asm.genNewLine();
            _asm.genLabel(std::format("elif{}_{}", labNum, i/2-1));
            generateRelationalExpression(node->getChild(i));

            type = node->getChild(i)->getChild(0)->getType() == Types::INT ? 0 : 1;
            std::string jmpLabel{ "" };
            if(i+2 <= size){
                jmpLabel = std::format("if{}_end", labNum);
            }
            else if(i+2 == size+1){
                jmpLabel = std::format("else{}", labNum);
            }
            else{
                jmpLabel = std::format("elif{}_{}", labNum, i/2); 
            }

            _asm.genJmp(stringToOppJMP.at(node->getChild(i)->getValue())[type], jmpLabel);

            generateConstruct(node->getChild(i+1));
            _asm.genJmp("jmp", std::format("if{}_end", labNum));
        }
    }

    if(size % 2 == 1){
        _asm.genLabel(std::format("else{}", labNum));
        generateConstruct(node->getChildren().back().get());
    }
    _asm.genLabel(std::format("if{}_end", labNum));

}

void CodeGenerator::generateWhileStatement(const IRTree* node){
    size_t labNum{ getNextLabelNum() };

    _asm.genLabel(std::format("while{}", labNum));
    generateRelationalExpression(node->getChild(0));

    int type{ node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1 };
    _asm.genJmp(stringToOppJMP.at(node->getChild(0)->getValue())[type], std::format("while{}_end", labNum));
    _asm.genNewLine();

    generateConstruct(node->getChild(1));
    _asm.genJmp("jmp", std::format("while{}", labNum));
    _asm.genNewLine();
    _asm.genLabel(std::format("while{}_end", labNum));
}

void CodeGenerator::generateForStatement(const IRTree* node){
    size_t labNum{ getNextLabelNum() };

    // initializer
    generateAssignmentStatement(node->getChild(0));
    _asm.genNewLine();
    _asm.genLabel(std::format("for{}", labNum));
    generateRelationalExpression(node->getChild(1));

    // condition
    int type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    _asm.genJmp(stringToOppJMP.at(node->getChild(1)->getValue())[type], std::format("for{}_end", labNum));
    _asm.genNewLine();

    generateConstruct(node->getChild(3));

    // increment
    generateAssignmentStatement(node->getChild(2));
    _asm.genJmp("jmp", std::format("for{}", labNum));

    _asm.genNewLine();
    _asm.genLabel(std::format("for{}_end", labNum));
}

void CodeGenerator::generateDoWhileStatement(const IRTree* node){
    size_t labNum{ getNextLabelNum() };

    _asm.genLabel(std::format("do_while{}", labNum));
    generateConstruct(node->getChild(0));
    
    generateRelationalExpression(node->getChild(1));
    int type{ node->getChild(1)->getChild(0)->getType() == Types::INT ? 0 : 1 };
    _asm.genJmp(stringToJMP.at(node->getChild(1)->getValue())[type], std::format("do_while{}", labNum));
}

void CodeGenerator::generateCompoundStatement(const IRTree* node){
    for(const auto& child : node->getChildren()){
        generateConstruct(child.get());
    }
}

void CodeGenerator::generateAssignmentStatement(const IRTree* node){
    // evaluating rvalue
    generateNumericalExpression(node->getChild(1));
    freeGpReg();
    _asm.genMov(gpRegisters.at(gpFreeRegPos), generateID(node->getChild(0)), "q");
}

void CodeGenerator::generateReturnStatement(const IRTree* node){
    // assign return value to %rax
    if(node->getChildren().size() != 0){
        generateNumericalExpression(node->getChild(0));
        freeGpReg();
        _asm.genMov(gpRegisters.at(gpFreeRegPos), "%rax", "q");
    }else{
        _asm.genOperation("xor", "%rax", "%rax");
    }
    _asm.genJmp("jmp", std::format("{}_end", activeFunction));
}

void CodeGenerator::generateSwitchStatement(const IRTree* node){
    size_t labNum{ getNextLabelNum() };

    _asm.genLabel(std::format("switch{}", labNum));

    size_t i{ 1 };
    size_t size{ node->getChildren().size() };
    std::string var{ node->getChild(0)->getName() };
    bool hasDefault{ node->getChildren().back()->getNodeType() == IRNodeType::DEFAULT };
    
    // cases
    for(; i < size; i++){
        IRTree* child = node->getChild(i);
        if(child->getNodeType() == IRNodeType::CASE){
            _asm.genLabel(std::format("switch{}_case{}", labNum, i-1));
            _asm.genMov(variableMap.at(var), "%rcx", "q");
            _asm.genMov(generateLiteral(child->getChild(0)), "%rdx", "q");
            _asm.genCmp("%rcx", "%rdx");
            
            std::string jmpLabel = "";
            if(hasDefault){
                jmpLabel = std::format("switch{}_{}", labNum, (i < size-2 ?  "case" + std::to_string(i) : "default"));
            }
            else{
                jmpLabel = std::format("switch{}_{}", labNum, (i < size-1 ?  "case" + std::to_string(i) : "end"));
            }
            _asm.genJmp("jne", jmpLabel);

            for(size_t j = 1; j < child->getChildren().size(); ++j){
                generateConstruct(child->getChild(j));
            }

            if(child->getChildren().size()==3){
                _asm.genJmp("jmp", std::format("switch{}_end", labNum));
            }

        }else{
            _asm.genLabel(std::format("switch{}_default", labNum));
            for(size_t j = 0; j < child->getChildren().size(); ++j){
                generateConstruct(child->getChild(j));
            }
        }
    }
    _asm.genLabel(std::format("switch{}_end", labNum));
}

// evaluating equations using general-purpose registers r(8-15) and stack (if necessary)
void CodeGenerator::generateNumericalExpression(const IRTree* node){
    if(node->getNodeType() == IRNodeType::ID){
        if(gpFreeRegPos < gpRegisters.size()){
            _asm.genMov(generateID(node), gpRegisters.at(gpFreeRegPos), "q");
        }
        else{
            _asm.genPush(generateID(node));
        }
        takeGpReg();
    }
    else if(node->getNodeType() == IRNodeType::LITERAL){
        if(gpFreeRegPos < gpRegisters.size()){
            _asm.genMov(generateLiteral(node), gpRegisters.at(gpFreeRegPos), "q");
        }
        else{
            _asm.genPush(generateLiteral(node));
        }
        takeGpReg();
    }else if (node->getNodeType() == IRNodeType::CALL){
        generateFunctionCall(node);
        if(gpFreeRegPos < gpRegisters.size()){
            _asm.genMov("%rax", gpRegisters.at(gpFreeRegPos), "q");
        }
        else{
            _asm.genPush("%rax");
        }
        takeGpReg();
    }
    else{
        std::string lreg{};
        std::string rreg{};
        
        generateNumericalExpression(node->getChild(0));
        generateNumericalExpression(node->getChild(1));
        
        freeGpReg();
        if(gpFreeRegPos >= gpRegisters.size()){
            lreg = "%rdi";
            _asm.genPop(lreg);
        }
        else{
            lreg = gpRegisters.at(gpFreeRegPos);
        }
        freeGpReg();
        if(gpFreeRegPos >= gpRegisters.size()){
            rreg = "%rsi";
            _asm.genPop(rreg);
        }
        else{
            rreg = gpRegisters.at(gpFreeRegPos);
        }
        
        IRNodeType nodeType = node->getNodeType();
        if(nodeType == IRNodeType::MUL || nodeType == IRNodeType::DIV){ // result of MUL || DIV is in RDX:RAX
            _asm.genOperation("xor", "%rdx", "%rdx"); //add overflow check (TODO)
            _asm.genMov(rreg, "%rax", "q");
            if(node->getType() == Types::INT){
                _asm.genOperation(std::format("i{}", irNodeToString.at(nodeType)), lreg);
            }
            else{
                _asm.genOperation(irNodeToString.at(nodeType), lreg);
            }
            _asm.genMov("%rax", rreg, "q");
            _asm.genNewLine();
        }
        else if(nodeType == IRNodeType::AND || nodeType == IRNodeType::OR || nodeType == IRNodeType::XOR){
            _asm.genOperation(irNodeToString.at(nodeType), lreg, rreg);
        }
        else if(nodeType == IRNodeType::SHL || nodeType == IRNodeType::SAL || nodeType == IRNodeType::SHR || nodeType == IRNodeType::SAR){
            _asm.genMov(lreg, "%rcx", "q");
            _asm.genOperation(irNodeToString.at(nodeType), "%rcx", rreg);
        }
        else{
            _asm.genOperation(irNodeToString.at(nodeType), lreg, rreg);
        }
        if(gpFreeRegPos >= gpRegisters.size()){
            _asm.genPush(rreg);
        }
        takeGpReg();
    }
}

void CodeGenerator::generateRelationalExpression(const IRTree* node){
    generateNumericalExpression(node->getChild(0));
    generateNumericalExpression(node->getChild(1));
    
    freeGpReg();
    std::string lreg{ gpRegisters.at(gpFreeRegPos) };
    freeGpReg();
    std::string rreg{ gpRegisters.at(gpFreeRegPos) };
    _asm.genCmp(lreg, rreg);
}

const std::string& CodeGenerator::generateID(const IRTree* node) const {
    return variableMap.at(node->getName()); // returns address from variable map
}

std::string CodeGenerator::generateLiteral(const IRTree* node) const {
    std::string val{ node->getValue() };
    if(node->getType() == Types::UNSIGNED){
        val.pop_back();
    }
    return std::format("${}", val);
}

void CodeGenerator::generateFunctionCall(const IRTree* node){
    // push arguments to stack
    generateArgument(node->getChild(0));

    _asm.genCall(node->getName());

    // pop arguments from stack
    clearArguments(node->getChild(0));
}

void CodeGenerator::generateArgument(const IRTree* node){
    // pushing arguments onto stack
    for(size_t i = node->getChildren().size(); i-- > 0;){
        generateNumericalExpression(node->getChild(i));
        freeGpReg();
        if(gpFreeRegPos < gpRegisters.size()){ // if >= gpRegisters.size() argument is already pushed
            _asm.genPush(gpRegisters.at(gpFreeRegPos));
        }
    }
}

void CodeGenerator::clearArguments(const IRTree* node){
    // popping arguments of the stack
    for(size_t i = 0; i < node->getChildren().size(); ++i){
        _asm.genPop("%rbx");
    }
}
