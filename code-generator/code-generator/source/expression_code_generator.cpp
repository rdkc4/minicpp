#include "../expression_code_generator.hpp"

#include <format>

#include "../../asm-generator/asm_instruction_generator.hpp"
#include "../../../common/intermediate-representation-tree/ir_binary_expr.hpp"

ExpressionCodeGenerator::ExpressionCodeGenerator(
    CodeGeneratorFunctionContext& context
) : ctx{ context } {}

void ExpressionCodeGenerator::generateExpr(const ir::IRExpr* expr, ExprContext exprCtx){
    ir::IRNodeType nodeType{ expr->getNodeType() };

    switch(nodeType){
        case ir::IRNodeType::ID:
            generateIdExpr(static_cast<const ir::IRIdExpr*>(expr));
            break;

        case ir::IRNodeType::LITERAL:
            generateLiteralExpr(static_cast<const ir::IRLiteralExpr*>(expr));
            break;

        case ir::IRNodeType::CALL:
            generateFunctionCallExpr(static_cast<const ir::IRFunctionCallExpr*>(expr));
            break;

        default:
            generateBinaryExpr(static_cast<const ir::IRBinaryExpr*>(expr), exprCtx);
    }
}

void ExpressionCodeGenerator::generateBinaryExpr(const ir::IRBinaryExpr* binaryExpr, ExprContext exprCtx){
    generateExpr(binaryExpr->getLeftOperandExpr());
    generateExpr(binaryExpr->getRightOperandExpr());

    BinaryOperands operands{ getBinaryOperands() };
    ir::IRNodeType nodeType{ binaryExpr->getNodeType() };

    switch(nodeType){
        // result of MUL || DIV is in RDX:RAX
        case ir::IRNodeType::MUL:
        case ir::IRNodeType::DIV:
            generateMultiplicativeExpr(binaryExpr, operands);
            break;

        case ir::IRNodeType::SHL:
        case ir::IRNodeType::SAL:
        case ir::IRNodeType::SHR:
        case ir::IRNodeType::SAR:
            generateShiftExpr(binaryExpr, operands);
            break;

        case ir::IRNodeType::ANDL:
            generateLogicalAndExpr(operands);
            break;

        case ir::IRNodeType::ORL:
            generateLogicalOrExpr(operands);
            break;
        
        case ir::IRNodeType::JA:
        case ir::IRNodeType::JG:
        case ir::IRNodeType::JAE:
        case ir::IRNodeType::JGE:
        case ir::IRNodeType::JB:
        case ir::IRNodeType::JL:
        case ir::IRNodeType::JBE:
        case ir::IRNodeType::JLE:
        case ir::IRNodeType::JE:
        case ir::IRNodeType::JNE:
            generateRelationalExpr(binaryExpr, operands, exprCtx);
            break;

        default:
            generateALUExpr(binaryExpr, operands);
            break;
    }
    if(exprCtx == ExprContext::VALUE){
        if(ctx.gpFreeRegPos >= gpRegisters.size()){
            assembly::genPush(ctx.asmCode, operands.rightOperand);
        }
        ctx.takeGpReg();
    }
}

