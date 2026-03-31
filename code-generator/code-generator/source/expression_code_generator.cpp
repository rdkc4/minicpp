#include "../expression_code_generator.hpp"
#include "../function_code_generator.hpp"

#include <format>

#include "../../asm-generator/asm_instruction_generator.hpp"
#include "../../../common/intermediate-representation-tree/ir_binary_expr.hpp"

// evaluating equations using general-purpose registers r(8-15) and stack (if necessary)
void ExpressionCodeGenerator::generateNumericalExpr(const IRExpr* numericalExpr){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    if(numericalExpr->getNodeType() == IRNodeType::ID){
        if(codeGenContext.gpFreeRegPos < gpRegisters.size()){
            AsmGenerator::Instruction::genMov(
                codeGenContext.asmCode, 
                generateIDExpr(static_cast<const IRIdExpr*>(numericalExpr)), 
                gpRegisters.at(codeGenContext.gpFreeRegPos), 
                "q"
            );
        }
        else{
            AsmGenerator::Instruction::genPush(
                codeGenContext.asmCode, 
                generateIDExpr(static_cast<const IRIdExpr*>(numericalExpr))
            );
        }
        codeGenContext.takeGpReg();
    }
    else if(numericalExpr->getNodeType() == IRNodeType::LITERAL){
        if(codeGenContext.gpFreeRegPos < gpRegisters.size()){
            AsmGenerator::Instruction::genMov(
                codeGenContext.asmCode, 
                generateLiteralExpr(static_cast<const IRLiteralExpr*>(numericalExpr)), 
                gpRegisters.at(codeGenContext.gpFreeRegPos), 
                "q"
            );
        }
        else{
            AsmGenerator::Instruction::genPush(
                codeGenContext.asmCode, 
                generateLiteralExpr(static_cast<const IRLiteralExpr*>(numericalExpr))
            );
        }
        codeGenContext.takeGpReg();
    }
    else if (numericalExpr->getNodeType() == IRNodeType::CALL){
        generateFunctionCallExpr(static_cast<const IRFunctionCallExpr*>(numericalExpr));
        if(codeGenContext.gpFreeRegPos < gpRegisters.size()){
            AsmGenerator::Instruction::genMov(
                codeGenContext.asmCode, 
                "%rax", 
                gpRegisters.at(codeGenContext.gpFreeRegPos), 
                "q"
            );
        }
        else{
            AsmGenerator::Instruction::genPush(codeGenContext.asmCode, "%rax");
        }
        codeGenContext.takeGpReg();
    }
    else{
        generateBinaryExpr(static_cast<const IRBinaryExpr*>(numericalExpr));
    }
}

void ExpressionCodeGenerator::generateBinaryExpr(const IRBinaryExpr* binaryExpr){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    std::string lreg{};
    std::string rreg{};
    
    generateNumericalExpr(binaryExpr->getLeftOperandExpr());
    generateNumericalExpr(binaryExpr->getRightOperandExpr());
    
    codeGenContext.freeGpReg();
    if(codeGenContext.gpFreeRegPos >= gpRegisters.size()){
        lreg = "%rdi";
        AsmGenerator::Instruction::genPop(codeGenContext.asmCode, lreg);
    }
    else{
        lreg = gpRegisters.at(codeGenContext.gpFreeRegPos);
    }
    codeGenContext.freeGpReg();
    if(codeGenContext.gpFreeRegPos >= gpRegisters.size()){
        rreg = "%rsi";
        AsmGenerator::Instruction::genPop(codeGenContext.asmCode, rreg);
    }
    else{
        rreg = gpRegisters.at(codeGenContext.gpFreeRegPos);
    }
    
    IRNodeType nodeType{ binaryExpr->getNodeType() };

    switch(nodeType){
        // result of MUL || DIV is in RDX:RAX
        case IRNodeType::MUL:
        case IRNodeType::DIV:
            AsmGenerator::Instruction::genOperation(
                codeGenContext.asmCode, 
                "xor", "%rdx", "%rdx"
            );
            AsmGenerator::Instruction::genMov(
                codeGenContext.asmCode, 
                rreg, "%rax", "q"
            );
            if(binaryExpr->getType() == Type::INT){
                AsmGenerator::Instruction::genOperation(
                    codeGenContext.asmCode, 
                    std::format("i{}", 
                        irNodeToString.at(nodeType)), 
                        lreg
                );
            }
            else{
                AsmGenerator::Instruction::genOperation(
                    codeGenContext.asmCode, 
                    irNodeToString.at(nodeType), 
                    lreg
                );
            }
            AsmGenerator::Instruction::genMov(
                codeGenContext.asmCode, 
                "%rax", rreg, "q"
            );
            break;

        case IRNodeType::AND:
        case IRNodeType::OR:
        case IRNodeType::XOR:
            AsmGenerator::Instruction::genOperation(
                codeGenContext.asmCode, 
                irNodeToString.at(nodeType), 
                lreg, 
                rreg
            );
            break;

        case IRNodeType::SHL:
        case IRNodeType::SAL:
        case IRNodeType::SHR:
        case IRNodeType::SAR:
            AsmGenerator::Instruction::genMov(
                codeGenContext.asmCode, 
                lreg, 
                "%rcx", 
                "q"
            );
            AsmGenerator::Instruction::genOperation(
                codeGenContext.asmCode, 
                irNodeToString.at(nodeType), 
                "%cl", 
                rreg
            );
            break;

        default:
            AsmGenerator::Instruction::genOperation(
                codeGenContext.asmCode, 
                irNodeToString.at(nodeType), 
                lreg, 
                rreg
            );
            break;
    }
    if(codeGenContext.gpFreeRegPos >= gpRegisters.size()){
        AsmGenerator::Instruction::genPush(codeGenContext.asmCode, rreg);
    }
    codeGenContext.takeGpReg();
}

