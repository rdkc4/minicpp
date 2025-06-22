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

CodeGenerator::CodeGenerator(const std::string& filePath) : labelNum{ 0 }, prints{ false }, outputPath{ filePath } {}

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

void CodeGenerator::generateCode(const IRTree* _root){
    initFunctions(_root);

    ThreadPool threadPool{ std::thread::hardware_concurrency() };

    // counter of generated functions
    std::atomic<size_t> done{0};
    std::condition_variable doneCv;
    std::mutex doneMtx;

    const size_t total = _root->getChildren().size();

    for(const auto& _function : _root->getChildren()){
        threadPool.enqueue([&, _function=_function.get()]{
            generateFunction(_function);

            if(done.fetch_add(1) + 1 == total){
                doneCv.notify_one();
            }
        });
    }

    {
        // wait until each function is generated
        std::unique_lock<std::mutex> lock(doneMtx);
        doneCv.wait(lock, [&] { return done == total; });
    }

    writeCode(_root);
}

void CodeGenerator::writeCode(const IRTree* _root){
    std::ofstream file{ outputPath };
    if(!file.is_open()){
        throw std::runtime_error(std::format("Unable to open '{}'", outputPath));
    }

    // start of asm code
    file << _asm.genStart();

    for(const auto& _function : _root->getChildren()){
        for(const auto& instruction : asmCode[_function->getName()]){
            file << instruction;
        }
    }
    
    if(prints){
        std::vector<std::string> printfFunc;
        _asm.printfFunction(printfFunc);
        for(const auto& instruction : printfFunc){
            file << instruction;
        }
    }
}

void CodeGenerator::initFunctions(const IRTree* _root){
    for(const auto& _function : _root->getChildren()){
        asmCode[_function->getName()] = {};
    }
}

void CodeGenerator::generateFunction(const IRTree* _function){
    codeGenContext.init(_function->getName());

    _asm.genNewLine(codeGenContext.asmCode);

    // function label
    _asm.genLabel(codeGenContext.asmCode, codeGenContext.functionName);

    _asm.genFuncPrologue(codeGenContext.asmCode);
    
    // allocation of local variables
    if(_function->getValue() != "0"){
        _asm.genOperation(codeGenContext.asmCode, "sub", std::format("${}", _function->getValue()), "%rsp");
    }

    _asm.genNewLine(codeGenContext.asmCode);

    generateParameter(_function->getChild(0));

    for(size_t i = 1; i < _function->getChildren().size(); i++){
        generateConstruct(_function->getChild(i));
    }

    // function end label
    _asm.genLabel(codeGenContext.asmCode, std::format("{}_end", codeGenContext.functionName));
    
    // free local variables 
    if(_function->getValue() != "0"){
        _asm.genOperation(codeGenContext.asmCode, "add", std::format("${}", _function->getValue()), "%rsp");
    }

    _asm.genFuncEpilogue(codeGenContext.asmCode);

    if(_function->getName() != "main"){
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

void CodeGenerator::generateParameter(const IRTree* _parameters){
    size_t i{ 2 };
    for(const auto& _parameter : _parameters->getChildren()){
        // mapping parameter to address relative to %rbp (+n(%rbp))
        codeGenContext.variableMap.insert({_parameter->getName(), std::format("{}(%rbp)", i * regSize)});
        ++i;
    }
}

void CodeGenerator::generateConstruct(const IRTree* _construct){
    if(_construct->getNodeType() == IRNodeType::VARIABLE){
        generateVariable(_construct);
    }
    else{
        generateStatement(_construct);
    }
}

void CodeGenerator::generateVariable(const IRTree* _variable){
    // mapping local variable to address relative to %rbp (-n(%rbp))
    // if not successful it means that variable with the given name existed but went out of scope, so it overwrites it with new memory location
    auto [varPtr, success]{ codeGenContext.variableMap.insert({_variable->getName(), std::format("-{}(%rbp)", codeGenContext.variableNum * regSize)}) };
    if(!success){
        varPtr->second = std::format("-{}(%rbp)", codeGenContext.variableNum * regSize);
    }
    ++codeGenContext.variableNum;

    // direct initialization / default value assignation
    if(_variable->getChildren().size() != 0){
        generateAssignmentStatement(_variable->getChild(0));
    }
    else{
        // default value 
        _asm.genMov(codeGenContext.asmCode, "$0", generateID(_variable), "q");
    }
    _asm.genNewLine(codeGenContext.asmCode);

}

void CodeGenerator::generateStatement(const IRTree* _statement){
    switch(_statement->getNodeType()){
        case IRNodeType::PRINTF:
            generatePrintfStatement(_statement);
            break;
        case IRNodeType::IF:
            generateIfStatement(_statement);
            break;
        case IRNodeType::COMPOUND:
            generateCompoundStatement(_statement);
            return;
        case IRNodeType::ASSIGN:
            generateAssignmentStatement(_statement);
            break;
        case IRNodeType::RETURN:
            generateReturnStatement(_statement);
            break;
        case IRNodeType::WHILE:
            generateWhileStatement(_statement);
            break;
        case IRNodeType::FOR:
            generateForStatement(_statement);
            break;
        case IRNodeType::DO_WHILE:
            generateDoWhileStatement(_statement);
            break;
        case IRNodeType::SWITCH:
            generateSwitchStatement(_statement);
            break;
        default:
            return;
    }
    _asm.genNewLine(codeGenContext.asmCode);
}

void CodeGenerator::generatePrintfStatement(const IRTree* _printf){
    prints.store(true);

    size_t temporaryCount{}; // preventing register corruption when function call occurs
    if(_printf->getChild(0)->getNodeType() == IRNodeType::TEMPORARY){
        for(const auto& _variable : _printf->getChild(0)->getChildren()){
            generateVariable(_variable.get());
        }
        ++temporaryCount;
    }
    generateNumericalExpression(_printf->getChild(temporaryCount));
    freeGpReg(codeGenContext.gpFreeRegPos);
    _asm.genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), "%rax", "q");
    _asm.genCall(codeGenContext.asmCode, "_printf");
}

