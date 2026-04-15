#include "../expression_code_generator.hpp"
#include "../function_code_generator.hpp"

#include <format>

#include "../../asm-generator/asm_instruction_generator.hpp"
#include "../../../common/intermediate-representation-tree/ir_binary_expr.hpp"
#include "../code_generator.hpp"

void ExpressionCodeGenerator::generateExpr(const IR::node::IRExpr* expr, ExprContext ctx){
    IR::defs::IRNodeType nodeType{ expr->getNodeType() };

    switch(nodeType){
        case IR::defs::IRNodeType::ID:
            generateIdExpr(static_cast<const IR::node::IRIdExpr*>(expr));
            break;
        case IR::defs::IRNodeType::LITERAL:
            generateLiteralExpr(static_cast<const IR::node::IRLiteralExpr*>(expr));
            break;
        case IR::defs::IRNodeType::CALL:
            generateFunctionCallExpr(static_cast<const IR::node::IRFunctionCallExpr*>(expr));
            break;
        default:
            generateBinaryExpr(static_cast<const IR::node::IRBinaryExpr*>(expr), ctx);
    }
}

void ExpressionCodeGenerator::generateBinaryExpr(const IR::node::IRBinaryExpr* binaryExpr, ExprContext ctx){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };

    generateExpr(binaryExpr->getLeftOperandExpr());
    generateExpr(binaryExpr->getRightOperandExpr());

    BinaryOperands operands{ getBinaryOperands() };
    IR::defs::IRNodeType nodeType{ binaryExpr->getNodeType() };

    switch(nodeType){
        // result of MUL || DIV is in RDX:RAX
        case IR::defs::IRNodeType::MUL:
        case IR::defs::IRNodeType::DIV:
            generateMultiplicativeExpr(binaryExpr, operands);
            break;

        case IR::defs::IRNodeType::SHL:
        case IR::defs::IRNodeType::SAL:
        case IR::defs::IRNodeType::SHR:
        case IR::defs::IRNodeType::SAR:
            generateShiftExpr(binaryExpr, operands);
            break;

        case IR::defs::IRNodeType::ANDL:
            generateLogicalAndExpr(operands);
            break;

        case IR::defs::IRNodeType::ORL:
            generateLogicalOrExpr(operands);
            break;
        
        case IR::defs::IRNodeType::JA:
        case IR::defs::IRNodeType::JG:
        case IR::defs::IRNodeType::JAE:
        case IR::defs::IRNodeType::JGE:
        case IR::defs::IRNodeType::JB:
        case IR::defs::IRNodeType::JL:
        case IR::defs::IRNodeType::JBE:
        case IR::defs::IRNodeType::JLE:
        case IR::defs::IRNodeType::JE:
        case IR::defs::IRNodeType::JNE:
            generateRelationalExpr(binaryExpr, operands, ctx);
            break;

        default:
            generateALUExpr(binaryExpr, operands);
            break;
    }
    if(ctx == ExprContext::VALUE){
        if(codeGenContext.gpFreeRegPos >= gpRegisters.size()){
            AsmGenerator::Instruction::genPush(codeGenContext.asmCode, operands.rightOperand);
        }
        codeGenContext.takeGpReg();
    }
}

std::string ExpressionCodeGenerator::getUnaryOperand(std::string_view fallbackOperand){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    std::string operand{};

    codeGenContext.freeGpReg();
    if(codeGenContext.gpFreeRegPos >= gpRegisters.size()){
        operand = fallbackOperand;
        AsmGenerator::Instruction::genPop(codeGenContext.asmCode, fallbackOperand);
    }
    else{
        operand = gpRegisters.at(codeGenContext.gpFreeRegPos);
    }

    return operand;
}

BinaryOperands ExpressionCodeGenerator::getBinaryOperands(){
    return {
        .leftOperand = getUnaryOperand("%rdi"),
        .rightOperand = getUnaryOperand("%rsi")
    };
}

void ExpressionCodeGenerator::generateMultiplicativeExpr(const IR::node::IRBinaryExpr* binaryExpr, BinaryOperands operands){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    IR::defs::IRNodeType nodeType{ binaryExpr->getNodeType() };

    AsmGenerator::Instruction::genOperation(
        codeGenContext.asmCode, 
        "xor", "%rdx", "%rdx"
    );
    AsmGenerator::Instruction::genMov(
        codeGenContext.asmCode, 
        operands.rightOperand, "%rax", "q"
    );

    if(binaryExpr->getType() == Type::INT){
        AsmGenerator::Instruction::genOperation(
            codeGenContext.asmCode, 
            std::format("i{}", IR::defs::irNodeToStr(nodeType)), 
            operands.leftOperand
        );
    }
    else{
        AsmGenerator::Instruction::genOperation(
            codeGenContext.asmCode, 
            IR::defs::irNodeToStr(nodeType), 
            operands.leftOperand
        );
    }

    AsmGenerator::Instruction::genMov(
        codeGenContext.asmCode, 
        "%rax", operands.rightOperand, "q"
    );
}

