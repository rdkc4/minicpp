#include "../statement_code_generator.hpp"

#include <format>

#include "../../asm-generator/asm_instruction_generator.hpp"

StatementCodeGenerator::StatementCodeGenerator(
    CodeGeneratorFunctionContext& context
) : ctx { context }, exprGenerator { context } {} 

void StatementCodeGenerator::generateStmt(const IR::node::IRStmt* stmt){
    switch(stmt->getNodeType()){
        case IR::defs::IRNodeType::VARIABLE:
            generateVariableDeclStmt(static_cast<const IR::node::IRVariableDeclStmt*>(stmt));
            break;

        case IR::defs::IRNodeType::IF:
            generateIfStmt(static_cast<const IR::node::IRIfStmt*>(stmt));
            break;

        case IR::defs::IRNodeType::COMPOUND:
            generateCompoundStmt(static_cast<const IR::node::IRCompoundStmt*>(stmt));
            break;

        case IR::defs::IRNodeType::ASSIGN:
            generateAssignStmt(static_cast<const IR::node::IRAssignStmt*>(stmt));
            break;

        case IR::defs::IRNodeType::RETURN:
            generateReturnStmt(static_cast<const IR::node::IRReturnStmt*>(stmt));
            break;

        case IR::defs::IRNodeType::WHILE:
            generateWhileStmt(static_cast<const IR::node::IRWhileStmt*>(stmt));
            break;

        case IR::defs::IRNodeType::FOR:
            generateForStmt(static_cast<const IR::node::IRForStmt*>(stmt));
            break;

        case IR::defs::IRNodeType::DO_WHILE:
            generateDoWhileStmt(static_cast<const IR::node::IRDoWhileStmt*>(stmt));
            break;

        case IR::defs::IRNodeType::SWITCH:
            generateSwitchStmt(static_cast<const IR::node::IRSwitchStmt*>(stmt));
            break;

        case IR::defs::IRNodeType::CALL_STMT:
            generateFunctionCallStmt(static_cast<const IR::node::IRFunctionCallStmt*>(stmt));
            return;

        default:
            return;
    }
}

void StatementCodeGenerator::generateVariableDeclStmt(const IR::node::IRVariableDeclStmt* variableDecl){
    // mapping local variable to address relative to %rbp (-n(%rbp))
    // if not successful it means that variable with the given name existed but went out of scope, 
    // so it overwrites it with new memory location
    auto [varPtr, success]{ 
        ctx.variableMap.insert({
            variableDecl->getVarName(), 
            std::format(
                "-{}(%rbp)", 
                ctx.variableNum * AsmGenerator::Instruction::regSize
            )
        }) 
    };
    if(!success){
        varPtr->second = std::format(
            "-{}(%rbp)", 
            ctx.variableNum * AsmGenerator::Instruction::regSize
        );
    }
    ++ctx.variableNum;

    // direct initialization / default value assignation
    if(variableDecl->hasAssignExpr()){
        if(variableDecl->hasTemporaryExpr()){
            exprGenerator.generateTemporaryExprs(variableDecl->getTemporaryExpr());
        }

        exprGenerator.generateExpr(variableDecl->getAssignExpr());
        ctx.freeGpReg();
        AsmGenerator::Instruction::genMov(
            ctx.asmCode, 
            gpRegisters.at(ctx.gpFreeRegPos), 
            ctx.variableMap.at(variableDecl->getVarName()), 
            "q"
        );
    }
    else{
        // default value 
        AsmGenerator::Instruction::genMov(
            ctx.asmCode, 
            "$0", 
            ctx.variableMap.at(variableDecl->getVarName()), 
            "q"
        );
    }
}

void StatementCodeGenerator::generateIfStmt(const IR::node::IRIfStmt* ifStmt){
    size_t labNum{ AsmGenerator::Instruction::getNextLabelNum() };
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

    for(size_t i{0}; i < size; ++i){
        auto [condition, stmt, tempExpr]{ 
            ifStmt->getIfStmtAtN(i) 
        };
        AsmGenerator::Instruction::genLabel(
            ctx.asmCode, 
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
            ctx.asmCode, 
            bodyStartLabel
        );

        generateStmt(stmt);
        AsmGenerator::Instruction::genJmp(
            ctx.asmCode, 
            endLabel
        );
    }

    if(ifStmt->hasElseStmt()){
        AsmGenerator::Instruction::genLabel(
            ctx.asmCode, 
            elseLabel
        );
        generateStmt(ifStmt->getElseStmt());
    }
    AsmGenerator::Instruction::genLabel(
        ctx.asmCode, 
        endLabel
    );
}

void StatementCodeGenerator::generateWhileStmt(const IR::node::IRWhileStmt* whileStmt){
    size_t labNum{ AsmGenerator::Instruction::getNextLabelNum() };

    std::string startLabel{ std::format("_while{}", labNum) };
    std::string bodyLabel{ std::format("_while{}_body", labNum) };
    std::string endLabel{ std::format("_while{}_end", labNum) };

    AsmGenerator::Instruction::genLabel(
        ctx.asmCode, 
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
        ctx.asmCode, 
        bodyLabel
    );
    generateStmt(whileStmt->getStmt());
    AsmGenerator::Instruction::genJmp(
        ctx.asmCode, 
        startLabel
    );

    AsmGenerator::Instruction::genLabel(
        ctx.asmCode, 
        endLabel
    );
}

