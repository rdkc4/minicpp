#include "../expression_code_generator.hpp"
#include "../function_code_generator.hpp"

#include "../../asm-generator/asm_instruction_generator.hpp"
#include "../../../common/intermediate-representation-tree/IRBinaryExpression.hpp"

ExpressionCodeGenerator::ExpressionCodeGenerator() = default;

// evaluating equations using general-purpose registers r(8-15) and stack (if necessary)
void ExpressionCodeGenerator::generateNumericalExpression(const IRExpression* _numexp){
    auto& codeGenContext = FunctionCodeGenerator::getContext();
    if(_numexp->getNodeType() == IRNodeType::ID){
        if(codeGenContext.gpFreeRegPos < gpRegisters.size()){
            AsmGenerator::Instruction::genMov(codeGenContext.asmCode, generateID(static_cast<const IRId*>(_numexp)), gpRegisters.at(codeGenContext.gpFreeRegPos), "q");
        }
        else{
            AsmGenerator::Instruction::genPush(codeGenContext.asmCode, generateID(static_cast<const IRId*>(_numexp)));
        }
        codeGenContext.takeGpReg();
    }
    else if(_numexp->getNodeType() == IRNodeType::LITERAL){
        if(codeGenContext.gpFreeRegPos < gpRegisters.size()){
            AsmGenerator::Instruction::genMov(codeGenContext.asmCode, generateLiteral(static_cast<const IRLiteral*>(_numexp)), gpRegisters.at(codeGenContext.gpFreeRegPos), "q");
        }
        else{
            AsmGenerator::Instruction::genPush(codeGenContext.asmCode, generateLiteral(static_cast<const IRLiteral*>(_numexp)));
        }
        codeGenContext.takeGpReg();
    }else if (_numexp->getNodeType() == IRNodeType::CALL){
        generateFunctionCall(static_cast<const IRFunctionCall*>(_numexp));
        if(codeGenContext.gpFreeRegPos < gpRegisters.size()){
            AsmGenerator::Instruction::genMov(codeGenContext.asmCode, "%rax", gpRegisters.at(codeGenContext.gpFreeRegPos), "q");
        }
        else{
            AsmGenerator::Instruction::genPush(codeGenContext.asmCode, "%rax");
        }
        codeGenContext.takeGpReg();
    }
    else{
        std::string lreg{};
        std::string rreg{};

        const auto _binExp = static_cast<const IRBinaryExpression*>(_numexp);
        
        generateNumericalExpression(_binExp->getLeftOperand());
        generateNumericalExpression(_binExp->getRightOperand());
        
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
        
        IRNodeType nodeType = _numexp->getNodeType();
        if(nodeType == IRNodeType::MUL || nodeType == IRNodeType::DIV){ // result of MUL || DIV is in RDX:RAX
            AsmGenerator::Instruction::genOperation(codeGenContext.asmCode, "xor", "%rdx", "%rdx"); //add overflow check (TODO)
            AsmGenerator::Instruction::genMov(codeGenContext.asmCode, rreg, "%rax", "q");
            if(_numexp->getType() == Types::INT){
                AsmGenerator::Instruction::genOperation(codeGenContext.asmCode, std::format("i{}", irNodeToString.at(nodeType)), lreg);
            }
            else{
                AsmGenerator::Instruction::genOperation(codeGenContext.asmCode, irNodeToString.at(nodeType), lreg);
            }
            AsmGenerator::Instruction::genMov(codeGenContext.asmCode, "%rax", rreg, "q");
        }
        else if(nodeType == IRNodeType::AND || nodeType == IRNodeType::OR || nodeType == IRNodeType::XOR){
            AsmGenerator::Instruction::genOperation(codeGenContext.asmCode, irNodeToString.at(nodeType), lreg, rreg);
        }
        else if(nodeType == IRNodeType::SHL || nodeType == IRNodeType::SAL || nodeType == IRNodeType::SHR || nodeType == IRNodeType::SAR){
            AsmGenerator::Instruction::genMov(codeGenContext.asmCode, lreg, "%rcx", "q");
            AsmGenerator::Instruction::genOperation(codeGenContext.asmCode, irNodeToString.at(nodeType), "%rcx", rreg);
        }
        else{
            AsmGenerator::Instruction::genOperation(codeGenContext.asmCode, irNodeToString.at(nodeType), lreg, rreg);
        }
        if(codeGenContext.gpFreeRegPos >= gpRegisters.size()){
            AsmGenerator::Instruction::genPush(codeGenContext.asmCode, rreg);
        }
        codeGenContext.takeGpReg();
    }
}

