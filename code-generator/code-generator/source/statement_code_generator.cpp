#include "../statement_code_generator.hpp"

#include <format>

#include "../code_generator.hpp"
#include "../function_code_generator.hpp"
#include "../../asm-generator/asm_instruction_generator.hpp"

void StatementCodeGenerator::generateStmt(const IRStmt* stmt){
    switch(stmt->getNodeType()){
        case IRNodeType::VARIABLE:
            generateVariableDeclStmt(static_cast<const IRVariableDeclStmt*>(stmt));
            break;
        case IRNodeType::IF:
            generateIfStmt(static_cast<const IRIfStmt*>(stmt));
            break;
        case IRNodeType::COMPOUND:
            generateCompoundStmt(static_cast<const IRCompoundStmt*>(stmt));
            break;
        case IRNodeType::ASSIGN:
            generateAssignStmt(static_cast<const IRAssignStmt*>(stmt));
            break;
        case IRNodeType::RETURN:
            generateReturnStmt(static_cast<const IRReturnStmt*>(stmt));
            break;
        case IRNodeType::WHILE:
            generateWhileStmt(static_cast<const IRWhileStmt*>(stmt));
            break;
        case IRNodeType::FOR:
            generateForStmt(static_cast<const IRForStmt*>(stmt));
            break;
        case IRNodeType::DO_WHILE:
            generateDoWhileStmt(static_cast<const IRDoWhileStmt*>(stmt));
            break;
        case IRNodeType::SWITCH:
            generateSwitchStmt(static_cast<const IRSwitchStmt*>(stmt));
            break;
        case IRNodeType::CALL:
            generateFunctionCallStmt(static_cast<const IRFunctionCallStmt*>(stmt));
            return;
        default:
            return;
    }
}

void StatementCodeGenerator::generateVariableDeclStmt(const IRVariableDeclStmt* variableDecl){
    auto& codeGenContext = FunctionCodeGenerator::getContext();
    // mapping local variable to address relative to %rbp (-n(%rbp))
    // if not successful it means that variable with the given name existed but went out of scope, so it overwrites it with new memory location
    auto [varPtr, success]{ codeGenContext.variableMap.insert({variableDecl->getVarName(), std::format("-{}(%rbp)", codeGenContext.variableNum * AsmGenerator::Instruction::regSize)}) };
    if(!success){
        varPtr->second = std::format("-{}(%rbp)", codeGenContext.variableNum * AsmGenerator::Instruction::regSize);
    }
    ++codeGenContext.variableNum;

    // direct initialization / default value assignation
    if(variableDecl->hasAssign()){
        if(variableDecl->hasTemporaries()){
            exprGenerator.generateTemporaryExprs(variableDecl->getTemporaries());
        }

        exprGenerator.generateNumericalExpr(variableDecl->getAssign());
        codeGenContext.freeGpReg();
        AsmGenerator::Instruction::genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), codeGenContext.variableMap.at(variableDecl->getVarName()), "q");
    }
    else{
        // default value 
        AsmGenerator::Instruction::genMov(codeGenContext.asmCode, "$0", codeGenContext.variableMap.at(variableDecl->getVarName()), "q");
    }
}

void StatementCodeGenerator::generateIfStmt(const IRIfStmt* ifStmt){
    size_t labNum{ CodeGenerator::getNextLabelNum() };
    size_t size{ ifStmt->getConditionCount() };
    std::string jmpLabel{ "" };

    auto& codeGenContext = FunctionCodeGenerator::getContext();

    for(size_t i = 0; i < size; ++i){
        auto [condition, stmt, tempExpr] = ifStmt->getIfAtN(i);
        AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_if{}_{}", labNum, i));
        
        if(tempExpr != nullptr){
            exprGenerator.generateTemporaryExprs(tempExpr);
        }
        exprGenerator.generateRelationalExpr(condition);

        if(i == size - 1 && ifStmt->hasElse()){
            jmpLabel = std::format("_else{}", labNum);
        }
        else if(i < size - 1){
            jmpLabel = std::format("_if{}_{}", labNum, i + 1);
        }
        else {
            jmpLabel = std::format("_if{}_end", labNum);
        }

        AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, nodeToOppJMP.at(condition->getNodeType()), jmpLabel);

        generateStmt(stmt);
        AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jmp", std::format("_if{}_end", labNum));
    }

    if(ifStmt->hasElse()){
        AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_else{}", labNum));
        generateStmt(ifStmt->getElseStatement());
    }
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_if{}_end", labNum));
}

void StatementCodeGenerator::generateWhileStmt(const IRWhileStmt* whileStmt){
    size_t labNum{ CodeGenerator::getNextLabelNum() };

    auto& codeGenContext = FunctionCodeGenerator::getContext();
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_while{}", labNum));
    if(whileStmt->hasTemporaries()){
        exprGenerator.generateTemporaryExprs(whileStmt->getTemporaries());
    }
    exprGenerator.generateRelationalExpr(whileStmt->getCondition());

    AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, nodeToOppJMP.at(whileStmt->getCondition()->getNodeType()), std::format("_while{}_end", labNum));

    generateStmt(whileStmt->getStatement());
    AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jmp", std::format("_while{}", labNum));
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_while{}_end", labNum));
}