void CodeGenerator::generateIfStatement(const IRTree* _if){
    size_t labNum{ getNextLabelNum() };
    size_t size{ _if->getChildren().size() };

    _asm.genLabel(codeGenContext.asmCode, std::format("_if{}", labNum));
    generateRelationalExpression(_if->getChild(0));

    int type = _if->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
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

    _asm.genJmp(codeGenContext.asmCode, stringToOppJMP.at(_if->getChild(0)->getValue())[type], jmpLabel);

    generateStatement(_if->getChild(1));
    _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("_if{}_end", labNum));
    if(_if->getChildren().size() == 3){
        _asm.genNewLine(codeGenContext.asmCode);
    }

    for(size_t i = 2; i < size; i+=2){
        if(_if->getChild(i)->getNodeType() == IRNodeType::CMP){
            _asm.genNewLine(codeGenContext.asmCode);
            _asm.genLabel(codeGenContext.asmCode, std::format("_elif{}_{}", labNum, i/2-1));
            generateRelationalExpression(_if->getChild(i));

            type = _if->getChild(i)->getChild(0)->getType() == Types::INT ? 0 : 1;
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

            _asm.genJmp(codeGenContext.asmCode, stringToOppJMP.at(_if->getChild(i)->getValue())[type], jmpLabel);

            generateConstruct(_if->getChild(i+1));
            _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("_if{}_end", labNum));
        }
    }

    if(size % 2 == 1){
        _asm.genLabel(codeGenContext.asmCode, std::format("_else{}", labNum));
        generateConstruct(_if->getChildren().back().get());
    }
    _asm.genLabel(codeGenContext.asmCode, std::format("_if{}_end", labNum));

}

void CodeGenerator::generateWhileStatement(const IRTree* _while){
    size_t labNum{ getNextLabelNum() };

    _asm.genLabel(codeGenContext.asmCode, std::format("_while{}", labNum));
    generateRelationalExpression(_while->getChild(0));

    int type{ _while->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1 };
    _asm.genJmp(codeGenContext.asmCode, stringToOppJMP.at(_while->getChild(0)->getValue())[type], std::format("_while{}_end", labNum));
    _asm.genNewLine(codeGenContext.asmCode);

    generateConstruct(_while->getChild(1));
    _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("_while{}", labNum));
    _asm.genNewLine(codeGenContext.asmCode);
    _asm.genLabel(codeGenContext.asmCode, std::format("_while{}_end", labNum));
}

void CodeGenerator::generateForStatement(const IRTree* _for){
    size_t labNum{ getNextLabelNum() };

    // initializer
    generateAssignmentStatement(_for->getChild(0));
    _asm.genNewLine(codeGenContext.asmCode);
    _asm.genLabel(codeGenContext.asmCode, std::format("_for{}", labNum));
    generateRelationalExpression(_for->getChild(1));

    // condition
    int type = _for->getChild(0)->getChild(0)->getType() == Types::INT ? 0 : 1;
    _asm.genJmp(codeGenContext.asmCode, stringToOppJMP.at(_for->getChild(1)->getValue())[type], std::format("_for{}_end", labNum));
    _asm.genNewLine(codeGenContext.asmCode);

    generateConstruct(_for->getChild(3));

    // increment
    generateAssignmentStatement(_for->getChild(2));
    _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("_for{}", labNum));

    _asm.genNewLine(codeGenContext.asmCode);
    _asm.genLabel(codeGenContext.asmCode, std::format("_for{}_end", labNum));
}