void ExpressionCodeGenerator::generateShiftExpr(const IR::node::IRBinaryExpr* binaryExpr, BinaryOperands operands){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    IR::defs::IRNodeType nodeType{ binaryExpr->getNodeType() };

    AsmGenerator::Instruction::genMov(
        codeGenContext.asmCode, 
        operands.leftOperand, 
        "%rcx", 
        "q"
    );
    AsmGenerator::Instruction::genOperation(
        codeGenContext.asmCode, 
        IR::defs::irNodeToStr(nodeType), 
        "%cl", 
        operands.rightOperand
    );
}

void ExpressionCodeGenerator::generateLogicalAndExpr(BinaryOperands operands){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };

    AsmGenerator::Instruction::genTest(
        codeGenContext.asmCode, 
        operands.leftOperand
    );
    AsmGenerator::Instruction::genSet(
        codeGenContext.asmCode, 
        "%al", "ne"
    );

    AsmGenerator::Instruction::genTest(
        codeGenContext.asmCode, 
        operands.rightOperand
    );
    AsmGenerator::Instruction::genSet(
        codeGenContext.asmCode, 
        "%cl", "ne"
    );

    AsmGenerator::Instruction::genOperation(
        codeGenContext.asmCode, 
        "and", "%cl", "%al"
    );
    AsmGenerator::Instruction::genMov(
        codeGenContext.asmCode,
        "%al", operands.rightOperand, "zx"
    );
}

void ExpressionCodeGenerator::generateLogicalOrExpr(BinaryOperands operands){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };

    AsmGenerator::Instruction::genMov(
        codeGenContext.asmCode, 
        operands.rightOperand, "%rax"
    );

    AsmGenerator::Instruction::genOperation(
        codeGenContext.asmCode, 
        "or", operands.leftOperand, "%rax"
    );

    AsmGenerator::Instruction::genSet(
        codeGenContext.asmCode, 
        "%al", "ne"
    );

    AsmGenerator::Instruction::genMov(
        codeGenContext.asmCode, 
        "%al", operands.rightOperand, "zx"
    );
}

void ExpressionCodeGenerator::generateRelationalExpr(const IR::node::IRBinaryExpr* binaryExpr, BinaryOperands operands, ExprContext ctx){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    AsmGenerator::Instruction::genCmp(
        codeGenContext.asmCode, 
        operands.leftOperand, 
        operands.rightOperand
    );

    if(ctx == ExprContext::VALUE){
        AsmGenerator::Instruction::genSet(
            codeGenContext.asmCode, 
            "%al", 
            nodeToSetExt.at(binaryExpr->getNodeType())
        );

        AsmGenerator::Instruction::genMov(
            codeGenContext.asmCode, 
            "%al", operands.rightOperand, "zx"
        );
    }
}

void ExpressionCodeGenerator::generateConditionExpr(const IR::node::IRExpr* expr, std::string_view trueLabel, std::string_view falseLabel){
    assert(trueLabel != "" || falseLabel != "");
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    size_t labNum{ CodeGenerator::getNextLabelNum() };
    
    IR::defs::IRNodeType nodeType{ expr->getNodeType() };
    if(nodeToJMP.find(nodeType) != nodeToJMP.end()){
        generateBinaryExpr(static_cast<const IR::node::IRBinaryExpr*>(expr), ExprContext::BRANCH);

        if(trueLabel != ""){
            AsmGenerator::Instruction::genJmp(
                codeGenContext.asmCode,
                nodeToJMP.at(nodeType),
                trueLabel
            );
        }

        if(falseLabel != ""){
            AsmGenerator::Instruction::genJmp(
                codeGenContext.asmCode,
                nodeToOppJMP.at(nodeType),
                falseLabel
            );
        }

        return;
    }

    if(nodeType == IR::defs::IRNodeType::ANDL){
        const auto* binaryExpr{ static_cast<const IR::node::IRBinaryExpr*>(expr) };
        std::string midLabel{ std::format("_andl{}_mid", labNum) };

        generateConditionExpr(binaryExpr->getLeftOperandExpr(), midLabel, falseLabel);

        AsmGenerator::Instruction::genLabel(
            codeGenContext.asmCode,
            midLabel
        );

        generateConditionExpr(binaryExpr->getRightOperandExpr(), trueLabel, falseLabel);
        return;
    }

    if(nodeType == IR::defs::IRNodeType::ORL){
        const auto* binaryExpr{ static_cast<const IR::node::IRBinaryExpr*>(expr) };
        std::string midLabel{ std::format("_orl{}_mid", labNum) };

        generateConditionExpr(binaryExpr->getLeftOperandExpr(), trueLabel, midLabel);

        AsmGenerator::Instruction::genLabel(
            codeGenContext.asmCode,
            midLabel
        );

        generateConditionExpr(binaryExpr->getRightOperandExpr(), trueLabel, falseLabel);
        return;
    }

    generateExpr(expr);
    std::string operand{ getUnaryOperand("%rdi") };

    AsmGenerator::Instruction::genTest(codeGenContext.asmCode, operand);

    if(trueLabel != ""){
        AsmGenerator::Instruction::genJmp(
            codeGenContext.asmCode, 
            "jne", trueLabel
        );
    }

    if(falseLabel != ""){
        AsmGenerator::Instruction::genJmp(
            codeGenContext.asmCode, 
            "je", falseLabel
        );
    }
}

