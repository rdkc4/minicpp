#include "preprocessor.hpp"

#include <cassert>
#include <cctype>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../common/preprocessing/preprocessing_libraries.hpp"

Preprocessor::Preprocessor() = default;

void Preprocessor::preprocess(const std::string& source) {
    size_t index = 0;
    size_t len = source.length();

    while(index < len){
        if(source[index] == '#'){
            handleDirective(source, index);
        }
        else {
            ++index;
        }
    }

    preprocessed.push_back(std::move(source));
}

const std::vector<std::string>& Preprocessor::getPreprocessed() const noexcept {
    return preprocessed;
}

bool Preprocessor::hasErrors() const noexcept {
    return !preprocessErrors.empty();
}

void Preprocessor::showErrors() const {
    std::string err{ "\nPreprocessing: failed\n" };
    for(const auto& preprocessErr : preprocessErrors){
        err += std::format("{}\n", preprocessErr);
    }
    std::cerr << err;
}

void Preprocessor::handleDirective(const std::string& source, size_t& idx){
    size_t start = ++idx;
    while(std::isalpha(source[idx])){
        ++idx;
    }

    if(start == idx){
        return;
    }

    std::string directive = std::string{ source.substr(start, idx - start) };
    if(directive == "include"){
        handleInclude(source, idx);
    }
}

void Preprocessor::handleInclude(const std::string& source, size_t& idx){
    if(source[idx] != ':'){
        return;
    }

    size_t start = ++idx;
    while(std::isalnum(source[idx])){
        ++idx;
    }

    if(start == idx){
        return;
    }

    std::string includeLib = std::string{ source.substr(start, idx - start) };
    std::string includeLibPath = Preprocessing::Libs::generateLibSourcePath(includeLib);

    if(!includedLibraries.contains(includeLib)){
        includedLibraries.insert(includeLib);
        std::ifstream inputFileStream(includeLibPath);

        if(!inputFileStream.is_open()){
            preprocessErrors.push_back(std::format("Failed to load library \"{}\"\n", includeLib));
            return;
        }

        std::stringstream input;
        input << inputFileStream.rdbuf();
        inputFileStream.close();

        preprocess(input.str());
    }

}
