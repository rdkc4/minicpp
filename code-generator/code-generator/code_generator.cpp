#include "code_generator.hpp"
#include <cstddef>
#include <fstream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <format>
#include <thread>

#include "../../thread-pool/thread_pool.hpp"
#include "../defs/code_generator_defs.hpp"

CodeGenerator::CodeGenerator(const std::string& file) : labelNum{ 0 }, prints{ false }, outputPath{ file } {}

thread_local CodeGeneratorThreadContext CodeGenerator::codeGenContext;

// allocating general-purpose register r(8-15)
void CodeGenerator::takeGpReg(size_t& gpFreeRegPos) noexcept {
    ++gpFreeRegPos;    
}

// releasing general-purpose register r(8-15)
void CodeGenerator::freeGpReg(size_t& gpFreeRegPos) noexcept {
    --gpFreeRegPos;
}

size_t CodeGenerator::getNextLabelNum() noexcept {
    return labelNum.fetch_add(1);
}

void CodeGenerator::generateCode(const IRTree* root){
    initFunctions(root);

    ThreadPool threadPool{ std::thread::hardware_concurrency() };

    // counter of generated functions
    std::atomic<size_t> done{0};
    std::condition_variable doneCv;
    std::mutex doneMtx;

    const size_t total = root->getChildren().size();

    for(const auto& child : root->getChildren()){
        threadPool.enqueue([&, child=child.get()]{
            generateFunction(child);

            if(done.fetch_add(1) + 1 == total){
                doneCv.notify_all();
            }
        });
    }

    {
        // wait until each function is generated
        std::unique_lock<std::mutex> lock(doneMtx);
        doneCv.wait(lock, [&] { return done == total; });
    }

    std::ofstream file{ outputPath };
    if(!file.is_open()){
        throw std::runtime_error(std::format("Unable to open '{}'", outputPath));
    }

    // start of asm code
    file << _asm.genStart();

    for(const auto& function : root->getChildren()){
        for(const auto& instruction : asmCode[function->getName()]){
            file << instruction;
        }
    }
    
    if(prints){
        auto printfFunc = _asm.printfFunction();
        for(const auto& instruction : printfFunc){
            file << instruction;
        }
    }
}

void CodeGenerator::initFunctions(const IRTree* node){
    for(const auto& child : node->getChildren()){
        asmCode[child->getName()] = {};
    }
}

void CodeGenerator::generateFunction(const IRTree* node){
    codeGenContext.init(node->getName());

    _asm.genNewLine(codeGenContext.asmCode);

    // function label
    _asm.genLabel(codeGenContext.asmCode, codeGenContext.functionName);

    _asm.genFuncPrologue(codeGenContext.asmCode);
    
    // allocation of local variables
    if(node->getValue() != "0"){
        _asm.genOperation(codeGenContext.asmCode, "sub", std::format("${}",node->getValue()), "%rsp");
    }

    _asm.genNewLine(codeGenContext.asmCode);

    generateParameter(node->getChild(0));

    for(size_t i = 1; i < node->getChildren().size(); i++){
        generateConstruct(node->getChild(i));
    }

    // function end label
    _asm.genLabel(codeGenContext.asmCode, std::format("{}_end", codeGenContext.functionName));
    
    // free local variables 
    if(node->getValue() != "0"){
        _asm.genOperation(codeGenContext.asmCode, "add", std::format("${}", node->getValue()), "%rsp");
    }

    _asm.genFuncEpilogue(codeGenContext.asmCode);

    if(node->getName() != "main"){
        _asm.genRet(codeGenContext.asmCode);
    }
    else{
        _asm.genExit(codeGenContext.asmCode);
    }

    {
        std::lock_guard<std::mutex> lock(asmMtx);
        asmCode[codeGenContext.functionName] = std::move(codeGenContext.asmCode);
    }

    codeGenContext.reset();
}