void StatementCodeGenerator::generateForStmt(const IR::node::IRForStmt* forStmt){
    size_t labNum{ AsmGenerator::Instruction::getNextLabelNum() };

    std::string startLabel{ std::format("_for{}", labNum) };
    std::string bodyLabel{ std::format("_for{}_body", labNum) };
    std::string endLabel{ std::format("_for{}_end", labNum) };

    // initializer
    if(forStmt->hasInitializerStmt()){
        generateAssignStmt(forStmt->getInitializerStmt());
    }
    AsmGenerator::Instruction::genLabel(
        ctx.asmCode, 
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
            ctx.asmCode, 
            bodyLabel
        );
    }

    generateStmt(forStmt->getStmt());

    // increment
    if(forStmt->hasIncrementerStmt()){
        generateAssignStmt(forStmt->getIncrementerStmt());
    }
    AsmGenerator::Instruction::genJmp(
        ctx.asmCode, 
        startLabel
    );

    AsmGenerator::Instruction::genLabel(
        ctx.asmCode, 
        endLabel
    );
}

void StatementCodeGenerator::generateDoWhileStmt(const IR::node::IRDoWhileStmt* dowhileStmt){
    size_t labNum{ AsmGenerator::Instruction::getNextLabelNum() };
    
    std::string startLabel{ std::format("_do_while{}", labNum) };
    std::string endLabel{ std::format("_do_while{}_end", labNum) };

    AsmGenerator::Instruction::genLabel(
        ctx.asmCode, 
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
        ctx.asmCode, 
        endLabel
    );
}

void StatementCodeGenerator::generateCompoundStmt(const IR::node::IRCompoundStmt* compoundStmt){
    for(const auto& stmt : compoundStmt->getStmts()){
        generateStmt(stmt.get());
    }
}

// evaluating rvalue
void StatementCodeGenerator::generateAssignStmt(const IR::node::IRAssignStmt* assignStmt){
    // preventing register corruption when function call occurs
    if(assignStmt->hasTemporaryExpr()){
        exprGenerator.generateTemporaryExprs(assignStmt->getTemporaryExpr());
    }

    exprGenerator.generateExpr(assignStmt->getAssignedExpr());
    ctx.freeGpReg();
    AsmGenerator::Instruction::genMov(
        ctx.asmCode, 
        gpRegisters.at(ctx.gpFreeRegPos), 
        exprGenerator.getIdExprAddress(assignStmt->getVariableIdExpr()), 
        "q"
    );
}

// return value ends up in %rax
void StatementCodeGenerator::generateReturnStmt(const IR::node::IRReturnStmt* returnStmt){
    if(returnStmt->hasReturnValue()){
        // preventing register corruption when function call occurs
        if(returnStmt->hasTemporaryExpr()){
            exprGenerator.generateTemporaryExprs(returnStmt->getTemporaryExpr());
        }

        exprGenerator.generateExpr(returnStmt->getReturnExpr());
        ctx.freeGpReg();
        AsmGenerator::Instruction::genMov(
            ctx.asmCode, 
            gpRegisters.at(ctx.gpFreeRegPos), 
            "%rax", 
            "q"
        );
    } 
    else{
        AsmGenerator::Instruction::genOperation(
            ctx.asmCode, 
            "xor", "%rax", "%rax"
        );
    }
    AsmGenerator::Instruction::genJmp(
        ctx.asmCode, 
        std::format("_{}_end", ctx.functionName)
    );
}

void StatementCodeGenerator::generateFunctionCallStmt(const IR::node::IRFunctionCallStmt* callStmt){
    exprGenerator.generateFunctionCallExpr(callStmt->getFunctionCallExpr(), false);
}

void StatementCodeGenerator::generateSwitchStmt(const IR::node::IRSwitchStmt* switchStmt){
    size_t labNum{ AsmGenerator::Instruction::getNextLabelNum() };

    std::string startLabel{ std::format("_switch{}", labNum) };
    std::string defaultLabel{ std::format("_switch{}_default", labNum) };
    std::string endLabel{ std::format("_switch{}_end", labNum) };

    auto caseLabel = [labNum](size_t i) -> std::string {
        return std::format("_switch{}_case{}", labNum, i);
    };

    AsmGenerator::Instruction::genLabel(
        ctx.asmCode, 
        startLabel
    );

    size_t size{ switchStmt->getCaseCount() };
    std::string var{ switchStmt->getVariableIdExpr()->getIdName() };
    
    // cases
    for(size_t i{0}; i < size; i++){
        const IR::node::IRCaseStmt* caseStmt{ switchStmt->getCaseStmtAtN(i) };

        AsmGenerator::Instruction::genLabel(
            ctx.asmCode, 
            caseLabel(i)
        );
        AsmGenerator::Instruction::genMov(
            ctx.asmCode, 
            ctx.variableMap.at(var), 
            "%rcx", 
            "q"
        );
        AsmGenerator::Instruction::genMov(
            ctx.asmCode, 
            exprGenerator.formatLiteral(caseStmt->getLiteralExpr()), 
            "%rdx", 
            "q"
        );
        AsmGenerator::Instruction::genCmp(ctx.asmCode, "%rcx", "%rdx");
        
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
        AsmGenerator::Instruction::genJcc(
            ctx.asmCode, 
            "jne", jmpLabel
        );
        
        for(const auto& stmt : caseStmt->getSwitchBlockStmt()->getStmts()){
            generateStmt(stmt.get());
        }

        if(caseStmt->hasBreakStmt()){
            AsmGenerator::Instruction::genJmp(
                ctx.asmCode, 
                endLabel
            );
        }
    }
    if(switchStmt->hasDefaultStmt()){
        AsmGenerator::Instruction::genLabel(
            ctx.asmCode, 
            defaultLabel
        );
        const auto& defaultStmt{ switchStmt->getDefaultStmt() };
        for(const auto& stmt : defaultStmt->getSwitchBlockStmt()->getStmts()){
            generateStmt(stmt.get());
        }
    }
    AsmGenerator::Instruction::genLabel(
        ctx.asmCode, 
        endLabel
    );
}