std::string ExpressionCodeGenerator::getUnaryOperand(std::string_view fallbackOperand){
    std::string operand{};

    ctx.freeGpReg();
    if(ctx.gpFreeRegPos >= gpRegisters.size()){
        operand = fallbackOperand;
        assembly::genPop(ctx.asmCode, fallbackOperand);
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

void ExpressionCodeGenerator::generateMultiplicativeExpr(const ir::IRBinaryExpr* binaryExpr, BinaryOperands operands){
    ir::IRNodeType nodeType{ binaryExpr->getNodeType() };

    assembly::genOperation(
        ctx.asmCode, 
        "xor", "%rdx", "%rdx"
    );
    assembly::genMov(
        ctx.asmCode, 
        operands.rightOperand, "%rax", "q"
    );

    if(binaryExpr->getType() == Type::INT){
        assembly::genOperation(
            ctx.asmCode, 
            std::format("i{}", ir::irNodeToStr(nodeType)), 
            operands.leftOperand
        );
    }
    else{
        assembly::genOperation(
            ctx.asmCode, 
            ir::irNodeToStr(nodeType), 
            operands.leftOperand
        );
    }

    assembly::genMov(
        ctx.asmCode, 
        "%rax", operands.rightOperand, "q"
    );
}

void ExpressionCodeGenerator::generateShiftExpr(const ir::IRBinaryExpr* binaryExpr, BinaryOperands operands){
    ir::IRNodeType nodeType{ binaryExpr->getNodeType() };

    assembly::genMov(
        ctx.asmCode, 
        operands.leftOperand, 
        "%rcx", 
        "q"
    );
    assembly::genOperation(
        ctx.asmCode, 
        ir::irNodeToStr(nodeType), 
        "%cl", 
        operands.rightOperand
    );
}

void ExpressionCodeGenerator::generateLogicalAndExpr(BinaryOperands operands){
    assembly::genTest(
        ctx.asmCode, 
        operands.leftOperand
    );
    assembly::genSet(
        ctx.asmCode, 
        "%al", "ne"
    );

    assembly::genTest(
        ctx.asmCode, 
        operands.rightOperand
    );
    assembly::genSet(
        ctx.asmCode, 
        "%cl", "ne"
    );

    assembly::genOperation(
        ctx.asmCode, 
        "and", "%cl", "%al"
    );
    assembly::genMov(
        ctx.asmCode,
        "%al", operands.rightOperand, "zx"
    );
}

void ExpressionCodeGenerator::generateLogicalOrExpr(BinaryOperands operands){
    assembly::genMov(
        ctx.asmCode, 
        operands.rightOperand, "%rax"
    );

    assembly::genOperation(
        ctx.asmCode, 
        "or", operands.leftOperand, "%rax"
    );

    assembly::genSet(
        ctx.asmCode, 
        "%al", "ne"
    );

    assembly::genMov(
        ctx.asmCode, 
        "%al", operands.rightOperand, "zx"
    );
}

void ExpressionCodeGenerator::generateRelationalExpr(
    const ir::IRBinaryExpr* binaryExpr, 
    BinaryOperands operands, 
    ExprContext exprCtx
){
    assembly::genCmp(
        ctx.asmCode, 
        operands.leftOperand, 
        operands.rightOperand
    );

    if(exprCtx == ExprContext::VALUE){
        assembly::genSetcc(
            ctx.asmCode,  
            irNodeTypeToJumpInfo(binaryExpr->getNodeType()).setcc,
            "%al"
        );

        assembly::genMov(
            ctx.asmCode, 
            "%al", operands.rightOperand, "zx"
        );
    }
}

void ExpressionCodeGenerator::generateConditionExpr(
    const ir::IRExpr* expr, 
    std::string_view trueLabel, 
    std::string_view falseLabel
){
    size_t labNum{ assembly::getNextLabelNum() };
    
    ir::IRNodeType nodeType{ expr->getNodeType() };
    if(auto jumpInfo{ irNodeTypeToJumpInfo(nodeType) }; !jumpInfo.jcc.empty()){
        generateBinaryExpr(static_cast<const ir::IRBinaryExpr*>(expr), ExprContext::BRANCH);

        assembly::genJcc(
            ctx.asmCode,
            jumpInfo.jcc,
            trueLabel
        );

        assembly::genJcc(
            ctx.asmCode,
            jumpInfo.jccInverse,
            falseLabel
        );

        return;
    }

    if(nodeType == ir::IRNodeType::ANDL){
        const auto* binaryExpr{ 
            static_cast<const ir::IRBinaryExpr*>(expr) 
        };
        std::string midLabel{ std::format("_andl{}_mid", labNum) };

        generateConditionExpr(binaryExpr->getLeftOperandExpr(), midLabel, falseLabel);

        assembly::genLabel(
            ctx.asmCode,
            midLabel
        );

        generateConditionExpr(binaryExpr->getRightOperandExpr(), trueLabel, falseLabel);
        return;
    }

    if(nodeType == ir::IRNodeType::ORL){
        const auto* binaryExpr{ 
            static_cast<const ir::IRBinaryExpr*>(expr) 
        };
        std::string midLabel{ std::format("_orl{}_mid", labNum) };

        generateConditionExpr(binaryExpr->getLeftOperandExpr(), trueLabel, midLabel);

        assembly::genLabel(
            ctx.asmCode,
            midLabel
        );

        generateConditionExpr(binaryExpr->getRightOperandExpr(), trueLabel, falseLabel);
        return;
    }

    generateExpr(expr);
    std::string operand{ getUnaryOperand("%rdi") };

    assembly::genTest(ctx.asmCode, operand);

    assembly::genJcc(
        ctx.asmCode, 
        "jne", trueLabel
    );
    
    assembly::genJcc(
        ctx.asmCode, 
        "je", falseLabel
    );
    
}

void ExpressionCodeGenerator::generateALUExpr(const ir::IRBinaryExpr* binaryExpr, BinaryOperands operands){
    ir::IRNodeType nodeType{ binaryExpr->getNodeType() };

    assembly::genOperation(
        ctx.asmCode, 
        ir::irNodeToStr(nodeType), 
        operands.leftOperand, 
        operands.rightOperand
    );
}

void ExpressionCodeGenerator::generateIdExpr(const ir::IRIdExpr* idExpr) const {
    if(ctx.gpFreeRegPos < gpRegisters.size()){
        assembly::genMov(
            ctx.asmCode, 
            getIdExprAddress(idExpr), 
            gpRegisters.at(ctx.gpFreeRegPos), 
            "q"
        );
    }
    else{
        assembly::genPush(
            ctx.asmCode, 
            getIdExprAddress(idExpr)
        );
    }
    ctx.takeGpReg();
}

std::string_view ExpressionCodeGenerator::getIdExprAddress(const ir::IRIdExpr* idExpr) const {
    return ctx.variableMap.at(idExpr->getIdName());
}

void ExpressionCodeGenerator::generateLiteralExpr(const ir::IRLiteralExpr* literalExpr){
    std::string val{ formatLiteral(literalExpr) };

    if(ctx.gpFreeRegPos < gpRegisters.size()){
        assembly::genMov(
            ctx.asmCode, 
            val, 
            gpRegisters.at(ctx.gpFreeRegPos), 
            "q"
        );
    }
    else{
        assembly::genPush(
            ctx.asmCode, 
            val
        );
    }
    ctx.takeGpReg();
}

std::string ExpressionCodeGenerator::formatLiteral(const ir::IRLiteralExpr* literalExpr) const {
    std::string val{ literalExpr->getValue() };
    if(literalExpr->getType() == Type::UNSIGNED){
        val.pop_back();
    }
    return std::format("${}", val);
}

void ExpressionCodeGenerator::generateFunctionCallExpr(const ir::IRFunctionCallExpr* callExpr, bool expectsReturnVal){
    // push arguments to stack
    generateArguments(callExpr);

    assembly::genCall(ctx.asmCode, callExpr->getCallName());

    // pop arguments from stack
    clearArguments(callExpr->getArgumentCount());

    if(expectsReturnVal){
        if(ctx.gpFreeRegPos < gpRegisters.size()){
            assembly::genMov(
                ctx.asmCode, 
                "%rax", 
                gpRegisters.at(ctx.gpFreeRegPos), 
                "q"
            );
        }
        else{
            assembly::genPush(ctx.asmCode, "%rax");
        }
        ctx.takeGpReg();
    }
}

void ExpressionCodeGenerator::generateArguments(const ir::IRFunctionCallExpr* callExpr){
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
            assembly::genPush(
                ctx.asmCode, 
                gpRegisters.at(ctx.gpFreeRegPos)
            );
        }
    }
}