void CodeGenerator::generateDoWhileStatement(const IRTree* _dowhile){
    size_t labNum{ getNextLabelNum() };

    _asm.genLabel(codeGenContext.asmCode, std::format("_do_while{}", labNum));
    generateConstruct(_dowhile->getChild(0));
    
    generateRelationalExpression(_dowhile->getChild(1));
    int type{ _dowhile->getChild(1)->getChild(0)->getType() == Types::INT ? 0 : 1 };
    _asm.genJmp(codeGenContext.asmCode, stringToJMP.at(_dowhile->getChild(1)->getValue())[type], std::format("_do_while{}", labNum));
}

void CodeGenerator::generateCompoundStatement(const IRTree* _compound){
    for(const auto& _construct : _compound->getChildren()){
        generateConstruct(_construct.get());
    }
}

// evaluating rvalue
void CodeGenerator::generateAssignmentStatement(const IRTree* _assignment){
    size_t temporaryCount{}; // preventing register corruption when function call occurs
    if(_assignment->getChild(0)->getNodeType() == IRNodeType::TEMPORARY){
        for(const auto& _variable : _assignment->getChild(0)->getChildren()){
            generateVariable(_variable.get());
        }
        ++temporaryCount;
    }
    generateNumericalExpression(_assignment->getChild(1 + temporaryCount));
    freeGpReg(codeGenContext.gpFreeRegPos);
    _asm.genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), generateID(_assignment->getChild(temporaryCount)), "q");
}

// return value ends up in %rax
void CodeGenerator::generateReturnStatement(const IRTree* _return){
    if(_return->getChildren().size() != 0){
        size_t temporaryCount{}; // preventing register corruption when function call occurs
        if(_return->getChild(0)->getNodeType() == IRNodeType::TEMPORARY){
            for(const auto& _variable : _return->getChild(0)->getChildren()){
                generateVariable(_variable.get());
            }
            ++temporaryCount;
        }
        generateNumericalExpression(_return->getChild(temporaryCount));
        freeGpReg(codeGenContext.gpFreeRegPos);
        _asm.genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), "%rax", "q");
    }else{
        _asm.genOperation(codeGenContext.asmCode, "xor", "%rax", "%rax");
    }
    _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("{}_end", codeGenContext.functionName));
}

void CodeGenerator::generateSwitchStatement(const IRTree* _switch){
    size_t labNum{ getNextLabelNum() };

    _asm.genLabel(codeGenContext.asmCode, std::format("_switch{}", labNum));

    size_t i{ 1 };
    size_t size{ _switch->getChildren().size() };
    std::string var{ _switch->getChild(0)->getName() };
    bool hasDefault{ _switch->getChildren().back()->getNodeType() == IRNodeType::DEFAULT };
    
    // cases
    for(; i < size; i++){
        const IRTree* _case = _switch->getChild(i);
        if(_case->getNodeType() == IRNodeType::CASE){
            _asm.genLabel(codeGenContext.asmCode, std::format("_switch{}_case{}", labNum, i-1));
            _asm.genMov(codeGenContext.asmCode, codeGenContext.variableMap.at(var), "%rcx", "q");
            _asm.genMov(codeGenContext.asmCode, generateLiteral(_case->getChild(0)), "%rdx", "q");
            _asm.genCmp(codeGenContext.asmCode, "%rcx", "%rdx");
            
            std::string jmpLabel = "";
            if(hasDefault){
                jmpLabel = std::format("_switch{}_{}", labNum, (i < size-2 ?  "case" + std::to_string(i) : "default"));
            }
            else{
                jmpLabel = std::format("_switch{}_{}", labNum, (i < size-1 ?  "case" + std::to_string(i) : "end"));
            }
            _asm.genJmp(codeGenContext.asmCode, "jne", jmpLabel);

            for(size_t j = 1; j < _case->getChildren().size(); ++j){
                generateConstruct(_case->getChild(j));
            }

            if(_case->getChildren().size()==3){
                _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("_switch{}_end", labNum));
            }

        }else{
            _asm.genLabel(codeGenContext.asmCode, std::format("_switch{}_default", labNum));
            for(size_t j = 0; j < _case->getChildren().size(); ++j){
                generateConstruct(_case->getChild(j));
            }
        }
    }
    _asm.genLabel(codeGenContext.asmCode, std::format("_switch{}_end", labNum));
}