void CodeGenerator::generateParameter(const IRTree* node){
    size_t i{ 2 };
    for(const auto& parameter : node->getChildren()){
        // mapping parameter to address relative to %rbp (+n(%rbp))
        codeGenContext.variableMap.insert({parameter->getName(), std::format("{}(%rbp)", i * regSize)});
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
    auto [varPtr, success]{ codeGenContext.variableMap.insert({node->getName(), std::format("-{}(%rbp)", codeGenContext.variableNum * regSize)}) };
    if(!success){
        varPtr->second = std::format("-{}(%rbp)", codeGenContext.variableNum * regSize);
    }
    ++codeGenContext.variableNum;

    // direct initialization / default value assignation
    if(node->getChildren().size() != 0){
        generateAssignmentStatement(node->getChild(0));
    }
    else{
        // default value 
        _asm.genMov(codeGenContext.asmCode, "$0", generateID(node), "q");
    }
    _asm.genNewLine(codeGenContext.asmCode);

}

void CodeGenerator::generateStatement(const IRTree* node){
    switch(node->getNodeType()){
        case IRNodeType::PRINTF:
            generatePrintfStatement(node);
            break;
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
            return;
    }
    _asm.genNewLine(codeGenContext.asmCode);
}

void CodeGenerator::generatePrintfStatement(const IRTree* node){
    prints.store(true);

    size_t temporaryCount{}; // preventing register corruption when function call occurs
    if(node->getChild(0)->getNodeType() == IRNodeType::TEMPORARY){
        for(const auto& child : node->getChild(0)->getChildren()){
            generateVariable(child.get());
        }
        ++temporaryCount;
    }
    generateNumericalExpression(node->getChild(temporaryCount));
    freeGpReg(codeGenContext.gpFreeRegPos);
    _asm.genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), "%rax", "q");
    _asm.genCall(codeGenContext.asmCode, "_printf");
}

void CodeGenerator::generateIfStatement(const IRTree* node){
    size_t labNum{ getNextLabelNum() };
    size_t size{ node->getChildren().size() };

    _asm.genLabel(codeGenContext.asmCode, std::format("_if{}", labNum));
    generateRelationalExpression(node->getChild(0));

    int type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    std::string jmpLabel{ "" };

    if(size > 3){
        jmpLabel = std::format("_elif{}_0\n", labNum);
    }
    else if(size == 3){
        jmpLabel = std::format("_else{}\n", labNum);
    }
    else{
        jmpLabel = std::format("_if{}_end\n", labNum);
    }

    _asm.genJmp(codeGenContext.asmCode, stringToOppJMP.at(node->getChild(0)->getValue())[type], jmpLabel);

    generateStatement(node->getChild(1));
    _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("_if{}_end", labNum));
    if(node->getChildren().size() == 3){
        _asm.genNewLine(codeGenContext.asmCode);
    }

    for(size_t i = 2; i < size; i+=2){
        if(node->getChild(i)->getNodeType() == IRNodeType::CMP){
            _asm.genNewLine(codeGenContext.asmCode);
            _asm.genLabel(codeGenContext.asmCode, std::format("_elif{}_{}", labNum, i/2-1));
            generateRelationalExpression(node->getChild(i));

            type = node->getChild(i)->getChild(0)->getType() == Types::INT ? 0 : 1;
            std::string jmpLabel{ "" };
            
            if(i+2 == size - 1){
                jmpLabel = std::format("_else{}", labNum);
            }
            else if(i+2 < size){
                jmpLabel = std::format("_elif{}_{}", labNum, i/2);
            }
            else{
                jmpLabel = std::format("_if{}_end", labNum);
            }

            _asm.genJmp(codeGenContext.asmCode, stringToOppJMP.at(node->getChild(i)->getValue())[type], jmpLabel);

            generateConstruct(node->getChild(i+1));
            _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("_if{}_end", labNum));
        }
    }

    if(size % 2 == 1){
        _asm.genLabel(codeGenContext.asmCode, std::format("_else{}", labNum));
        generateConstruct(node->getChildren().back().get());
    }
    _asm.genLabel(codeGenContext.asmCode, std::format("_if{}_end", labNum));

}

