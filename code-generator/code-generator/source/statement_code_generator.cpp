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
    auto [varPtr, success]{ 
        codeGenContext.variableMap.insert({
            variableDecl->getVarName(), 
            std::format("-{}(%rbp)", codeGenContext.variableNum * AsmGenerator::Instruction::regSize)
        }) 
    };
    if(!success){
        varPtr->second = std::format("-{}(%rbp)", codeGenContext.variableNum * AsmGenerator::Instruction::regSize);
    }
    ++codeGenContext.variableNum;

    // direct initialization / default value assignation
    if(variableDecl->hasAssignExpr()){
        if(variableDecl->hasTemporaryExpr()){
            exprGenerator.generateTemporaryExprs(variableDecl->getTemporaryExpr());
        }

        exprGenerator.generateNumericalExpr(variableDecl->getAssignExpr());
        codeGenContext.freeGpReg();
        AsmGenerator::Instruction::genMov(
            codeGenContext.asmCode, 
            gpRegisters.at(codeGenContext.gpFreeRegPos), 
            codeGenContext.variableMap.at(variableDecl->getVarName()), 
            "q"
        );
    }
    else{
        // default value 
        AsmGenerator::Instruction::genMov(
            codeGenContext.asmCode, 
            "$0", 
            codeGenContext.variableMap.at(variableDecl->getVarName()), 
            "q"
        );
    }
}

void StatementCodeGenerator::generateIfStmt(const IRIfStmt* ifStmt){
    size_t labNum{ CodeGenerator::getNextLabelNum() };
    size_t size{ ifStmt->getConditionCount() };
    std::string jmpLabel{ "" };

    auto& codeGenContext = FunctionCodeGenerator::getContext();

    for(size_t i = 0; i < size; ++i){
        auto [condition, stmt, tempExpr] = ifStmt->getIfStmtAtN(i);
        AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_if{}_{}", labNum, i));
        
        if(tempExpr != nullptr){
            exprGenerator.generateTemporaryExprs(tempExpr);
        }
        exprGenerator.generateRelationalExpr(condition);

        if(i == size - 1 && ifStmt->hasElseStmt()){
            jmpLabel = std::format("_else{}", labNum);
        }
        else if(i < size - 1){
            jmpLabel = std::format("_if{}_{}", labNum, i + 1);
        }
        else {
            jmpLabel = std::format("_if{}_end", labNum);
        }

        AsmGenerator::Instruction::genJmp(
            codeGenContext.asmCode, 
            nodeToOppJMP.at(condition->getNodeType()), 
            jmpLabel
        );

        generateStmt(stmt);
        AsmGenerator::Instruction::genJmp(
            codeGenContext.asmCode, 
            "jmp", 
            std::format("_if{}_end", labNum)
        );
    }

    if(ifStmt->hasElseStmt()){
        AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_else{}", labNum));
        generateStmt(ifStmt->getElseStmt());
    }
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_if{}_end", labNum));
}

void StatementCodeGenerator::generateWhileStmt(const IRWhileStmt* whileStmt){
    size_t labNum{ CodeGenerator::getNextLabelNum() };

    auto& codeGenContext = FunctionCodeGenerator::getContext();
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_while{}", labNum));
    if(whileStmt->hasTemporaryExpr()){
        exprGenerator.generateTemporaryExprs(whileStmt->getTemporaryExpr());
    }
    exprGenerator.generateRelationalExpr(whileStmt->getConditionExpr());

    AsmGenerator::Instruction::genJmp(
        codeGenContext.asmCode, 
        nodeToOppJMP.at(whileStmt->getConditionExpr()->getNodeType()), 
        std::format("_while{}_end", labNum)
    );

    generateStmt(whileStmt->getStmt());
    AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jmp", std::format("_while{}", labNum));
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_while{}_end", labNum));
}

void StatementCodeGenerator::generateForStmt(const IRForStmt* forStmt){
    size_t labNum{ CodeGenerator::getNextLabelNum() };

    auto& codeGenContext = FunctionCodeGenerator::getContext();
    // initializer
    if(forStmt->hasInitializerStmt()){
        generateAssignStmt(forStmt->getInitializerStmt());
    }
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_for{}", labNum));

    // no condition is treated as true
    if(forStmt->hasConditionExpr()){
        if(forStmt->hasTemporaryExpr()){
            exprGenerator.generateTemporaryExprs(forStmt->getTemporaryExpr());
        }
        exprGenerator.generateRelationalExpr(forStmt->getConditionExpr());

        // condition jump
        AsmGenerator::Instruction::genJmp(
            codeGenContext.asmCode, 
            nodeToOppJMP.at(forStmt->getConditionExpr()->getNodeType()), 
            std::format("_for{}_end", labNum)
        );
    }

    generateStmt(forStmt->getStmt());

    // increment
    if(forStmt->hasIncrementerStmt()){
        generateAssignStmt(forStmt->getIncrementerStmt());
    }
    AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jmp", std::format("_for{}", labNum));

    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_for{}_end", labNum));
}