// evaluating equations using general-purpose registers r(8-15) and stack (if necessary)
void CodeGenerator::generateNumericalExpression(const IRTree* _numexp){
    if(_numexp->getNodeType() == IRNodeType::ID){
        if(codeGenContext.gpFreeRegPos < gpRegisters.size()){
            _asm.genMov(codeGenContext.asmCode, generateID(_numexp), gpRegisters.at(codeGenContext.gpFreeRegPos), "q");
        }
        else{
            _asm.genPush(codeGenContext.asmCode, generateID(_numexp));
        }
        takeGpReg(codeGenContext.gpFreeRegPos);
    }
    else if(_numexp->getNodeType() == IRNodeType::LITERAL){
        if(codeGenContext.gpFreeRegPos < gpRegisters.size()){
            _asm.genMov(codeGenContext.asmCode, generateLiteral(_numexp), gpRegisters.at(codeGenContext.gpFreeRegPos), "q");
        }
        else{
            _asm.genPush(codeGenContext.asmCode, generateLiteral(_numexp));
        }
        takeGpReg(codeGenContext.gpFreeRegPos);
    }else if (_numexp->getNodeType() == IRNodeType::CALL){
        generateFunctionCall(_numexp);
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
        
        generateNumericalExpression(_numexp->getChild(0));
        generateNumericalExpression(_numexp->getChild(1));
        
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
        
        IRNodeType nodeType = _numexp->getNodeType();
        if(nodeType == IRNodeType::MUL || nodeType == IRNodeType::DIV){ // result of MUL || DIV is in RDX:RAX
            _asm.genOperation(codeGenContext.asmCode, "xor", "%rdx", "%rdx"); //add overflow check (TODO)
            _asm.genMov(codeGenContext.asmCode, rreg, "%rax", "q");
            if(_numexp->getType() == Types::INT){
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

void CodeGenerator::generateRelationalExpression(const IRTree* _relexp){
    size_t temporaryCount{}; // preventing register corruption when function call occurs
    for(size_t i = 0; i < 2; ++i){
        if(_relexp->getChild(i)->getNodeType() == IRNodeType::TEMPORARY){
            for(const auto& child : _relexp->getChild(i)->getChildren()){
                generateVariable(child.get());
            }
            ++temporaryCount;
        }
    }
    generateNumericalExpression(_relexp->getChild(0 + temporaryCount));
    generateNumericalExpression(_relexp->getChild(1 + temporaryCount));
    
    freeGpReg(codeGenContext.gpFreeRegPos);
    std::string lreg{ gpRegisters.at(codeGenContext.gpFreeRegPos) };
    freeGpReg(codeGenContext.gpFreeRegPos);
    std::string rreg{ gpRegisters.at(codeGenContext.gpFreeRegPos) };
    _asm.genCmp(codeGenContext.asmCode, lreg, rreg);
}

const std::string& CodeGenerator::generateID(const IRTree* _id) const {
    return codeGenContext.variableMap.at(_id->getName()); // returns address from variable map
}

std::string CodeGenerator::generateLiteral(const IRTree* _literal) const {
    std::string val{ _literal->getValue() };
    if(_literal->getType() == Types::UNSIGNED){
        val.pop_back();
    }
    return std::format("${}", val);
}

void CodeGenerator::generateFunctionCall(const IRTree* _functionCall){
    // push arguments to stack
    generateArgument(_functionCall->getChild(0));

    _asm.genCall(codeGenContext.asmCode, _functionCall->getName());

    // pop arguments from stack
    clearArguments(_functionCall->getChild(0));
}

void CodeGenerator::generateArgument(const IRTree* _arguments){
    // evaluating temporaries
    for(const auto& _argument : _arguments->getChildren()){
        if(_argument->getNodeType() == IRNodeType::TEMPORARY){
            for(const auto& _variable : _argument->getChildren()){
                generateVariable(_variable.get());
            }
        }
    }
    // pushing arguments onto stack
    for(size_t i = _arguments->getChildren().size(); i-- > 0;){
        const IRTree* _argument = _arguments->getChild(i); 
        if(_argument->getNodeType() != IRNodeType::TEMPORARY){
            generateNumericalExpression(_argument);
            freeGpReg(codeGenContext.gpFreeRegPos);
            if(codeGenContext.gpFreeRegPos < gpRegisters.size()){ // if >= gpRegisters.size() argument is already pushed
                _asm.genPush(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos));
            }
        }
    }
}

void CodeGenerator::clearArguments(const IRTree* _arguments){
    // popping arguments of the stack
    size_t argumentCount{};
    for(const auto& _argument : _arguments->getChildren()){
        if(_argument->getNodeType() != IRNodeType::TEMPORARY){
            ++argumentCount;
        }
    }
    _asm.genOperation(codeGenContext.asmCode, "add", std::format("${}", argumentCount*regSize), "%rsp");
}
