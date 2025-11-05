#include "../statement_code_generator.hpp"
#include "../function_code_generator.hpp"
#include "../code_generator.hpp"

#include "../../asm-generator/asm_instruction_generator.hpp"

void StatementCodeGenerator::generateStatement(const IRStatement* _statement){
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

void StatementCodeGenerator::generateVariable(const IRVariable* _variable){
    auto& codeGenContext = FunctionCodeGenerator::getContext();
    // mapping local variable to address relative to %rbp (-n(%rbp))
    // if not successful it means that variable with the given name existed but went out of scope, so it overwrites it with new memory location
    auto [varPtr, success]{ codeGenContext.variableMap.insert({_variable->getVarName(), std::format("-{}(%rbp)", codeGenContext.variableNum * AsmGenerator::Instruction::regSize)}) };
    if(!success){
        varPtr->second = std::format("-{}(%rbp)", codeGenContext.variableNum * AsmGenerator::Instruction::regSize);
    }
    ++codeGenContext.variableNum;

    // direct initialization / default value assignation
    if(_variable->hasAssign()){
        if(_variable->hasTemporaries()){
            exprGenerator.generateTemporaries(_variable->getTemporaries());
        }

        exprGenerator.generateNumericalExpression(_variable->getAssign());
        codeGenContext.freeGpReg();
        AsmGenerator::Instruction::genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), codeGenContext.variableMap.at(_variable->getVarName()), "q");
    }
    else{
        // default value 
        AsmGenerator::Instruction::genMov(codeGenContext.asmCode, "$0", codeGenContext.variableMap.at(_variable->getVarName()), "q");
    }
}

void StatementCodeGenerator::generatePrintfStatement(const IRPrintfSt* _printf){
    FunctionCodeGenerator::updatePrints(true);

    // preventing register corruption when function call occurs
    if(_printf->hasTemporaries()){
        exprGenerator.generateTemporaries(_printf->getTemporaries());
    }
    exprGenerator.generateNumericalExpression(_printf->getExp());

    auto& codeGenContext = FunctionCodeGenerator::getContext();
    codeGenContext.freeGpReg();
    AsmGenerator::Instruction::genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), "%rax", "q");
    AsmGenerator::Instruction::genCall(codeGenContext.asmCode, "_printf");
}

void StatementCodeGenerator::generateIfStatement(const IRIfSt* _if){
    size_t labNum{ CodeGenerator::getNextLabelNum() };

    size_t size{ _if->getConditionCount() };

    auto [_ifCondition, _ifStatement, _ifTemp] = _if->getIfAtN(0);

    auto& codeGenContext = FunctionCodeGenerator::getContext();
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_if{}", labNum));
    if(_ifTemp != nullptr){
        exprGenerator.generateTemporaries(_ifTemp);
    }
    exprGenerator.generateRelationalExpression(_ifCondition);

    std::string jmpLabel{ "" };
    if(size > 1){
        jmpLabel = std::format("_elif{}_0", labNum);
    }
    else if(_if->hasElse()){
        jmpLabel = std::format("_else{}", labNum);
    }
    else{
        jmpLabel = std::format("_if{}_end", labNum);
    }

    AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, nodeToOppJMP.at(_ifCondition->getNodeType()), jmpLabel);

    generateStatement(_ifStatement);
    AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jmp", std::format("_if{}_end", labNum));

    for(size_t i = 1; i < size; ++i){
        auto [_elifCondition, _elifStatement, _elifTemp] = _if->getIfAtN(i);
        AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_elif{}_{}", labNum, i-1));
        
        if(_elifTemp != nullptr){
            exprGenerator.generateTemporaries(_elifTemp);
        }
        exprGenerator.generateRelationalExpression(_elifCondition);

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

        AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, nodeToOppJMP.at(_elifCondition->getNodeType()), jmpLabel);

        generateStatement(_elifStatement);
        AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jmp", std::format("_if{}_end", labNum));
    }

    if(_if->hasElse()){
        AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_else{}", labNum));
        generateStatement(_if->getElseStatement());
    }
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_if{}_end", labNum));
}

void StatementCodeGenerator::generateWhileStatement(const IRWhileSt* _while){
    size_t labNum{ CodeGenerator::getNextLabelNum() };

    auto& codeGenContext = FunctionCodeGenerator::getContext();
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_while{}", labNum));
    if(_while->hasTemporaries()){
        exprGenerator.generateTemporaries(_while->getTemporaries());
    }
    exprGenerator.generateRelationalExpression(_while->getCondition());

    AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, nodeToOppJMP.at(_while->getCondition()->getNodeType()), std::format("_while{}_end", labNum));

    generateStatement(_while->getStatement());
    AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jmp", std::format("_while{}", labNum));
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_while{}_end", labNum));
}