void ExpressionCodeGenerator::generateALUExpr(const IR::node::IRBinaryExpr* binaryExpr, BinaryOperands operands){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    IR::defs::IRNodeType nodeType{ binaryExpr->getNodeType() };

    AsmGenerator::Instruction::genOperation(
        codeGenContext.asmCode, 
        IR::defs::irNodeToStr(nodeType), 
        operands.leftOperand, 
        operands.rightOperand
    );
}

void ExpressionCodeGenerator::generateIdExpr(const IR::node::IRIdExpr* idExpr) const {
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    if(codeGenContext.gpFreeRegPos < gpRegisters.size()){
        AsmGenerator::Instruction::genMov(
            codeGenContext.asmCode, 
            getIdExprAddress(idExpr), 
            gpRegisters.at(codeGenContext.gpFreeRegPos), 
            "q"
        );
    }
    else{
        AsmGenerator::Instruction::genPush(
            codeGenContext.asmCode, 
            getIdExprAddress(idExpr)
        );
    }
    codeGenContext.takeGpReg();
}

std::string_view ExpressionCodeGenerator::getIdExprAddress(const IR::node::IRIdExpr* idExpr) const {
    return FunctionCodeGenerator::getContext().variableMap.at(idExpr->getIdName());
}

void ExpressionCodeGenerator::generateLiteralExpr(const IR::node::IRLiteralExpr* literalExpr){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    std::string val{ formatLiteral(literalExpr) };

    if(codeGenContext.gpFreeRegPos < gpRegisters.size()){
        AsmGenerator::Instruction::genMov(
            codeGenContext.asmCode, 
            val, 
            gpRegisters.at(codeGenContext.gpFreeRegPos), 
            "q"
        );
    }
    else{
        AsmGenerator::Instruction::genPush(
            codeGenContext.asmCode, 
            val
        );
    }
    codeGenContext.takeGpReg();
}

std::string ExpressionCodeGenerator::formatLiteral(const IR::node::IRLiteralExpr* literalExpr) const {
    std::string val{ literalExpr->getValue() };
    if(literalExpr->getType() == Type::UNSIGNED){
        val.pop_back();
    }
    return std::format("${}", val);
}

void ExpressionCodeGenerator::generateFunctionCallExpr(const IR::node::IRFunctionCallExpr* callExpr, bool expectsReturnVal){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    // push arguments to stack
    generateArguments(callExpr);

    AsmGenerator::Instruction::genCall(codeGenContext.asmCode, callExpr->getCallName());

    // pop arguments from stack
    clearArguments(callExpr->getArgumentCount());

    if(expectsReturnVal){
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
}

void ExpressionCodeGenerator::generateArguments(const IR::node::IRFunctionCallExpr* callExpr){
    auto& codeGenContext{ FunctionCodeGenerator::getContext() };
    // evaluating temporaries
    for(const auto& tempExpr : callExpr->getTemporaryExprs()){
        if(tempExpr != nullptr){
            generateTemporaryExprs(tempExpr.get());
        }
    }
    // pushing arguments onto stack
    for(size_t i{callExpr->getArgumentCount()}; i-- > 0; ){
        generateExpr(callExpr->getArgumentAtN(i));
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

void ExpressionCodeGenerator::generateTemporaryExprs(const IR::node::IRTemporaryExpr* tempExprs){
    for(size_t i{0}; i < tempExprs->getTemporaryExprs().size(); ++i){
        auto& codeGenContext{ FunctionCodeGenerator::getContext() };
        const auto* tempExpr{ tempExprs->getTemporaryExprAtN(i) };

        if(tempExpr->getNodeType() == IR::defs::IRNodeType::CALL){
            for(const auto& temp : static_cast<const IR::node::IRFunctionCallExpr*>(tempExpr)->getTemporaryExprs()){
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

        generateExpr(tempExpr);
        codeGenContext.freeGpReg();
        AsmGenerator::Instruction::genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), 
            codeGenContext.variableMap.at(tempExprs->getTemporaryNameAtN(i)), "q"
        );
    }
}