#ifndef PREPROCESSING_LIBS_HPP
#define PREPROCESSING_LIBS_HPP

#include <string>

/**
 * @namespace Preprocessing::Libs
 * @brief Module containing the definitions of included libraries
*/
namespace Preprocessing::Libs {
    /// relative path to standard library
    inline constexpr const char* relativeLibPath{ "./libmcpp/" };

    /// extension of the object file
    inline constexpr const char* objExt{ ".o" };

    /// extension of the source file
    inline constexpr const char* mcppExt{ ".mcpp" };

    /**
     * @brief generates the relative path to the source file
     * @param libName - name of the library
     * @returns relative path to the source of the library
    */
    std::string generateLibSourcePath(const std::string& libName);

    /**
     * @brief generates the relative path to the object file
     * @param libName - name of the library
     * @returns relative path to the object file of the library
    */
    std::string generateLibObjPath(const std::string& libName);

};

#endif