void StatementCodeGenerator::generateForStatement(const IRForSt* _for){
    size_t labNum{ CodeGenerator::getNextLabelNum() };

    auto& codeGenContext = FunctionCodeGenerator::getContext();
    // initializer
    if(_for->hasInitializer()){
        generateAssignmentStatement(_for->getInitializer());
    }
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_for{}", labNum));

    // no condition is treated as true
    if(_for->hasCondition()){
        if(_for->hasTemporaries()){
            exprGenerator.generateTemporaries(_for->getTemporaries());
        }
        exprGenerator.generateRelationalExpression(_for->getCondition());

        // condition jump
        AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, nodeToOppJMP.at(_for->getCondition()->getNodeType()), std::format("_for{}_end", labNum));
    }

    generateStatement(_for->getStatement());

    // increment
    if(_for->hasIncrementer()){
        generateAssignmentStatement(_for->getIncrementer());
    }
    AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jmp", std::format("_for{}", labNum));

    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_for{}_end", labNum));
}

void StatementCodeGenerator::generateDoWhileStatement(const IRDoWhileSt* _dowhile){
    size_t labNum{ CodeGenerator::getNextLabelNum() };
    auto& codeGenContext = FunctionCodeGenerator::getContext();

    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_do_while{}", labNum));
    generateStatement(_dowhile->getStatement());
    
    if(_dowhile->hasTemporaries()){
        exprGenerator.generateTemporaries(_dowhile->getTemporaries());
    }

    exprGenerator.generateRelationalExpression(_dowhile->getCondition());
    AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, nodeToJMP.at(_dowhile->getCondition()->getNodeType()), std::format("_do_while{}", labNum));
}

void StatementCodeGenerator::generateCompoundStatement(const IRCompoundSt* _compound){
    for(const auto& _statement : _compound->getStatements()){
        generateStatement(_statement.get());
    }
}

// evaluating rvalue
void StatementCodeGenerator::generateAssignmentStatement(const IRAssignSt* _assignment){
    // preventing register corruption when function call occurs
    if(_assignment->hasTemporaries()){
        exprGenerator.generateTemporaries(_assignment->getTemporaries());
    }

    auto& codeGenContext = FunctionCodeGenerator::getContext();
    exprGenerator.generateNumericalExpression(_assignment->getExp());
    codeGenContext.freeGpReg();
    AsmGenerator::Instruction::genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), exprGenerator.generateID(_assignment->getVariable()), "q");
}

// return value ends up in %rax
void StatementCodeGenerator::generateReturnStatement(const IRReturnSt* _return){
    auto& codeGenContext = FunctionCodeGenerator::getContext();
    if(_return->returns()){
        // preventing register corruption when function call occurs
        if(_return->hasTemporaries()){
            exprGenerator.generateTemporaries(_return->getTemporaries());
        }

        exprGenerator.generateNumericalExpression(_return->getExp());
        codeGenContext.freeGpReg();
        AsmGenerator::Instruction::genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), "%rax", "q");
    } 
    else{
        AsmGenerator::Instruction::genOperation(codeGenContext.asmCode, "xor", "%rax", "%rax");
    }
    AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jmp", std::format("{}_end", codeGenContext.functionName));
}

void StatementCodeGenerator::generateSwitchStatement(const IRSwitchSt* _switch){
    size_t labNum{ CodeGenerator::getNextLabelNum() };
    auto& codeGenContext = FunctionCodeGenerator::getContext();

    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_switch{}", labNum));

    size_t size{ _switch->getCaseCount() };
    std::string var{ _switch->getVariable()->getIdName() };
    
    // cases
    for(size_t i = 0; i < size; i++){
        const IRCaseSt* _case = _switch->getCaseAtN(i);

        AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_switch{}_case{}", labNum, i));
        AsmGenerator::Instruction::genMov(codeGenContext.asmCode, codeGenContext.variableMap.at(var), "%rcx", "q");
        AsmGenerator::Instruction::genMov(codeGenContext.asmCode, exprGenerator.generateLiteral(_case->getLiteral()), "%rdx", "q");
        AsmGenerator::Instruction::genCmp(codeGenContext.asmCode, "%rcx", "%rdx");
        
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
        AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jne", jmpLabel);
        
        for(const auto& _statement : _case->getSwitchBlock()->getStatements()){
            generateStatement(_statement.get());
        }

        if(_case->hasBreak()){
            AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jmp", std::format("_switch{}_end", labNum));
        }
    }
    if(_switch->hasDefault()){
        AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_switch{}_default", labNum));
        for(const auto& _statement : _switch->getDefault()->getSwitchBlock()->getStatements()){
            generateStatement(_statement.get());
        }
    }
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_switch{}_end", labNum));
}