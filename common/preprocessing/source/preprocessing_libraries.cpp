#include "../preprocessing_libraries.hpp"
#include <format>

std::string Preprocessing::Libs::generateLibSourcePath(const std::string& libName) {
    return std::format("{}{}/{}{}", relativeLibPath, libName, libName, mcppExt);
}

std::string Preprocessing::Libs::generateLibObjPath(const std::string& libName) {
    return std::format("{}{}/{}{}", relativeLibPath, libName, libName, objExt);
}