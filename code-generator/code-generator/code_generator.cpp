#include "code_generator.hpp"
#include <cstddef>
#include <fstream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <format>
#include <thread>

#include "../../common/intermediate-representation-tree/IRBinaryExpression.hpp"
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

void CodeGenerator::generateCode(const IRProgram* _program){
    initFunctions(_program);

    ThreadPool threadPool{ std::thread::hardware_concurrency() };

    // counter of generated functions
    std::atomic<size_t> done{0};
    std::condition_variable doneCv;
    std::mutex doneMtx;

    const size_t total = _program->getFunctionCount();

    for(const auto& _function : _program->getFunctions()){
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

    writeCode(_program);
}

void CodeGenerator::writeCode(const IRProgram* _program){
    std::ofstream file{ outputPath };
    if(!file.is_open()){
        throw std::runtime_error(std::format("Unable to open '{}'", outputPath));
    }

    // start of asm code
    file << _asm.genStart();

    for(const auto& _function : _program->getFunctions()){
        for(const auto& instruction : asmCode[_function->getFunctionName()]){
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

void CodeGenerator::initFunctions(const IRProgram* _program){
    for(const auto& _function : _program->getFunctions()){
        asmCode[_function->getFunctionName()] = {};
    }
}

void CodeGenerator::generateFunction(const IRFunction* _function){
    codeGenContext.init(_function->getFunctionName());

    // function label
    _asm.genLabel(codeGenContext.asmCode, codeGenContext.functionName);

    _asm.genFuncPrologue(codeGenContext.asmCode);
    
    // allocation of local variables
    if(_function->getRequiredMemory() != "0"){
        _asm.genOperation(codeGenContext.asmCode, "sub", std::format("${}", _function->getRequiredMemory()), "%rsp");
    }

    generateParameter(_function->getParameters());

    for(const auto& _statement : _function->getBody()){
        generateStatement(_statement.get());
    }

    // function end label
    _asm.genLabel(codeGenContext.asmCode, std::format("{}_end", codeGenContext.functionName));
    
    // free local variables 
    if(_function->getRequiredMemory() != "0"){
        _asm.genOperation(codeGenContext.asmCode, "add", std::format("${}", _function->getRequiredMemory()), "%rsp");
    }

    _asm.genFuncEpilogue(codeGenContext.asmCode);

    if(_function->getFunctionName() != "main"){
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

void CodeGenerator::generateParameter(const std::vector<std::unique_ptr<IRParameter>>& _parameters){
    size_t i{ 2 };
    for(const auto& _parameter : _parameters){
        // mapping parameter to address relative to %rbp (+n(%rbp))
        codeGenContext.variableMap.insert({_parameter->getParName(), std::format("{}(%rbp)", i * regSize)});
        ++i;
    }
}

void CodeGenerator::generateStatement(const IRStatement* _statement){
    switch(_statement->getNodeType()){
        case IRNodeType::VARIABLE:
            generateVariable(static_cast<const IRVariable*>(_statement));
            break;
        case IRNodeType::PRINTF:
            generatePrintfStatement(static_cast<const IRPrintfSt*>(_statement));
            break;
        case IRNodeType::IF:
            generateIfStatement(static_cast<const IRIfSt*>(_statement));
            break;
        case IRNodeType::COMPOUND:
            generateCompoundStatement(static_cast<const IRCompoundSt*>(_statement));
            break;
        case IRNodeType::ASSIGN:
            generateAssignmentStatement(static_cast<const IRAssignSt*>(_statement));
            break;
        case IRNodeType::RETURN:
            generateReturnStatement(static_cast<const IRReturnSt*>(_statement));
            break;
        case IRNodeType::WHILE:
            generateWhileStatement(static_cast<const IRWhileSt*>(_statement));
            break;
        case IRNodeType::FOR:
            generateForStatement(static_cast<const IRForSt*>(_statement));
            break;
        case IRNodeType::DO_WHILE:
            generateDoWhileStatement(static_cast<const IRDoWhileSt*>(_statement));
            break;
        case IRNodeType::SWITCH:
            generateSwitchStatement(static_cast<const IRSwitchSt*>(_statement));
            break;
        default:
            return;
    }
}

void CodeGenerator::generateVariable(const IRVariable* _variable){
    // mapping local variable to address relative to %rbp (-n(%rbp))
    // if not successful it means that variable with the given name existed but went out of scope, so it overwrites it with new memory location
    auto [varPtr, success]{ codeGenContext.variableMap.insert({_variable->getVarName(), std::format("-{}(%rbp)", codeGenContext.variableNum * regSize)}) };
    if(!success){
        varPtr->second = std::format("-{}(%rbp)", codeGenContext.variableNum * regSize);
    }
    ++codeGenContext.variableNum;

    // direct initialization / default value assignation
    if(_variable->hasAssign()){
        if(_variable->hasTemporaries()){
            generateTemporaries(_variable->getTemporaries());
        }

        generateNumericalExpression(_variable->getAssign());
        freeGpReg(codeGenContext.gpFreeRegPos);
        _asm.genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), codeGenContext.variableMap.at(_variable->getVarName()), "q");
    }
    else{
        // default value 
        _asm.genMov(codeGenContext.asmCode, "$0", codeGenContext.variableMap.at(_variable->getVarName()), "q");
    }
}

void CodeGenerator::generatePrintfStatement(const IRPrintfSt* _printf){
    prints.store(true);

    // preventing register corruption when function call occurs
    if(_printf->hasTemporaries()){
        generateTemporaries(_printf->getTemporaries());
    }
    generateNumericalExpression(_printf->getExp());

    freeGpReg(codeGenContext.gpFreeRegPos);
    _asm.genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), "%rax", "q");
    _asm.genCall(codeGenContext.asmCode, "_printf");
}

void CodeGenerator::generateIfStatement(const IRIfSt* _if){
    size_t labNum{ getNextLabelNum() };

    size_t size{ _if->getConditionCount() };

    auto [_ifCondition, _ifStatement, _ifTemp] = _if->getIfAtN(0);

    _asm.genLabel(codeGenContext.asmCode, std::format("_if{}", labNum));
    if(_ifTemp != nullptr){
        generateTemporaries(_ifTemp);
    }
    generateRelationalExpression(_ifCondition);

    std::string jmpLabel{ "" };
    if(size > 1){
        jmpLabel = std::format("_elif{}_0\n", labNum);
    }
    else if(_if->hasElse()){
        jmpLabel = std::format("_else{}\n", labNum);
    }
    else{
        jmpLabel = std::format("_if{}_end\n", labNum);
    }

    _asm.genJmp(codeGenContext.asmCode, nodeToOppJMP.at(_ifCondition->getNodeType()), jmpLabel);

    generateStatement(_ifStatement);
    _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("_if{}_end", labNum));

    for(size_t i = 1; i < size; ++i){
        auto [_elifCondition, _elifStatement, _elifTemp] = _if->getIfAtN(i);
        _asm.genLabel(codeGenContext.asmCode, std::format("_elif{}_{}", labNum, i-1));
        
        if(_elifTemp != nullptr){
            generateTemporaries(_elifTemp);
        }
        generateRelationalExpression(_elifCondition);

        std::string jmpLabel{ "" };
        if(i == size - 1 && _if->hasElse()){
            jmpLabel = std::format("_else{}", labNum);
        }
        else if(i < size - 1){
            jmpLabel = std::format("_elif{}_{}", labNum, i + 1);
        }
        else{
            jmpLabel = std::format("_if{}_end", labNum);
        }

        _asm.genJmp(codeGenContext.asmCode, nodeToOppJMP.at(_elifCondition->getNodeType()), jmpLabel);

        generateStatement(_elifStatement);
        _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("_if{}_end", labNum));
    }

    if(_if->hasElse()){
        _asm.genLabel(codeGenContext.asmCode, std::format("_else{}", labNum));
        generateStatement(_if->getElseStatement());
    }
    _asm.genLabel(codeGenContext.asmCode, std::format("_if{}_end", labNum));
}

