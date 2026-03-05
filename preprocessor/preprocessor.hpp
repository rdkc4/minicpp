#ifndef PREPROCESSOR_HPP
#define PREPROCESSOR_HPP

#include <cstddef>
#include <string>
#include <unordered_set>
#include <vector>

/** 
 * @class Preprocessor
 * @brief used for preprocessing source file
*/
class Preprocessor {
public:
    /** 
     * @brief Creates the instance of the preprocessor
    */
    Preprocessor();

    /** 
     * @brief entry point for preprocessing of the source file
     * @param source - reference to a source code
     * @returns void
    */
    void preprocess(const std::string& source);

    /** 
     * @brief getter for the vector of preprocessed files
     * @returns vector of preprocessed files containing the source code of each
    */
    const std::vector<std::string>& getPreprocessed() const noexcept;

    /** 
     * @brief flag if any error were caught in preprocessing phase
     * @returns false if no errors were found, true otherwise
    */
    bool hasErrors() const noexcept;

    /** 
     * @brief displays all preprocessing errors
     * @returns void
    */
    void showErrors() const;

private:
    /// vector of preprocessed files containing the source code of each
    std::vector<std::string> preprocessed;
    /// unordered_set of included libraries
    std::unordered_set<std::string> includedLibraries;
    /// errors of the preprocessing phase
    std::vector<std::string> preprocessErrors;

    /** 
     * @brief handles the directive
     * @param source - reference to a source code
     * @param idx - current index in the source code
     * @returns void
    */
    void handleDirective(const std::string& source, size_t& idx);
    
    /** 
     * @brief handles the include directive
     * @param source - reference to a source code
     * @param idx - current index in the source code
     * @returns void
    */
    void handleInclude(const std::string& source, size_t& idx);

};

#endif