#include "../ASTFunction.hpp"

#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <format>

ASTFunction::ASTFunction(const Token& token, ASTNodeType ntype, Types type) : ASTNode(token, ntype), type{ type } {}

const std::vector<std::unique_ptr<ASTParameter>>& ASTFunction::getParameters() const noexcept {
    return parameters;
}

const std::vector<std::unique_ptr<ASTStatement>>& ASTFunction::getBody() const noexcept {
    return body;
}

void ASTFunction::addParameter(std::unique_ptr<ASTParameter> parameter){
    parameters.push_back(std::move(parameter));
}

void ASTFunction::addStatement(std::unique_ptr<ASTStatement> statement){
    body.push_back(std::move(statement));
}

Types ASTFunction::getType() const noexcept {
    return type;
}

size_t ASTFunction::getParameterCount() const noexcept {
    return parameters.size();
}

void ASTFunction::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());

    std::cout << std::format("{}|-> {}", std::string((offset + 1) * 2, ' '), "PARAMETERS\n");
    for(const auto& par : parameters){
        par->print(offset + 2);
    }

    std::cout << std::format("{}|-> {}", std::string((offset + 1) * 2, ' '), "BODY\n");
    for(const auto& st : body){
        st->print(offset + 2);
    }
}