void CodeGenerator::generateWhileStatement(const IRWhileSt* _while){
    size_t labNum{ getNextLabelNum() };

    _asm.genLabel(codeGenContext.asmCode, std::format("_while{}", labNum));
    if(_while->hasTemporaries()){
        generateTemporaries(_while->getTemporaries());
    }
    generateRelationalExpression(_while->getCondition());

    _asm.genJmp(codeGenContext.asmCode, nodeToOppJMP.at(_while->getCondition()->getNodeType()), std::format("_while{}_end", labNum));

    generateStatement(_while->getStatement());
    _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("_while{}", labNum));
    _asm.genLabel(codeGenContext.asmCode, std::format("_while{}_end", labNum));
}

void CodeGenerator::generateForStatement(const IRForSt* _for){
    size_t labNum{ getNextLabelNum() };

    // initializer
    if(_for->hasInitializer()){
        generateAssignmentStatement(_for->getInitializer());
    }
    _asm.genLabel(codeGenContext.asmCode, std::format("_for{}", labNum));

    // no condition is treated as true
    if(_for->hasCondition()){
        if(_for->hasTemporaries()){
            generateTemporaries(_for->getTemporaries());
        }
        generateRelationalExpression(_for->getCondition());

        // condition jump
        _asm.genJmp(codeGenContext.asmCode, nodeToOppJMP.at(_for->getCondition()->getNodeType()), std::format("_for{}_end", labNum));
    }

    generateStatement(_for->getStatement());

    // increment
    if(_for->hasIncrementer()){
        generateAssignmentStatement(_for->getIncrementer());
    }
    _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("_for{}", labNum));

    _asm.genLabel(codeGenContext.asmCode, std::format("_for{}_end", labNum));
}