void CodeGenerator::generateWhileStatement(const IRTree* node){
    size_t labNum{ getNextLabelNum() };

    _asm.genLabel(codeGenContext.asmCode, std::format("_while{}", labNum));
    generateRelationalExpression(node->getChild(0));

    int type{ node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1 };
    _asm.genJmp(codeGenContext.asmCode, stringToOppJMP.at(node->getChild(0)->getValue())[type], std::format("_while{}_end", labNum));
    _asm.genNewLine(codeGenContext.asmCode);

    generateConstruct(node->getChild(1));
    _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("_while{}", labNum));
    _asm.genNewLine(codeGenContext.asmCode);
    _asm.genLabel(codeGenContext.asmCode, std::format("_while{}_end", labNum));
}

void CodeGenerator::generateForStatement(const IRTree* node){
    size_t labNum{ getNextLabelNum() };

    // initializer
    generateAssignmentStatement(node->getChild(0));
    _asm.genNewLine(codeGenContext.asmCode);
    _asm.genLabel(codeGenContext.asmCode, std::format("_for{}", labNum));
    generateRelationalExpression(node->getChild(1));

    // condition
    int type = node->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    _asm.genJmp(codeGenContext.asmCode, stringToOppJMP.at(node->getChild(1)->getValue())[type], std::format("_for{}_end", labNum));
    _asm.genNewLine(codeGenContext.asmCode);

    generateConstruct(node->getChild(3));

    // increment
    generateAssignmentStatement(node->getChild(2));
    _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("_for{}", labNum));

    _asm.genNewLine(codeGenContext.asmCode);
    _asm.genLabel(codeGenContext.asmCode, std::format("_for{}_end", labNum));
}

void CodeGenerator::generateDoWhileStatement(const IRTree* node){
    size_t labNum{ getNextLabelNum() };

    _asm.genLabel(codeGenContext.asmCode, std::format("_do_while{}", labNum));
    generateConstruct(node->getChild(0));
    
    generateRelationalExpression(node->getChild(1));
    int type{ node->getChild(1)->getChild(0)->getType() == Types::INT ? 0 : 1 };
    _asm.genJmp(codeGenContext.asmCode, stringToJMP.at(node->getChild(1)->getValue())[type], std::format("_do_while{}", labNum));
}

void CodeGenerator::generateCompoundStatement(const IRTree* node){
    for(const auto& child : node->getChildren()){
        generateConstruct(child.get());
    }
}

// evaluating rvalue
void CodeGenerator::generateAssignmentStatement(const IRTree* node){
    size_t temporaryCount{}; // preventing register corruption when function call occurs
    if(node->getChild(0)->getNodeType() == IRNodeType::TEMPORARY){
        for(const auto& child : node->getChild(0)->getChildren()){
            generateVariable(child.get());
        }
        ++temporaryCount;
    }
    generateNumericalExpression(node->getChild(1 + temporaryCount));
    freeGpReg(codeGenContext.gpFreeRegPos);
    _asm.genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), generateID(node->getChild(temporaryCount)), "q");
}

// return value ends up in %rax
void CodeGenerator::generateReturnStatement(const IRTree* node){
    if(node->getChildren().size() != 0){
        size_t temporaryCount{}; // preventing register corruption when function call occurs
        if(node->getChild(0)->getNodeType() == IRNodeType::TEMPORARY){
            for(const auto& child : node->getChild(0)->getChildren()){
                generateVariable(child.get());
            }
            ++temporaryCount;
        }
        generateNumericalExpression(node->getChild(temporaryCount));
        freeGpReg(codeGenContext.gpFreeRegPos);
        _asm.genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), "%rax", "q");
    }else{
        _asm.genOperation(codeGenContext.asmCode, "xor", "%rax", "%rax");
    }
    _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("{}_end", codeGenContext.functionName));
}

