#include "preprocessor.hpp"

#include <cassert>
#include <cctype>
#include <fstream>
#include <sstream>

#include "../common/preprocessing/preprocessing_libs.hpp"

void preprocessing::Preprocessor::preprocess(const std::string& source) {
    size_t index{0};
    size_t len{ source.length() };

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

const std::vector<std::string>& 
preprocessing::Preprocessor::getPreprocessed() const noexcept {
    return preprocessed;
}

bool preprocessing::Preprocessor::hasErrors() const noexcept {
    return !preprocessErrors.empty();
}

std::string preprocessing::Preprocessor::getPreprocessErrors() const noexcept {
    if(preprocessErrors.empty()){
        return "";
    }

    std::stringstream errors{"Preprocessing failed:\n"};
    for(const auto& error : preprocessErrors){
        errors << error << "\n";
    }

    return errors.str();
}

void preprocessing::Preprocessor::handleDirective(const std::string& source, size_t& idx){
    size_t start{++idx};
    while(std::isalpha(source[idx])){
        ++idx;
    }

    if(start == idx){
        return;
    }

    std::string directive{ source.substr(start, idx - start) };
    if(directive == "include"){
        handleInclude(source, idx);
    }
}

void preprocessing::Preprocessor::handleInclude(const std::string& source, size_t& idx){
    if(source[idx] != ':'){
        return;
    }

    size_t start{ ++idx };
    while(std::isalnum(source[idx])){
        ++idx;
    }

    if(start == idx){
        return;
    }

    std::string includeLib{ source.substr(start, idx - start) };
    std::string includeLibPath{ preprocessing::generateLibSourcePath(includeLib) };

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