void StatementCodeGenerator::generateDoWhileStmt(const IRDoWhileStmt* dowhileStmt){
    size_t labNum{ CodeGenerator::getNextLabelNum() };
    auto& codeGenContext = FunctionCodeGenerator::getContext();

    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_do_while{}", labNum));
    generateStmt(dowhileStmt->getStmt());
    
    if(dowhileStmt->hasTemporaryExpr()){
        exprGenerator.generateTemporaryExprs(dowhileStmt->getTemporaryExpr());
    }

    exprGenerator.generateRelationalExpr(dowhileStmt->getConditionExpr());
    AsmGenerator::Instruction::genJmp(
        codeGenContext.asmCode, 
        nodeToJMP.at(dowhileStmt->getConditionExpr()->getNodeType()), 
        std::format("_do_while{}", labNum)
    );
}

void StatementCodeGenerator::generateCompoundStmt(const IRCompoundStmt* compoundStmt){
    for(const auto& stmt : compoundStmt->getStmts()){
        generateStmt(stmt.get());
    }
}

// evaluating rvalue
void StatementCodeGenerator::generateAssignStmt(const IRAssignStmt* assignStmt){
    // preventing register corruption when function call occurs
    if(assignStmt->hasTemporaryExpr()){
        exprGenerator.generateTemporaryExprs(assignStmt->getTemporaryExpr());
    }

    auto& codeGenContext = FunctionCodeGenerator::getContext();
    exprGenerator.generateNumericalExpr(assignStmt->getAssignedExpr());
    codeGenContext.freeGpReg();
    AsmGenerator::Instruction::genMov(
        codeGenContext.asmCode, 
        gpRegisters.at(codeGenContext.gpFreeRegPos), 
        exprGenerator.generateIDExpr(assignStmt->getVariableIdExpr()), 
        "q"
    );
}

// return value ends up in %rax
void StatementCodeGenerator::generateReturnStmt(const IRReturnStmt* returnStmt){
    auto& codeGenContext = FunctionCodeGenerator::getContext();
    if(returnStmt->hasReturnValue()){
        // preventing register corruption when function call occurs
        if(returnStmt->hasTemporaryExpr()){
            exprGenerator.generateTemporaryExprs(returnStmt->getTemporaryExpr());
        }

        exprGenerator.generateNumericalExpr(returnStmt->getReturnExpr());
        codeGenContext.freeGpReg();
        AsmGenerator::Instruction::genMov(
            codeGenContext.asmCode, 
            gpRegisters.at(codeGenContext.gpFreeRegPos), 
            "%rax", 
            "q"
        );
    } 
    else{
        AsmGenerator::Instruction::genOperation(codeGenContext.asmCode, "xor", "%rax", "%rax");
    }
    AsmGenerator::Instruction::genJmp(
        codeGenContext.asmCode, 
        "jmp", 
        std::format("{}_end", codeGenContext.functionName)
    );
}

void StatementCodeGenerator::generateFunctionCallStmt(const IRFunctionCallStmt* callStmt){
    exprGenerator.generateFunctionCallExpr(callStmt->getFunctionCallExpr());
}

void StatementCodeGenerator::generateSwitchStmt(const IRSwitchStmt* switchStmt){
    size_t labNum{ CodeGenerator::getNextLabelNum() };
    auto& codeGenContext = FunctionCodeGenerator::getContext();

    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_switch{}", labNum));

    size_t size{ switchStmt->getCaseCount() };
    std::string var{ switchStmt->getVariableIdExpr()->getIdName() };
    
    // cases
    for(size_t i = 0; i < size; i++){
        const IRCaseStmt* caseStmt = switchStmt->getCaseStmtAtN(i);

        AsmGenerator::Instruction::genLabel(
            codeGenContext.asmCode, 
            std::format("_switch{}_case{}", labNum, i)
        );
        AsmGenerator::Instruction::genMov(
            codeGenContext.asmCode, 
            codeGenContext.variableMap.at(var), 
            "%rcx", 
            "q"
        );
        AsmGenerator::Instruction::genMov(
            codeGenContext.asmCode, 
            exprGenerator.generateLiteralExpr(caseStmt->getLiteralExpr()), 
            "%rdx", 
            "q"
        );
        AsmGenerator::Instruction::genCmp(codeGenContext.asmCode, "%rcx", "%rdx");
        
        std::string jmpLabel = "";
        if(i < size - 1){
            jmpLabel = std::format("_switch{}_{}", labNum, ("case" + std::to_string(i + 1)));
        }
        else if(switchStmt->hasDefaultStmt()){
            jmpLabel = std::format("_switch{}_default", labNum);
        }
        else{
            jmpLabel = std::format("_switch{}_end", labNum);
        }
        AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jne", jmpLabel);
        
        for(const auto& stmt : caseStmt->getSwitchBlockStmt()->getStmts()){
            generateStmt(stmt.get());
        }

        if(caseStmt->hasBreakStmt()){
            AsmGenerator::Instruction::genJmp(
                codeGenContext.asmCode, 
                "jmp", 
                std::format("_switch{}_end", labNum)
            );
        }
    }
    if(switchStmt->hasDefaultStmt()){
        AsmGenerator::Instruction::genLabel(
            codeGenContext.asmCode, 
            std::format("_switch{}_default", labNum)
        );
        for(const auto& stmt : switchStmt->getDefaultStmt()->getSwitchBlockStmt()->getStmts()){
            generateStmt(stmt.get());
        }
    }
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("_switch{}_end", labNum));
}