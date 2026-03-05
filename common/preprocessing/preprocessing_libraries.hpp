#ifndef PREPROCESSING_LIBS_HPP
#define PREPROCESSING_LIBS_HPP

#include <string>

namespace Preprocessing {
    namespace Libs {
        inline constexpr const char* relativeLibPath = "./libmcpp/";
        inline constexpr const char* objExt = ".o";
        inline constexpr const char* mcppExt = ".mcpp";

        std::string generateLibSourcePath(const std::string& libName);

        std::string generateLibObjPath(const std::string& libName);
    }
}

#endif