void CodeGenerator::generateDoWhileStatement(const IRDoWhileSt* _dowhile){
    size_t labNum{ getNextLabelNum() };

    _asm.genLabel(codeGenContext.asmCode, std::format("_do_while{}", labNum));
    generateStatement(_dowhile->getStatement());
    
    if(_dowhile->hasTemporaries()){
        generateTemporaries(_dowhile->getTemporaries());
    }

    generateRelationalExpression(_dowhile->getCondition());
    _asm.genJmp(codeGenContext.asmCode, nodeToJMP.at(_dowhile->getCondition()->getNodeType()), std::format("_do_while{}", labNum));
}

void CodeGenerator::generateCompoundStatement(const IRCompoundSt* _compound){
    for(const auto& _statement : _compound->getStatements()){
        generateStatement(_statement.get());
    }
}

// evaluating rvalue
void CodeGenerator::generateAssignmentStatement(const IRAssignSt* _assignment){
    // preventing register corruption when function call occurs
    if(_assignment->hasTemporaries()){
        generateTemporaries(_assignment->getTemporaries());
    }

    generateNumericalExpression(_assignment->getExp());
    freeGpReg(codeGenContext.gpFreeRegPos);
    _asm.genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), generateID(_assignment->getVariable()), "q");
}

// return value ends up in %rax
void CodeGenerator::generateReturnStatement(const IRReturnSt* _return){
    if(_return->returns()){
        // preventing register corruption when function call occurs
        if(_return->hasTemporaries()){
            generateTemporaries(_return->getTemporaries());
        }

        generateNumericalExpression(_return->getExp());
        freeGpReg(codeGenContext.gpFreeRegPos);
        _asm.genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), "%rax", "q");
    } 
    else{
        _asm.genOperation(codeGenContext.asmCode, "xor", "%rax", "%rax");
    }
    _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("{}_end", codeGenContext.functionName));
}

void CodeGenerator::generateSwitchStatement(const IRSwitchSt* _switch){
    size_t labNum{ getNextLabelNum() };

    _asm.genLabel(codeGenContext.asmCode, std::format("_switch{}", labNum));

    size_t size{ _switch->getCaseCount() };
    std::string var{ _switch->getVariable()->getIdName() };
    
    // cases
    for(size_t i = 0; i < size; i++){
        const IRCaseSt* _case = _switch->getCaseAtN(i);

        _asm.genLabel(codeGenContext.asmCode, std::format("_switch{}_case{}", labNum, i));
        _asm.genMov(codeGenContext.asmCode, codeGenContext.variableMap.at(var), "%rcx", "q");
        _asm.genMov(codeGenContext.asmCode, generateLiteral(_case->getLiteral()), "%rdx", "q");
        _asm.genCmp(codeGenContext.asmCode, "%rcx", "%rdx");
        
        std::string jmpLabel = "";
        if(i < size - 1){
            jmpLabel = std::format("_switch{}_{}", labNum, ("case" + std::to_string(i + 1)));
        }
        else if(_switch->hasDefault()){
            jmpLabel = std::format("_switch{}_default", labNum);
        }
        else{
            jmpLabel = std::format("_switch{}_end", labNum);
        }
        _asm.genJmp(codeGenContext.asmCode, "jne", jmpLabel);
        
        for(const auto& _statement : _case->getSwitchBlock()->getStatements()){
            generateStatement(_statement.get());
        }

        if(_case->hasBreak()){
            _asm.genJmp(codeGenContext.asmCode, "jmp", std::format("_switch{}_end", labNum));
        }
    }
    if(_switch->hasDefault()){
        _asm.genLabel(codeGenContext.asmCode, std::format("_switch{}_default", labNum));
        for(const auto& _statement : _switch->getDefault()->getSwitchBlock()->getStatements()){
            generateStatement(_statement.get());
        }
    }
    _asm.genLabel(codeGenContext.asmCode, std::format("_switch{}_end", labNum));
}

