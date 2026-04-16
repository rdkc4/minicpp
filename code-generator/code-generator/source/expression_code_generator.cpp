#include "../expression_code_generator.hpp"

#include <format>

#include "../../asm-generator/asm_instruction_generator.hpp"
#include "../../../common/intermediate-representation-tree/ir_binary_expr.hpp"

ExpressionCodeGenerator::ExpressionCodeGenerator(
    CodeGeneratorFunctionContext& context
) : ctx{ context } {}

void ExpressionCodeGenerator::generateExpr(const IR::node::IRExpr* expr, ExprContext exprCtx){
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
            generateBinaryExpr(static_cast<const IR::node::IRBinaryExpr*>(expr), exprCtx);
    }
}

void ExpressionCodeGenerator::generateBinaryExpr(const IR::node::IRBinaryExpr* binaryExpr, ExprContext exprCtx){
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
            generateRelationalExpr(binaryExpr, operands, exprCtx);
            break;

        default:
            generateALUExpr(binaryExpr, operands);
            break;
    }
    if(exprCtx == ExprContext::VALUE){
        if(ctx.gpFreeRegPos >= gpRegisters.size()){
            AsmGenerator::Instruction::genPush(ctx.asmCode, operands.rightOperand);
        }
        ctx.takeGpReg();
    }
}