void CodeGenerator::generateSwitchStatement(const IRTree* node){
    size_t labNum{ getNextLabelNum() };

    _asm.genLabel(codeGenContext.asmCode, std::format("_switch{}", labNum));

    size_t i{ 1 };
    size_t size{ node->getChildren().size() };
    std::string var{ node->getChild(0)->getName() };
    bool hasDefault{ node->getChildren().back()->getNodeType() == IRNodeType::DEFAULT };
    
    // cases
    for(; i < size; i++){
        IRTree* child = node->getChild(i);
        if(child->getNodeType() == IRNodeType::CASE){
            _asm.genLabel(codeGenContext.asmCode, std::format("_switch{}_case{}", labNum, i-1));
            _asm.genMov(codeGenContext.asmCode, codeGenContext.variableMap.at(var), "%rcx", "q");
            _asm.genMov(codeGenContext.asmCode, generateLiteral(child->getChild(0)), "%rdx", "q");
            _asm.genCmp(codeGenContext.asmCode, "%rcx", "%rdx");
            
            std::string jmpLabel = "";
            if(hasDefault){
                jmpLabel = std::format("_switch{}_{}", labNum, (i < size-2 ?  "case" + std::to_string(i) : "default"));
            }
            else{
                jmpLabel = std::format("_switch{}_{}", labNum, (i < size-1 ?  "case" + std::to_string(i) : "end"));
            }
            _asm.genJmp(codeGenContext.asmCode, "jne", jmpLabel);

            for(size_t j = 1; j < child->getChildren().size(); ++j){
                generateConstruct(child->getChild(j));
            }

            if(child->getChildren().size()==3){
                _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("_switch{}_end", labNum));
            }

        }else{
            _asm.genLabel(codeGenContext.asmCode, std::format("_switch{}_default", labNum));
            for(size_t j = 0; j < child->getChildren().size(); ++j){
                generateConstruct(child->getChild(j));
            }
        }
    }
    _asm.genLabel(codeGenContext.asmCode, std::format("_switch{}_end", labNum));
}

// evaluating equations using general-purpose registers r(8-15) and stack (if necessary)
void CodeGenerator::generateNumericalExpression(const IRTree* node){
    if(node->getNodeType() == IRNodeType::ID){
        if(codeGenContext.gpFreeRegPos < gpRegisters.size()){
            _asm.genMov(codeGenContext.asmCode, generateID(node), gpRegisters.at(codeGenContext.gpFreeRegPos), "q");
        }
        else{
            _asm.genPush(codeGenContext.asmCode, generateID(node));
        }
        takeGpReg(codeGenContext.gpFreeRegPos);
    }
    else if(node->getNodeType() == IRNodeType::LITERAL){
        if(codeGenContext.gpFreeRegPos < gpRegisters.size()){
            _asm.genMov(codeGenContext.asmCode, generateLiteral(node), gpRegisters.at(codeGenContext.gpFreeRegPos), "q");
        }
        else{
            _asm.genPush(codeGenContext.asmCode, generateLiteral(node));
        }
        takeGpReg(codeGenContext.gpFreeRegPos);
    }else if (node->getNodeType() == IRNodeType::CALL){
        generateFunctionCall(node);
        if(codeGenContext.gpFreeRegPos < gpRegisters.size()){
            _asm.genMov(codeGenContext.asmCode, "%rax", gpRegisters.at(codeGenContext.gpFreeRegPos), "q");
        }
        else{
            _asm.genPush(codeGenContext.asmCode, "%rax");
        }
        takeGpReg(codeGenContext.gpFreeRegPos);
    }
    else{
        std::string lreg{};
        std::string rreg{};
        
        generateNumericalExpression(node->getChild(0));
        generateNumericalExpression(node->getChild(1));
        
        freeGpReg(codeGenContext.gpFreeRegPos);
        if(codeGenContext.gpFreeRegPos >= gpRegisters.size()){
            lreg = "%rdi";
            _asm.genPop(codeGenContext.asmCode, lreg);
        }
        else{
            lreg = gpRegisters.at(codeGenContext.gpFreeRegPos);
        }
        freeGpReg(codeGenContext.gpFreeRegPos);
        if(codeGenContext.gpFreeRegPos >= gpRegisters.size()){
            rreg = "%rsi";
            _asm.genPop(codeGenContext.asmCode, rreg);
        }
        else{
            rreg = gpRegisters.at(codeGenContext.gpFreeRegPos);
        }
        
        IRNodeType nodeType = node->getNodeType();
        if(nodeType == IRNodeType::MUL || nodeType == IRNodeType::DIV){ // result of MUL || DIV is in RDX:RAX
            _asm.genOperation(codeGenContext.asmCode, "xor", "%rdx", "%rdx"); //add overflow check (TODO)
            _asm.genMov(codeGenContext.asmCode, rreg, "%rax", "q");
            if(node->getType() == Types::INT){
                _asm.genOperation(codeGenContext.asmCode, std::format("i{}", irNodeToString.at(nodeType)), lreg);
            }
            else{
                _asm.genOperation(codeGenContext.asmCode, irNodeToString.at(nodeType), lreg);
            }
            _asm.genMov(codeGenContext.asmCode, "%rax", rreg, "q");
            _asm.genNewLine(codeGenContext.asmCode);
        }
        else if(nodeType == IRNodeType::AND || nodeType == IRNodeType::OR || nodeType == IRNodeType::XOR){
            _asm.genOperation(codeGenContext.asmCode, irNodeToString.at(nodeType), lreg, rreg);
        }
        else if(nodeType == IRNodeType::SHL || nodeType == IRNodeType::SAL || nodeType == IRNodeType::SHR || nodeType == IRNodeType::SAR){
            _asm.genMov(codeGenContext.asmCode, lreg, "%rcx", "q");
            _asm.genOperation(codeGenContext.asmCode, irNodeToString.at(nodeType), "%rcx", rreg);
        }
        else{
            _asm.genOperation(codeGenContext.asmCode, irNodeToString.at(nodeType), lreg, rreg);
        }
        if(codeGenContext.gpFreeRegPos >= gpRegisters.size()){
            _asm.genPush(codeGenContext.asmCode, rreg);
        }
        takeGpReg(codeGenContext.gpFreeRegPos);
    }
}

