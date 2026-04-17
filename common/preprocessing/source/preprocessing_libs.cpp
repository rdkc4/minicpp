#include "../preprocessing_libs.hpp"
#include <format>

std::string preprocessing::generateLibSourcePath(std::string_view libName) {
    return std::format("{}{}/{}{}", relativeLibPath, libName, libName, mcppExt);
}

std::string preprocessing::generateLibObjPath(std::string_view libName) {
    return std::format("{}{}/{}{}", relativeLibPath, libName, libName, objExt);
}