std::string ExpressionCodeGenerator::getUnaryOperand(std::string_view fallbackOperand){
    std::string operand{};

    ctx.freeGpReg();
    if(ctx.gpFreeRegPos >= gpRegisters.size()){
        operand = fallbackOperand;
        AsmGenerator::Instruction::genPop(ctx.asmCode, fallbackOperand);
    }
    else{
        operand = gpRegisters.at(ctx.gpFreeRegPos);
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
    IR::defs::IRNodeType nodeType{ binaryExpr->getNodeType() };

    AsmGenerator::Instruction::genOperation(
        ctx.asmCode, 
        "xor", "%rdx", "%rdx"
    );
    AsmGenerator::Instruction::genMov(
        ctx.asmCode, 
        operands.rightOperand, "%rax", "q"
    );

    if(binaryExpr->getType() == Type::INT){
        AsmGenerator::Instruction::genOperation(
            ctx.asmCode, 
            std::format("i{}", IR::defs::irNodeToStr(nodeType)), 
            operands.leftOperand
        );
    }
    else{
        AsmGenerator::Instruction::genOperation(
            ctx.asmCode, 
            IR::defs::irNodeToStr(nodeType), 
            operands.leftOperand
        );
    }

    AsmGenerator::Instruction::genMov(
        ctx.asmCode, 
        "%rax", operands.rightOperand, "q"
    );
}

void ExpressionCodeGenerator::generateShiftExpr(const IR::node::IRBinaryExpr* binaryExpr, BinaryOperands operands){
    IR::defs::IRNodeType nodeType{ binaryExpr->getNodeType() };

    AsmGenerator::Instruction::genMov(
        ctx.asmCode, 
        operands.leftOperand, 
        "%rcx", 
        "q"
    );
    AsmGenerator::Instruction::genOperation(
        ctx.asmCode, 
        IR::defs::irNodeToStr(nodeType), 
        "%cl", 
        operands.rightOperand
    );
}

void ExpressionCodeGenerator::generateLogicalAndExpr(BinaryOperands operands){
    AsmGenerator::Instruction::genTest(
        ctx.asmCode, 
        operands.leftOperand
    );
    AsmGenerator::Instruction::genSet(
        ctx.asmCode, 
        "%al", "ne"
    );

    AsmGenerator::Instruction::genTest(
        ctx.asmCode, 
        operands.rightOperand
    );
    AsmGenerator::Instruction::genSet(
        ctx.asmCode, 
        "%cl", "ne"
    );

    AsmGenerator::Instruction::genOperation(
        ctx.asmCode, 
        "and", "%cl", "%al"
    );
    AsmGenerator::Instruction::genMov(
        ctx.asmCode,
        "%al", operands.rightOperand, "zx"
    );
}

void ExpressionCodeGenerator::generateLogicalOrExpr(BinaryOperands operands){
    AsmGenerator::Instruction::genMov(
        ctx.asmCode, 
        operands.rightOperand, "%rax"
    );

    AsmGenerator::Instruction::genOperation(
        ctx.asmCode, 
        "or", operands.leftOperand, "%rax"
    );

    AsmGenerator::Instruction::genSet(
        ctx.asmCode, 
        "%al", "ne"
    );

    AsmGenerator::Instruction::genMov(
        ctx.asmCode, 
        "%al", operands.rightOperand, "zx"
    );
}

void ExpressionCodeGenerator::generateRelationalExpr(
    const IR::node::IRBinaryExpr* binaryExpr, 
    BinaryOperands operands, 
    ExprContext exprCtx
){
    AsmGenerator::Instruction::genCmp(
        ctx.asmCode, 
        operands.leftOperand, 
        operands.rightOperand
    );

    if(exprCtx == ExprContext::VALUE){
        AsmGenerator::Instruction::genSetcc(
            ctx.asmCode,  
            irNodeTypeToJumpInfo(binaryExpr->getNodeType()).setcc,
            "%al"
        );

        AsmGenerator::Instruction::genMov(
            ctx.asmCode, 
            "%al", operands.rightOperand, "zx"
        );
    }
}

void ExpressionCodeGenerator::generateConditionExpr(
    const IR::node::IRExpr* expr, 
    std::string_view trueLabel, 
    std::string_view falseLabel
){
    size_t labNum{ AsmGenerator::Instruction::getNextLabelNum() };
    
    IR::defs::IRNodeType nodeType{ expr->getNodeType() };
    if(auto jumpInfo{ irNodeTypeToJumpInfo(nodeType) }; !jumpInfo.jcc.empty()){
        generateBinaryExpr(static_cast<const IR::node::IRBinaryExpr*>(expr), ExprContext::BRANCH);

        AsmGenerator::Instruction::genJcc(
            ctx.asmCode,
            jumpInfo.jcc,
            trueLabel
        );

        AsmGenerator::Instruction::genJcc(
            ctx.asmCode,
            jumpInfo.jccInverse,
            falseLabel
        );

        return;
    }

    if(nodeType == IR::defs::IRNodeType::ANDL){
        const auto* binaryExpr{ 
            static_cast<const IR::node::IRBinaryExpr*>(expr) 
        };
        std::string midLabel{ std::format("_andl{}_mid", labNum) };

        generateConditionExpr(binaryExpr->getLeftOperandExpr(), midLabel, falseLabel);

        AsmGenerator::Instruction::genLabel(
            ctx.asmCode,
            midLabel
        );

        generateConditionExpr(binaryExpr->getRightOperandExpr(), trueLabel, falseLabel);
        return;
    }

    if(nodeType == IR::defs::IRNodeType::ORL){
        const auto* binaryExpr{ 
            static_cast<const IR::node::IRBinaryExpr*>(expr) 
        };
        std::string midLabel{ std::format("_orl{}_mid", labNum) };

        generateConditionExpr(binaryExpr->getLeftOperandExpr(), trueLabel, midLabel);

        AsmGenerator::Instruction::genLabel(
            ctx.asmCode,
            midLabel
        );

        generateConditionExpr(binaryExpr->getRightOperandExpr(), trueLabel, falseLabel);
        return;
    }

    generateExpr(expr);
    std::string operand{ getUnaryOperand("%rdi") };

    AsmGenerator::Instruction::genTest(ctx.asmCode, operand);

    AsmGenerator::Instruction::genJcc(
        ctx.asmCode, 
        "jne", trueLabel
    );
    
    AsmGenerator::Instruction::genJcc(
        ctx.asmCode, 
        "je", falseLabel
    );
    
}

void ExpressionCodeGenerator::generateALUExpr(const IR::node::IRBinaryExpr* binaryExpr, BinaryOperands operands){
    IR::defs::IRNodeType nodeType{ binaryExpr->getNodeType() };

    AsmGenerator::Instruction::genOperation(
        ctx.asmCode, 
        IR::defs::irNodeToStr(nodeType), 
        operands.leftOperand, 
        operands.rightOperand
    );
}

void ExpressionCodeGenerator::generateIdExpr(const IR::node::IRIdExpr* idExpr) const {
    if(ctx.gpFreeRegPos < gpRegisters.size()){
        AsmGenerator::Instruction::genMov(
            ctx.asmCode, 
            getIdExprAddress(idExpr), 
            gpRegisters.at(ctx.gpFreeRegPos), 
            "q"
        );
    }
    else{
        AsmGenerator::Instruction::genPush(
            ctx.asmCode, 
            getIdExprAddress(idExpr)
        );
    }
    ctx.takeGpReg();
}

std::string_view ExpressionCodeGenerator::getIdExprAddress(const IR::node::IRIdExpr* idExpr) const {
    return ctx.variableMap.at(idExpr->getIdName());
}

void ExpressionCodeGenerator::generateLiteralExpr(const IR::node::IRLiteralExpr* literalExpr){
    std::string val{ formatLiteral(literalExpr) };

    if(ctx.gpFreeRegPos < gpRegisters.size()){
        AsmGenerator::Instruction::genMov(
            ctx.asmCode, 
            val, 
            gpRegisters.at(ctx.gpFreeRegPos), 
            "q"
        );
    }
    else{
        AsmGenerator::Instruction::genPush(
            ctx.asmCode, 
            val
        );
    }
    ctx.takeGpReg();
}

std::string ExpressionCodeGenerator::formatLiteral(const IR::node::IRLiteralExpr* literalExpr) const {
    std::string val{ literalExpr->getValue() };
    if(literalExpr->getType() == Type::UNSIGNED){
        val.pop_back();
    }
    return std::format("${}", val);
}

void ExpressionCodeGenerator::generateFunctionCallExpr(const IR::node::IRFunctionCallExpr* callExpr, bool expectsReturnVal){
    // push arguments to stack
    generateArguments(callExpr);

    AsmGenerator::Instruction::genCall(ctx.asmCode, callExpr->getCallName());

    // pop arguments from stack
    clearArguments(callExpr->getArgumentCount());

    if(expectsReturnVal){
        if(ctx.gpFreeRegPos < gpRegisters.size()){
            AsmGenerator::Instruction::genMov(
                ctx.asmCode, 
                "%rax", 
                gpRegisters.at(ctx.gpFreeRegPos), 
                "q"
            );
        }
        else{
            AsmGenerator::Instruction::genPush(ctx.asmCode, "%rax");
        }
        ctx.takeGpReg();
    }
}

void ExpressionCodeGenerator::generateArguments(const IR::node::IRFunctionCallExpr* callExpr){
    // evaluating temporaries
    for(const auto& tempExpr : callExpr->getTemporaryExprs()){
        if(tempExpr != nullptr){
            generateTemporaryExprs(tempExpr.get());
        }
    }
    // pushing arguments onto stack
    for(size_t i{callExpr->getArgumentCount()}; i-- > 0; ){
        generateExpr(callExpr->getArgumentAtN(i));
        ctx.freeGpReg();

        if(ctx.gpFreeRegPos < gpRegisters.size()){ // if >= gpRegisters.size() argument is already pushed
            AsmGenerator::Instruction::genPush(
                ctx.asmCode, 
                gpRegisters.at(ctx.gpFreeRegPos)
            );
        }
    }
}

void ExpressionCodeGenerator::clearArguments(size_t argCount){
    // popping arguments of the stack
    AsmGenerator::Instruction::genOperation(
        ctx.asmCode, 
        "add", 
        std::format("${}", argCount * AsmGenerator::Instruction::regSize), 
        "%rsp"
    );
}

void ExpressionCodeGenerator::generateTemporaryExprs(const IR::node::IRTemporaryExpr* tempExprs){
    for(size_t i{0}; i < tempExprs->getTemporaryExprs().size(); ++i){
        const auto* tempExpr{ tempExprs->getTemporaryExprAtN(i) };

        if(tempExpr->getNodeType() == IR::defs::IRNodeType::CALL){
            for(const auto& temp : static_cast<const IR::node::IRFunctionCallExpr*>(tempExpr)->getTemporaryExprs()){
                if(temp != nullptr){
                    generateTemporaryExprs(temp.get());
                }
            }
        }

        ctx.variableMap.insert({
            tempExprs->getTemporaryNameAtN(i), 
            std::format("-{}(%rbp)", ctx.variableNum * AsmGenerator::Instruction::regSize)
        });
        ++ctx.variableNum;

        generateExpr(tempExpr);
        ctx.freeGpReg();
        AsmGenerator::Instruction::genMov(
            ctx.asmCode, 
            gpRegisters.at(ctx.gpFreeRegPos), 
            ctx.variableMap.at(tempExprs->getTemporaryNameAtN(i)), 
            "q"
        );
    }
}