void ExpressionCodeGenerator::generateRelationalExpression(const IRExpression* _relexp){
    const auto _binExp = static_cast<const IRBinaryExpression*>(_relexp);
    generateNumericalExpression(_binExp->getLeftOperand());
    generateNumericalExpression(_binExp->getRightOperand());
    
    auto& codeGenContext = FunctionCodeGenerator::getContext();
    codeGenContext.freeGpReg();
    std::string lreg{ gpRegisters.at(codeGenContext.gpFreeRegPos) };
    codeGenContext.freeGpReg();
    std::string rreg{ gpRegisters.at(codeGenContext.gpFreeRegPos) };
    AsmGenerator::Instruction::genCmp(codeGenContext.asmCode, lreg, rreg);
}

const std::string& ExpressionCodeGenerator::generateID(const IRId* _id) const {
    return FunctionCodeGenerator::getContext().variableMap.at(_id->getIdName()); // returns address from variable map
}

std::string ExpressionCodeGenerator::generateLiteral(const IRLiteral* _literal) const {
    std::string val{ _literal->getValue() };
    if(_literal->getType() == Types::UNSIGNED){
        val.pop_back();
    }
    return std::format("${}", val);
}

void ExpressionCodeGenerator::generateFunctionCall(const IRFunctionCall* _functionCall){
    auto& codeGenContext = FunctionCodeGenerator::getContext();
    // push arguments to stack
    generateArgument(_functionCall);

    AsmGenerator::Instruction::genCall(codeGenContext.asmCode, _functionCall->getCallName());

    // pop arguments from stack
    clearArguments(_functionCall->getArgumentCount());
}

void ExpressionCodeGenerator::generateArgument(const IRFunctionCall* _functionCall){
    auto& codeGenContext = FunctionCodeGenerator::getContext();
    // evaluating temporaries
    for(const auto& temp : _functionCall->getTemporaries()){
        if(temp != nullptr){
            generateTemporaries(temp.get());
        }
    }
    // pushing arguments onto stack
    for(int i = _functionCall->getArgumentCount() - 1; i >= 0; --i){
        generateNumericalExpression(_functionCall->getArgumentAtN(static_cast<size_t>(i)));
        codeGenContext.freeGpReg();
        if(codeGenContext.gpFreeRegPos < gpRegisters.size()){ // if >= gpRegisters.size() argument is already pushed
            AsmGenerator::Instruction::genPush(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos));
        }
    }
}

void ExpressionCodeGenerator::clearArguments(size_t argCount){
    auto& codeGenContext = FunctionCodeGenerator::getContext();
    // popping arguments of the stack
    AsmGenerator::Instruction::genOperation(codeGenContext.asmCode, "add", std::format("${}", argCount * AsmGenerator::Instruction::regSize), "%rsp");
}

void ExpressionCodeGenerator::generateTemporaries(const IRTemporary* _temporary){
    for(size_t i = 0; i < _temporary->getTemporaries().size(); ++i){
        auto& codeGenContext = FunctionCodeGenerator::getContext();
        const auto temp = _temporary->getExpressionAtN(i);

        if(temp->getNodeType() == IRNodeType::CALL){
            for(const auto& _temp : static_cast<const IRFunctionCall*>(temp)->getTemporaries()){
                if(_temp != nullptr){
                    generateTemporaries(_temp.get());
                }
            }
        }

        codeGenContext.variableMap.insert({_temporary->getTemporaryNameAtN(i), std::format("-{}(%rbp)", codeGenContext.variableNum * AsmGenerator::Instruction::regSize)});
        ++codeGenContext.variableNum;

        generateNumericalExpression(temp);
        codeGenContext.freeGpReg();
        AsmGenerator::Instruction::genMov(codeGenContext.asmCode, gpRegisters.at(codeGenContext.gpFreeRegPos), 
            codeGenContext.variableMap.at(_temporary->getTemporaryNameAtN(i)), "q"
        );
    }
}