// evaluating equations using general-purpose registers r(8-15) and stack (if necessary)
void CodeGenerator::generateNumericalExpression(const IRExpression* _numexp){
    if(_numexp->getNodeType() == IRNodeType::ID){
        if(codeGenContext.gpFreeRegPos < gpRegisters.size()){
            _asm.genMov(codeGenContext.asmCode, generateID(static_cast<const IRId*>(_numexp)), gpRegisters.at(codeGenContext.gpFreeRegPos), "q");
        }
        else{
            _asm.genPush(codeGenContext.asmCode, generateID(static_cast<const IRId*>(_numexp)));
        }
        takeGpReg(codeGenContext.gpFreeRegPos);
    }
    else if(_numexp->getNodeType() == IRNodeType::LITERAL){
        if(codeGenContext.gpFreeRegPos < gpRegisters.size()){
            _asm.genMov(codeGenContext.asmCode, generateLiteral(static_cast<const IRLiteral*>(_numexp)), gpRegisters.at(codeGenContext.gpFreeRegPos), "q");
        }
        else{
            _asm.genPush(codeGenContext.asmCode, generateLiteral(static_cast<const IRLiteral*>(_numexp)));
        }
        takeGpReg(codeGenContext.gpFreeRegPos);
    }else if (_numexp->getNodeType() == IRNodeType::CALL){
        generateFunctionCall(static_cast<const IRFunctionCall*>(_numexp));
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

        const auto _binExp = static_cast<const IRBinaryExpression*>(_numexp);
        
        generateNumericalExpression(_binExp->getLeftOperand());
        generateNumericalExpression(_binExp->getRightOperand());
        
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

void CodeGenerator::generateRelationalExpression(const IRExpression* _relexp){
    const auto _binExp = static_cast<const IRBinaryExpression*>(_relexp);
    generateNumericalExpression(_binExp->getLeftOperand());
    generateNumericalExpression(_binExp->getRightOperand());
    
    freeGpReg(codeGenContext.gpFreeRegPos);
    std::string lreg{ gpRegisters.at(codeGenContext.gpFreeRegPos) };
    freeGpReg(codeGenContext.gpFreeRegPos);
    std::string rreg{ gpRegisters.at(codeGenContext.gpFreeRegPos) };
    _asm.genCmp(codeGenContext.asmCode, lreg, rreg);
}

const std::string& CodeGenerator::generateID(const IRId* _id) const {
    return codeGenContext.variableMap.at(_id->getIdName()); // returns address from variable map
}

std::string CodeGenerator::generateLiteral(const IRLiteral* _literal) const {
    std::string val{ _literal->getValue() };
    if(_literal->getType() == Types::UNSIGNED){
        val.pop_back();
    }
    return std::format("${}", val);
}

void CodeGenerator::generateFunctionCall(const IRFunctionCall* _functionCall){
    // push arguments to stack
    generateArgument(_functionCall);

    _asm.genCall(codeGenContext.asmCode, _functionCall->getCallName());

    // pop arguments from stack
    clearArguments(_functionCall->getArgumentCount());
}

void CodeGenerator::generateArgument(const IRFunctionCall* _functionCall){
    // evaluating temporaries
    for(const auto& temp : _functionCall->getTemporaries()){
        if(temp != nullptr){
            generateTemporaries(temp.get());
        }
    }
    // pushing arguments onto stack
    for(int i = _functionCall->getArgumentCount() - 1; i >= 0; --i){
        generateNumericalExpression(_functionCall->getArgumentAtN(static_cast<size_t>(i)));
        freeGpReg(codeGenContext.gpFreeRegPos);
        if(codeGenContext.gpFreeRegPos < gpRegisters.size()){ // if >= gpRegisters.size() argument is already pushed
            _asm.genPush(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos));
        }
    }
}

void CodeGenerator::clearArguments(size_t argCount){
    // popping arguments of the stack
    _asm.genOperation(codeGenContext.asmCode, "add", std::format("${}", argCount*regSize), "%rsp");
}

void CodeGenerator::generateTemporaries(const IRTemporary* _temporary){
    for(size_t i = 0; i < _temporary->getTemporaries().size(); ++i){
        const auto temp = _temporary->getExpressionAtN(i);

        if(temp->getNodeType() == IRNodeType::CALL){
            for(const auto& _temp : static_cast<const IRFunctionCall*>(temp)->getTemporaries()){
                if(_temp != nullptr){
                    generateTemporaries(_temp.get());
                }
            }
        }

        codeGenContext.variableMap.insert({_temporary->getTemporaryNameAtN(i), std::format("-{}(%rbp)", codeGenContext.variableNum * regSize)});
        ++codeGenContext.variableNum;

        generateNumericalExpression(temp);
        freeGpReg(codeGenContext.gpFreeRegPos);
        _asm.genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), 
            codeGenContext.variableMap.at(_temporary->getTemporaryNameAtN(i)), "q"
        );
    }
}