void StatementCodeGenerator::generateForStmt(const IRForStmt* forStmt){
    size_t labNum{ CodeGenerator::getNextLabelNum() };

    auto& codeGenContext = FunctionCodeGenerator::getContext();
    // initializer
    if(forStmt->hasInitializer()){
        generateAssignStmt(forStmt->getInitializer());
    }
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_for{}", labNum));

    // no condition is treated as true
    if(forStmt->hasCondition()){
        if(forStmt->hasTemporaries()){
            exprGenerator.generateTemporaryExprs(forStmt->getTemporaries());
        }
        exprGenerator.generateRelationalExpr(forStmt->getCondition());

        // condition jump
        AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, nodeToOppJMP.at(forStmt->getCondition()->getNodeType()), std::format("_for{}_end", labNum));
    }

    generateStmt(forStmt->getStatement());

    // increment
    if(forStmt->hasIncrementer()){
        generateAssignStmt(forStmt->getIncrementer());
    }
    AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jmp", std::format("_for{}", labNum));

    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_for{}_end", labNum));
}

void StatementCodeGenerator::generateDoWhileStmt(const IRDoWhileStmt* dowhileStmt){
    size_t labNum{ CodeGenerator::getNextLabelNum() };
    auto& codeGenContext = FunctionCodeGenerator::getContext();

    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_do_while{}", labNum));
    generateStmt(dowhileStmt->getStatement());
    
    if(dowhileStmt->hasTemporaries()){
        exprGenerator.generateTemporaryExprs(dowhileStmt->getTemporaries());
    }

    exprGenerator.generateRelationalExpr(dowhileStmt->getCondition());
    AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, nodeToJMP.at(dowhileStmt->getCondition()->getNodeType()), std::format("_do_while{}", labNum));
}

void StatementCodeGenerator::generateCompoundStmt(const IRCompoundStmt* compoundStmt){
    for(const auto& stmt : compoundStmt->getStatements()){
        generateStmt(stmt.get());
    }
}

// evaluating rvalue
void StatementCodeGenerator::generateAssignStmt(const IRAssignStmt* assignStmt){
    // preventing register corruption when function call occurs
    if(assignStmt->hasTemporaries()){
        exprGenerator.generateTemporaryExprs(assignStmt->getTemporaries());
    }

    auto& codeGenContext = FunctionCodeGenerator::getContext();
    exprGenerator.generateNumericalExpr(assignStmt->getExp());
    codeGenContext.freeGpReg();
    AsmGenerator::Instruction::genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), exprGenerator.generateIDExpr(assignStmt->getVariable()), "q");
}

// return value ends up in %rax
void StatementCodeGenerator::generateReturnStmt(const IRReturnStmt* returnStmt){
    auto& codeGenContext = FunctionCodeGenerator::getContext();
    if(returnStmt->returns()){
        // preventing register corruption when function call occurs
        if(returnStmt->hasTemporaries()){
            exprGenerator.generateTemporaryExprs(returnStmt->getTemporaries());
        }

        exprGenerator.generateNumericalExpr(returnStmt->getExp());
        codeGenContext.freeGpReg();
        AsmGenerator::Instruction::genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), "%rax", "q");
    } 
    else{
        AsmGenerator::Instruction::genOperation(codeGenContext.asmCode, "xor", "%rax", "%rax");
    }
    AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jmp", std::format("{}_end", codeGenContext.functionName));
}

void StatementCodeGenerator::generateFunctionCallStmt(const IRFunctionCallStmt* callStmt){
    exprGenerator.generateFunctionCallExpr(callStmt->getFunctionCall());
}

void StatementCodeGenerator::generateSwitchStmt(const IRSwitchStmt* switchStmt){
    size_t labNum{ CodeGenerator::getNextLabelNum() };
    auto& codeGenContext = FunctionCodeGenerator::getContext();

    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_switch{}", labNum));

    size_t size{ switchStmt->getCaseCount() };
    std::string var{ switchStmt->getVariable()->getIdName() };
    
    // cases
    for(size_t i = 0; i < size; i++){
        const IRCaseStmt* caseStmt = switchStmt->getCaseAtN(i);

        AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_switch{}_case{}", labNum, i));
        AsmGenerator::Instruction::genMov(codeGenContext.asmCode, codeGenContext.variableMap.at(var), "%rcx", "q");
        AsmGenerator::Instruction::genMov(codeGenContext.asmCode, exprGenerator.generateLiteralExpr(caseStmt->getLiteral()), "%rdx", "q");
        AsmGenerator::Instruction::genCmp(codeGenContext.asmCode, "%rcx", "%rdx");
        
        std::string jmpLabel = "";
        if(i < size - 1){
            jmpLabel = std::format("_switch{}_{}", labNum, ("case" + std::to_string(i + 1)));
        }
        else if(switchStmt->hasDefault()){
            jmpLabel = std::format("_switch{}_default", labNum);
        }
        else{
            jmpLabel = std::format("_switch{}_end", labNum);
        }
        AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jne", jmpLabel);
        
        for(const auto& stmt : caseStmt->getSwitchBlock()->getStatements()){
            generateStmt(stmt.get());
        }

        if(caseStmt->hasBreak()){
            AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jmp", std::format("_switch{}_end", labNum));
        }
    }
    if(switchStmt->hasDefault()){
        AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_switch{}_default", labNum));
        for(const auto& stmt : switchStmt->getDefault()->getSwitchBlock()->getStatements()){
            generateStmt(stmt.get());
        }
    }
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_switch{}_end", labNum));
}