void CodeGenerator::generateRelationalExpression(const IRTree* node){
    size_t temporaryCount{}; // preventing register corruption when function call occurs
    for(size_t i = 0; i < 2; ++i){
        if(node->getChild(i)->getNodeType() == IRNodeType::TEMPORARY){
            for(const auto& child : node->getChild(i)->getChildren()){
                generateVariable(child.get());
            }
            ++temporaryCount;
        }
    }
    generateNumericalExpression(node->getChild(0 + temporaryCount));
    generateNumericalExpression(node->getChild(1 + temporaryCount));
    
    freeGpReg(codeGenContext.gpFreeRegPos);
    std::string lreg{ gpRegisters.at(codeGenContext.gpFreeRegPos) };
    freeGpReg(codeGenContext.gpFreeRegPos);
    std::string rreg{ gpRegisters.at(codeGenContext.gpFreeRegPos) };
    _asm.genCmp(codeGenContext.asmCode, lreg, rreg);
}

const std::string& CodeGenerator::generateID(const IRTree* node) const {
    return codeGenContext.variableMap.at(node->getName()); // returns address from variable map
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

    _asm.genCall(codeGenContext.asmCode, node->getName());

    // pop arguments from stack
    clearArguments(node->getChild(0));
}

void CodeGenerator::generateArgument(const IRTree* node){
    // evaluating temporaries
    for(const auto& child : node->getChildren()){
        if(child->getNodeType() == IRNodeType::TEMPORARY){
            for(const auto& _child : child->getChildren()){
                generateVariable(_child.get());
            }
        }
    }
    // pushing arguments onto stack
    for(size_t i = node->getChildren().size(); i-- > 0;){
        auto child = node->getChild(i); 
        if(child->getNodeType() != IRNodeType::TEMPORARY){
            generateNumericalExpression(child);
            freeGpReg(codeGenContext.gpFreeRegPos);
            if(codeGenContext.gpFreeRegPos < gpRegisters.size()){ // if >= gpRegisters.size() argument is already pushed
                _asm.genPush(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos));
            }
        }
    }
}

void CodeGenerator::clearArguments(const IRTree* node){
    // popping arguments of the stack
    size_t argumentCount{};
    for(size_t i = 0; i < node->getChildren().size(); ++i){
        if(node->getChild(i)->getNodeType() != IRNodeType::TEMPORARY){
            ++argumentCount;
        }
    }
    _asm.genOperation(codeGenContext.asmCode, "add", std::format("${}", argumentCount*regSize), "%rsp");
}
