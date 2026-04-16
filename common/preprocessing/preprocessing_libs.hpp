#ifndef PREPROCESSING_LIBS_HPP
#define PREPROCESSING_LIBS_HPP

#include <string_view>

/**
 * @namespace Preprocessing::Libs
 * @brief Module containing the definitions of included libraries
*/
namespace Preprocessing::Libs {
    /// relative path to standard library
    inline constexpr std::string_view relativeLibPath{ "./libmcpp/" };

    /// extension of the object file
    inline constexpr std::string_view objExt{ ".o" };

    /// extension of the source file
    inline constexpr std::string_view mcppExt{ ".mcpp" };

    /**
     * @brief generates the relative path to the source file
     * @param libName - name of the library
     * @returns relative path to the source of the library
    */
    std::string generateLibSourcePath(std::string_view libName);

    /**
     * @brief generates the relative path to the object file
     * @param libName - name of the library
     * @returns relative path to the object file of the library
    */
    std::string generateLibObjPath(std::string_view libName);

};

#endif