void ExpressionCodeGenerator::generateRelationalExpr(const IRExpr* relationalExpr){
    const auto* binaryExpr{ static_cast<const IRBinaryExpr*>(relationalExpr) };
    generateNumericalExpr(binaryExpr->getLeftOperandExpr());
    generateNumericalExpr(binaryExpr->getRightOperandExpr());
    
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    codeGenContext.freeGpReg();
    std::string lreg{ gpRegisters.at(codeGenContext.gpFreeRegPos) };
    codeGenContext.freeGpReg();
    std::string rreg{ gpRegisters.at(codeGenContext.gpFreeRegPos) };
    AsmGenerator::Instruction::genCmp(codeGenContext.asmCode, lreg, rreg);
}

const std::string& ExpressionCodeGenerator::generateIDExpr(const IRIdExpr* idExpr) const {
    // returns address from variable map
    return FunctionCodeGenerator::getContext().variableMap.at(idExpr->getIdName());
}

std::string ExpressionCodeGenerator::generateLiteralExpr(const IRLiteralExpr* literalExpr) const {
    std::string val{ literalExpr->getValue() };
    if(literalExpr->getType() == Type::UNSIGNED){
        val.pop_back();
    }
    return std::format("${}", val);
}

void ExpressionCodeGenerator::generateFunctionCallExpr(const IRFunctionCallExpr* callExpr){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    // push arguments to stack
    generateArguments(callExpr);

    AsmGenerator::Instruction::genCall(codeGenContext.asmCode, callExpr->getCallName());

    // pop arguments from stack
    clearArguments(callExpr->getArgumentCount());
}

void ExpressionCodeGenerator::generateArguments(const IRFunctionCallExpr* callExpr){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    // evaluating temporaries
    for(const auto& tempExpr : callExpr->getTemporaryExprs()){
        if(tempExpr != nullptr){
            generateTemporaryExprs(tempExpr.get());
        }
    }
    // pushing arguments onto stack
    for(size_t i{callExpr->getArgumentCount()}; i-- > 0; ){
        generateNumericalExpr(callExpr->getArgumentAtN(i));
        codeGenContext.freeGpReg();

        if(codeGenContext.gpFreeRegPos < gpRegisters.size()){ // if >= gpRegisters.size() argument is already pushed
            AsmGenerator::Instruction::genPush(
                codeGenContext.asmCode, 
                gpRegisters.at(codeGenContext.gpFreeRegPos)
            );
        }
    }
}

void ExpressionCodeGenerator::clearArguments(size_t argCount){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    // popping arguments of the stack
    AsmGenerator::Instruction::genOperation(
        codeGenContext.asmCode, 
        "add", 
        std::format("${}", argCount * AsmGenerator::Instruction::regSize), 
        "%rsp"
    );
}

void ExpressionCodeGenerator::generateTemporaryExprs(const IRTemporaryExpr* tempExprs){
    for(size_t i{0}; i < tempExprs->getTemporaryExprs().size(); ++i){
        auto& codeGenContext{ FunctionCodeGenerator::getContext() };
        const auto* tempExpr{ tempExprs->getTemporaryExprAtN(i) };

        if(tempExpr->getNodeType() == IRNodeType::CALL){
            for(const auto& temp : static_cast<const IRFunctionCallExpr*>(tempExpr)->getTemporaryExprs()){
                if(temp != nullptr){
                    generateTemporaryExprs(temp.get());
                }
            }
        }

        codeGenContext.variableMap.insert({
            tempExprs->getTemporaryNameAtN(i), 
            std::format("-{}(%rbp)", codeGenContext.variableNum * AsmGenerator::Instruction::regSize)
        });
        ++codeGenContext.variableNum;

        generateNumericalExpr(tempExpr);
        codeGenContext.freeGpReg();
        AsmGenerator::Instruction::genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), 
            codeGenContext.variableMap.at(tempExprs->getTemporaryNameAtN(i)), "q"
        );
    }
}