void ExpressionCodeGenerator::clearArguments(size_t argCount){
    // popping arguments of the stack
    assembly::genOperation(
        ctx.asmCode, 
        "add", 
        std::format("${}", argCount * assembly::regSize), 
        "%rsp"
    );
}

void ExpressionCodeGenerator::generateTemporaryExprs(const ir::IRTemporaryExpr* tempExprs){
    for(size_t i{0}; i < tempExprs->getTemporaryExprs().size(); ++i){
        const auto* tempExpr{ tempExprs->getTemporaryExprAtN(i) };

        if(tempExpr->getNodeType() == ir::IRNodeType::CALL){
            for(const auto& temp : static_cast<const ir::IRFunctionCallExpr*>(tempExpr)->getTemporaryExprs()){
                if(temp != nullptr){
                    generateTemporaryExprs(temp.get());
                }
            }
        }

        ctx.variableMap.insert({
            tempExprs->getTemporaryNameAtN(i), 
            std::format("-{}(%rbp)", ctx.variableNum * assembly::regSize)
        });
        ++ctx.variableNum;

        generateExpr(tempExpr);
        ctx.freeGpReg();
        assembly::genMov(
            ctx.asmCode, 
            gpRegisters.at(ctx.gpFreeRegPos), 
            ctx.variableMap.at(tempExprs->getTemporaryNameAtN(i)), 
            "q"
        );
    }
}