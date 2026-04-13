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
        case IRNodeType::CALL_STMT:
            generateFunctionCallStmt(static_cast<const IRFunctionCallStmt*>(stmt));
            return;
        default:
            return;
    }
}

void StatementCodeGenerator::generateVariableDeclStmt(const IRVariableDeclStmt* variableDecl){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    // mapping local variable to address relative to %rbp (-n(%rbp))
    // if not successful it means that variable with the given name existed but went out of scope, 
    // so it overwrites it with new memory location
    auto [varPtr, success]{ 
        codeGenContext.variableMap.insert({
            variableDecl->getVarName(), 
            std::format(
                "-{}(%rbp)", 
                codeGenContext.variableNum * AsmGenerator::Instruction::regSize
            )
        }) 
    };
    if(!success){
        varPtr->second = std::format(
            "-{}(%rbp)", 
            codeGenContext.variableNum * AsmGenerator::Instruction::regSize
        );
    }
    ++codeGenContext.variableNum;

    // direct initialization / default value assignation
    if(variableDecl->hasAssignExpr()){
        if(variableDecl->hasTemporaryExpr()){
            exprGenerator.generateTemporaryExprs(variableDecl->getTemporaryExpr());
        }

        exprGenerator.generateExpr(variableDecl->getAssignExpr());
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
    std::string bodyStartLabel{ "" };
    std::string elseLabel{ std::format("_else{}", labNum) };
    std::string endLabel{ std::format("_if{}_end", labNum) };

    auto conditionLabel = [labNum](size_t i) -> std::string {
        return std::format("_if{}_{}", labNum, i);
    };
    auto bodyLabel = [labNum](size_t i) -> std::string {
        return std::format("_if{}_body{}", labNum, i);
    };

    auto& codeGenContext{ FunctionCodeGenerator::getContext() };

    for(size_t i{0}; i < size; ++i){
        auto [condition, stmt, tempExpr]{ 
            ifStmt->getIfStmtAtN(i) 
        };
        AsmGenerator::Instruction::genLabel(
            codeGenContext.asmCode, 
            conditionLabel(i)
        );
        
        if(tempExpr != nullptr){
            exprGenerator.generateTemporaryExprs(tempExpr);
        }

        if(i == size - 1 && ifStmt->hasElseStmt()){
            jmpLabel = elseLabel;
        }
        else if(i < size - 1){
            jmpLabel = conditionLabel(i + 1);
        }
        else {
            jmpLabel = endLabel;
        }
        bodyStartLabel = bodyLabel(i + 1);
        exprGenerator.generateConditionExpr(
            condition, 
            bodyStartLabel, 
            jmpLabel
        );

        AsmGenerator::Instruction::genLabel(
            codeGenContext.asmCode, 
            bodyStartLabel
        );

        generateStmt(stmt);
        AsmGenerator::Instruction::genJmp(
            codeGenContext.asmCode, 
            "jmp", 
            endLabel
        );
    }

    if(ifStmt->hasElseStmt()){
        AsmGenerator::Instruction::genLabel(
            codeGenContext.asmCode, 
            elseLabel
        );
        generateStmt(ifStmt->getElseStmt());
    }
    AsmGenerator::Instruction::genLabel(
        codeGenContext.asmCode, 
        endLabel
    );
}

void StatementCodeGenerator::generateWhileStmt(const IRWhileStmt* whileStmt){
    size_t labNum{ CodeGenerator::getNextLabelNum() };

    std::string startLabel{ std::format("_while{}", labNum) };
    std::string bodyLabel{ std::format("_while{}_body", labNum) };
    std::string endLabel{ std::format("_while{}_end", labNum) };

    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    AsmGenerator::Instruction::genLabel(
        codeGenContext.asmCode, 
        startLabel
    );
    if(whileStmt->hasTemporaryExpr()){
        exprGenerator.generateTemporaryExprs(whileStmt->getTemporaryExpr());
    }
    exprGenerator.generateConditionExpr(
        whileStmt->getConditionExpr(), 
        bodyLabel, 
        endLabel
    );

    AsmGenerator::Instruction::genLabel(
        codeGenContext.asmCode, 
        bodyLabel
    );
    generateStmt(whileStmt->getStmt());
    AsmGenerator::Instruction::genJmp(
        codeGenContext.asmCode, 
        "jmp", 
        startLabel
    );

    AsmGenerator::Instruction::genLabel(
        codeGenContext.asmCode, 
        endLabel
    );
}

void StatementCodeGenerator::generateForStmt(const IRForStmt* forStmt){
    size_t labNum{ CodeGenerator::getNextLabelNum() };

    std::string startLabel{ std::format("_for{}", labNum) };
    std::string bodyLabel{ std::format("_for{}_body", labNum) };
    std::string endLabel{ std::format("_for{}_end", labNum) };

    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    // initializer
    if(forStmt->hasInitializerStmt()){
        generateAssignStmt(forStmt->getInitializerStmt());
    }
    AsmGenerator::Instruction::genLabel(
        codeGenContext.asmCode, 
        startLabel
    );

    // no condition is treated as true
    if(forStmt->hasConditionExpr()){
        if(forStmt->hasTemporaryExpr()){
            exprGenerator.generateTemporaryExprs(forStmt->getTemporaryExpr());
        }

        exprGenerator.generateConditionExpr(
            forStmt->getConditionExpr(), 
            bodyLabel, 
            endLabel
        );

        AsmGenerator::Instruction::genLabel(
            codeGenContext.asmCode, 
            bodyLabel
        );
    }

    generateStmt(forStmt->getStmt());

    // increment
    if(forStmt->hasIncrementerStmt()){
        generateAssignStmt(forStmt->getIncrementerStmt());
    }
    AsmGenerator::Instruction::genJmp(
        codeGenContext.asmCode, 
        "jmp", 
        startLabel
    );

    AsmGenerator::Instruction::genLabel(
        codeGenContext.asmCode, 
        endLabel
    );
}

void StatementCodeGenerator::generateDoWhileStmt(const IRDoWhileStmt* dowhileStmt){
    size_t labNum{ CodeGenerator::getNextLabelNum() };
    
    std::string startLabel{ std::format("_do_while{}", labNum) };
    std::string endLabel{ std::format("_do_while{}_end", labNum) };

    auto& codeGenContext{ FunctionCodeGenerator::getContext() };

    AsmGenerator::Instruction::genLabel(
        codeGenContext.asmCode, 
        startLabel
    );
    generateStmt(dowhileStmt->getStmt());
    
    if(dowhileStmt->hasTemporaryExpr()){
        exprGenerator.generateTemporaryExprs(dowhileStmt->getTemporaryExpr());
    }

    exprGenerator.generateConditionExpr(
        dowhileStmt->getConditionExpr(), 
        startLabel, 
        endLabel
    );

    AsmGenerator::Instruction::genLabel(
        codeGenContext.asmCode, 
        endLabel
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

    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    exprGenerator.generateExpr(assignStmt->getAssignedExpr());
    codeGenContext.freeGpReg();
    AsmGenerator::Instruction::genMov(
        codeGenContext.asmCode, 
        gpRegisters.at(codeGenContext.gpFreeRegPos), 
        exprGenerator.getIdExprAddress(assignStmt->getVariableIdExpr()), 
        "q"
    );
}

// return value ends up in %rax
void StatementCodeGenerator::generateReturnStmt(const IRReturnStmt* returnStmt){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    if(returnStmt->hasReturnValue()){
        // preventing register corruption when function call occurs
        if(returnStmt->hasTemporaryExpr()){
            exprGenerator.generateTemporaryExprs(returnStmt->getTemporaryExpr());
        }

        exprGenerator.generateExpr(returnStmt->getReturnExpr());
        codeGenContext.freeGpReg();
        AsmGenerator::Instruction::genMov(
            codeGenContext.asmCode, 
            gpRegisters.at(codeGenContext.gpFreeRegPos), 
            "%rax", 
            "q"
        );
    } 
    else{
        AsmGenerator::Instruction::genOperation(
            codeGenContext.asmCode, 
            "xor", "%rax", "%rax"
        );
    }
    AsmGenerator::Instruction::genJmp(
        codeGenContext.asmCode, 
        "jmp", 
        std::format("{}_end", codeGenContext.functionName)
    );
}

void StatementCodeGenerator::generateFunctionCallStmt(const IRFunctionCallStmt* callStmt){
    exprGenerator.generateFunctionCallExpr(callStmt->getFunctionCallExpr(), false);
}

void StatementCodeGenerator::generateSwitchStmt(const IRSwitchStmt* switchStmt){
    size_t labNum{ CodeGenerator::getNextLabelNum() };

    std::string startLabel{ std::format("_switch{}", labNum) };
    std::string defaultLabel{ std::format("_switch{}_default", labNum) };
    std::string endLabel{ std::format("_switch{}_end", labNum) };

    auto caseLabel = [labNum](size_t i) -> std::string {
        return std::format("_switch{}_case{}", labNum, i);
    };

    auto& codeGenContext{ FunctionCodeGenerator::getContext() };

    AsmGenerator::Instruction::genLabel(
        codeGenContext.asmCode, 
        startLabel
    );

    size_t size{ switchStmt->getCaseCount() };
    std::string var{ switchStmt->getVariableIdExpr()->getIdName() };
    
    // cases
    for(size_t i{0}; i < size; i++){
        const IRCaseStmt* caseStmt{ switchStmt->getCaseStmtAtN(i) };

        AsmGenerator::Instruction::genLabel(
            codeGenContext.asmCode, 
            caseLabel(i)
        );
        AsmGenerator::Instruction::genMov(
            codeGenContext.asmCode, 
            codeGenContext.variableMap.at(var), 
            "%rcx", 
            "q"
        );
        AsmGenerator::Instruction::genMov(
            codeGenContext.asmCode, 
            exprGenerator.formatLiteral(caseStmt->getLiteralExpr()), 
            "%rdx", 
            "q"
        );
        AsmGenerator::Instruction::genCmp(codeGenContext.asmCode, "%rcx", "%rdx");
        
        std::string jmpLabel{""};
        if(i < size - 1){
            jmpLabel = caseLabel(i + 1);
        }
        else if(switchStmt->hasDefaultStmt()){
            jmpLabel = defaultLabel;
        }
        else{
            jmpLabel = endLabel;
        }
        AsmGenerator::Instruction::genJmp(codeGenContext.asmCode, "jne", jmpLabel);
        
        for(const auto& stmt : caseStmt->getSwitchBlockStmt()->getStmts()){
            generateStmt(stmt.get());
        }

        if(caseStmt->hasBreakStmt()){
            AsmGenerator::Instruction::genJmp(
                codeGenContext.asmCode, 
                "jmp", 
                endLabel
            );
        }
    }
    if(switchStmt->hasDefaultStmt()){
        AsmGenerator::Instruction::genLabel(
            codeGenContext.asmCode, 
            defaultLabel
        );
        const auto& defaultStmt{ switchStmt->getDefaultStmt() };
        for(const auto& stmt : defaultStmt->getSwitchBlockStmt()->getStmts()){
            generateStmt(stmt.get());
        }
    }
    AsmGenerator::Instruction::genLabel(
        codeGenContext.asmCode, 
        endLabel
    );
}