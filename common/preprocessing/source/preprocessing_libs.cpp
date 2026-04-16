#include "../preprocessing_libs.hpp"
#include <format>

std::string Preprocessing::Libs::generateLibSourcePath(std::string_view libName) {
    return std::format("{}{}/{}{}", relativeLibPath, libName, libName, mcppExt);
}

std::string Preprocessing::Libs::generateLibObjPath(std::string_view libName) {
    return std::format("{}{}/{}{}", relativeLibPath, libName, libName, objExt);
}