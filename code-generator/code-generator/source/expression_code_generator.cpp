#include "../expression_code_generator.hpp"

#include <format>

#include "../../asm-generator/asm_instruction_generator.hpp"
#include "../../../common/intermediate-representation-tree/ir_binary_expr.hpp"

code_gen::ExpressionCodeGenerator::ExpressionCodeGenerator(
    code_gen::CodeGeneratorFunctionContext& context
) : ctx{ context } {}

void code_gen::ExpressionCodeGenerator::generateExpr(const ir::IRExpr* expr, code_gen::ExprContext exprCtx){
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

void code_gen::ExpressionCodeGenerator::generateBinaryExpr(
    const ir::IRBinaryExpr* binaryExpr, 
    code_gen::ExprContext exprCtx
){
    generateExpr(binaryExpr->getLeftOperandExpr());
    generateExpr(binaryExpr->getRightOperandExpr());

    auto operands{ getBinaryOperands() };
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

    if(exprCtx == code_gen::ExprContext::VALUE){
        if(ctx.gpFreeRegPos >= gpRegisters.size()){
            code_gen::assembly::genPush(ctx.asmCode, operands.rightOperand);
        }
        ctx.takeGpReg();
    }
}

std::string 
code_gen::ExpressionCodeGenerator::getUnaryOperand(std::string_view fallbackOperand){
    std::string operand{};

    ctx.freeGpReg();
    if(ctx.gpFreeRegPos >= gpRegisters.size()){
        operand = fallbackOperand;
        code_gen::assembly::genPop(ctx.asmCode, fallbackOperand);
    }
    else{
        operand = gpRegisters.at(ctx.gpFreeRegPos);
    }

    return operand;
}

code_gen::BinaryOperands code_gen::ExpressionCodeGenerator::getBinaryOperands(){
    return {
        .leftOperand = getUnaryOperand("%rdi"),
        .rightOperand = getUnaryOperand("%rsi")
    };
}

void code_gen::ExpressionCodeGenerator::generateMultiplicativeExpr(
    const ir::IRBinaryExpr* binaryExpr, 
    code_gen::BinaryOperands operands
){
    ir::IRNodeType nodeType{ binaryExpr->getNodeType() };

    code_gen::assembly::genOperation(
        ctx.asmCode, 
        "xor", "%rdx", "%rdx"
    );
    code_gen::assembly::genMov(
        ctx.asmCode, 
        operands.rightOperand, "%rax", "q"
    );

    if(binaryExpr->getType() == types::Type::INT){
        code_gen::assembly::genOperation(
            ctx.asmCode, 
            std::format("i{}", ir::irNodeToStr(nodeType)), 
            operands.leftOperand
        );
    }
    else{
        code_gen::assembly::genOperation(
            ctx.asmCode, 
            ir::irNodeToStr(nodeType), 
            operands.leftOperand
        );
    }

    code_gen::assembly::genMov(
        ctx.asmCode, 
        "%rax", operands.rightOperand, "q"
    );
}

void code_gen::ExpressionCodeGenerator::generateShiftExpr(
    const ir::IRBinaryExpr* binaryExpr, code_gen::BinaryOperands operands
){
    ir::IRNodeType nodeType{ binaryExpr->getNodeType() };

    code_gen::assembly::genMov(
        ctx.asmCode, 
        operands.leftOperand, 
        "%rcx", 
        "q"
    );
    code_gen::assembly::genOperation(
        ctx.asmCode, 
        ir::irNodeToStr(nodeType), 
        "%cl", 
        operands.rightOperand
    );
}

void code_gen::ExpressionCodeGenerator::generateLogicalAndExpr(code_gen::BinaryOperands operands){
    code_gen::assembly::genTest(
        ctx.asmCode, 
        operands.leftOperand
    );
    code_gen::assembly::genSet(
        ctx.asmCode, 
        "%al", "ne"
    );

    code_gen::assembly::genTest(
        ctx.asmCode, 
        operands.rightOperand
    );
    code_gen::assembly::genSet(
        ctx.asmCode, 
        "%cl", "ne"
    );

    code_gen::assembly::genOperation(
        ctx.asmCode, 
        "and", "%cl", "%al"
    );
    code_gen::assembly::genMov(
        ctx.asmCode,
        "%al", operands.rightOperand, "zx"
    );
}

void code_gen::ExpressionCodeGenerator::generateLogicalOrExpr(code_gen::BinaryOperands operands){
    code_gen::assembly::genMov(
        ctx.asmCode, 
        operands.rightOperand, "%rax"
    );

    code_gen::assembly::genOperation(
        ctx.asmCode, 
        "or", operands.leftOperand, "%rax"
    );

    code_gen::assembly::genSet(
        ctx.asmCode, 
        "%al", "ne"
    );

    code_gen::assembly::genMov(
        ctx.asmCode, 
        "%al", operands.rightOperand, "zx"
    );
}

void code_gen::ExpressionCodeGenerator::generateRelationalExpr(
    const ir::IRBinaryExpr* binaryExpr, 
    code_gen::BinaryOperands operands, 
    code_gen::ExprContext exprCtx
){
    code_gen::assembly::genCmp(
        ctx.asmCode, 
        operands.leftOperand, 
        operands.rightOperand
    );

    if(exprCtx == code_gen::ExprContext::VALUE){
        code_gen::assembly::genSetcc(
            ctx.asmCode,  
            irNodeTypeToJumpInfo(binaryExpr->getNodeType()).setcc,
            "%al"
        );

        code_gen::assembly::genMov(
            ctx.asmCode, 
            "%al", operands.rightOperand, "zx"
        );
    }
}

void code_gen::ExpressionCodeGenerator::generateConditionExpr(
    const ir::IRExpr* expr, 
    std::string_view trueLabel, 
    std::string_view falseLabel
){
    size_t labNum{ code_gen::assembly::getNextLabelNum() };
    
    ir::IRNodeType nodeType{ expr->getNodeType() };
    if(auto jumpInfo{ irNodeTypeToJumpInfo(nodeType) }; !jumpInfo.jcc.empty()){
        generateBinaryExpr(
            static_cast<const ir::IRBinaryExpr*>(expr), 
            code_gen::ExprContext::BRANCH
        );

        code_gen::assembly::genJcc(
            ctx.asmCode,
            jumpInfo.jcc,
            trueLabel
        );

        code_gen::assembly::genJcc(
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

        code_gen::assembly::genLabel(
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

        code_gen::assembly::genLabel(
            ctx.asmCode,
            midLabel
        );

        generateConditionExpr(binaryExpr->getRightOperandExpr(), trueLabel, falseLabel);
        return;
    }

    generateExpr(expr);
    std::string operand{ getUnaryOperand("%rdi") };

    code_gen::assembly::genTest(ctx.asmCode, operand);

    code_gen::assembly::genJcc(
        ctx.asmCode, 
        "jne", trueLabel
    );
    
    code_gen::assembly::genJcc(
        ctx.asmCode, 
        "je", falseLabel
    );
    
}

void code_gen::ExpressionCodeGenerator::generateALUExpr(
    const ir::IRBinaryExpr* binaryExpr, 
    code_gen::BinaryOperands operands
){
    ir::IRNodeType nodeType{ binaryExpr->getNodeType() };

    code_gen::assembly::genOperation(
        ctx.asmCode, 
        ir::irNodeToStr(nodeType), 
        operands.leftOperand, 
        operands.rightOperand
    );
}

void code_gen::ExpressionCodeGenerator::generateIdExpr(const ir::IRIdExpr* idExpr) const {
    if(ctx.gpFreeRegPos < gpRegisters.size()){
        code_gen::assembly::genMov(
            ctx.asmCode, 
            getIdExprAddress(idExpr), 
            gpRegisters.at(ctx.gpFreeRegPos), 
            "q"
        );
    }
    else{
        code_gen::assembly::genPush(
            ctx.asmCode, 
            getIdExprAddress(idExpr)
        );
    }
    ctx.takeGpReg();
}

std::string_view code_gen::ExpressionCodeGenerator::getIdExprAddress(const ir::IRIdExpr* idExpr) const {
    return ctx.variableMap.at(idExpr->getIdName());
}

void code_gen::ExpressionCodeGenerator::generateLiteralExpr(const ir::IRLiteralExpr* literalExpr){
    std::string val{ formatLiteral(literalExpr) };

    if(ctx.gpFreeRegPos < gpRegisters.size()){
        code_gen::assembly::genMov(
            ctx.asmCode, 
            val, 
            gpRegisters.at(ctx.gpFreeRegPos), 
            "q"
        );
    }
    else{
        code_gen::assembly::genPush(
            ctx.asmCode, 
            val
        );
    }
    ctx.takeGpReg();
}

std::string code_gen::ExpressionCodeGenerator::formatLiteral(
    const ir::IRLiteralExpr* literalExpr
) const {
    std::string val{ literalExpr->getValue() };
    if(literalExpr->getType() == types::Type::UNSIGNED){
        val.pop_back();
    }
    return std::format("${}", val);
}

void code_gen::ExpressionCodeGenerator::generateFunctionCallExpr(
    const ir::IRFunctionCallExpr* callExpr, 
    bool expectsReturnVal
){
    // push arguments to stack
    generateArguments(callExpr);

    code_gen::assembly::genCall(ctx.asmCode, callExpr->getCallName());

    // pop arguments from stack
    clearArguments(callExpr->getArgumentCount());

    if(expectsReturnVal){
        if(ctx.gpFreeRegPos < gpRegisters.size()){
            code_gen::assembly::genMov(
                ctx.asmCode, 
                "%rax", 
                gpRegisters.at(ctx.gpFreeRegPos), 
                "q"
            );
        }
        else{
            code_gen::assembly::genPush(ctx.asmCode, "%rax");
        }
        ctx.takeGpReg();
    }
}

void code_gen::ExpressionCodeGenerator::generateArguments(const ir::IRFunctionCallExpr* callExpr){
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
            code_gen::assembly::genPush(
                ctx.asmCode, 
                gpRegisters.at(ctx.gpFreeRegPos)
            );
        }
    }
}

void code_gen::ExpressionCodeGenerator::clearArguments(size_t argCount){
    // popping arguments of the stack
    code_gen::assembly::genOperation(
        ctx.asmCode, 
        "add", 
        std::format("${}", argCount * code_gen::assembly::regSize), 
        "%rsp"
    );
}

void code_gen::ExpressionCodeGenerator::generateTemporaryExprs(const ir::IRTemporaryExpr* tempExprs){
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
            std::format("-{}(%rbp)", ctx.variableNum * code_gen::assembly::regSize)
        });
        ++ctx.variableNum;

        generateExpr(tempExpr);
        ctx.freeGpReg();
        code_gen::assembly::genMov(
            ctx.asmCode, 
            gpRegisters.at(ctx.gpFreeRegPos), 
            ctx.variableMap.at(tempExprs->getTemporaryNameAtN(i)), 
            "q"
        );
    }
}