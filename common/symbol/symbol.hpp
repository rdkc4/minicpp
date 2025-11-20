#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>
#include <vector>
#include <memory>

#include "../abstract-syntax-tree/ASTParameter.hpp"
#include "../defs/defs.hpp"

/** 
 * @class Symbol
 * @brief description of a symbol
*/
class Symbol{
public:
    /** 
     * @brief Creates a new instance of the symbol
     * @param name - name of the symbol
     * @param kind - kind of the symbol, default NO_KIND
     * @param type - type of the symbol, default NO_TYPE
    */
    Symbol(std::string_view name, Kinds kind = Kinds::NO_KIND, Types type = Types::NO_TYPE);

    /** 
     * @brief getter for the name of the symbol
     * @returns const reference to a name of the symbol
    */
    const std::string& getName() const noexcept;

    /** 
     * @brief getter for the kind of the symbol
     * @returns kind of the symbol
    */
    Kinds getKind() const noexcept;

    /** 
     * @brief getter for the type of the symbol
     * @returns type of the symbol
    */
    Types getType() const noexcept;

    /** 
     * @brief getter for the parameters (if symbol is a function)
     * @returns pointer to a vector of pointers to parameters
    */
    const std::vector<std::unique_ptr<ASTParameter>>* getParameters() const noexcept;

    /** 
     * @brief initializes symbol name
     * @param _name - name of the symbol
     * @returns void
    */
    void setName(std::string_view _name);

    /** 
     * @brief initializes symbol kind
     * @param _kind - kind of the symbol
     * @returns void
    */    
    void setKind(Kinds _kind) noexcept;

    /** 
     * @brief initializes symbol type
     * @param _type - type of the symbol
     * @returns void
    */
    void setType(Types _type) noexcept;

    /** 
     * @brief initializes symbol parameters (if the symbol is a function)
     * @param _parameters - pointer to a vector of pointers to the parameters of the symbol
     * @returns void
    */
    void setParameters(const std::vector<std::unique_ptr<ASTParameter>>* _parameters) noexcept;

    /** 
     * @brief formats the fields of the symbol
     * @note debugging purposes
     * @returns formatted symbol string
    */
    std::string symbolToString() const;

private:
    /// name of the symbol
    std::string name;
    /// pointer to parameter node (only for functions)
    const std::vector<std::unique_ptr<ASTParameter>>* parameters;
    /// kind of the symbol
    Kinds kind;
    /// type of the symbol
    Types type;

};

#endif
