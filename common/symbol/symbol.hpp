#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>
#include <vector>
#include <memory>

#include "../abstract-syntax-tree/ast_parameter.hpp"
#include "../defs/types.hpp"
#include "../defs/kinds.hpp"

namespace semantic {
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
        Symbol(std::string_view name, Kind kind = Kind::NO_KIND, Type type = Type::NO_TYPE);

        /** 
         * @brief getter for the name of the symbol
         * @returns const reference to a name of the symbol
        */
        const std::string& getName() const noexcept;

        /** 
         * @brief getter for the kind of the symbol
         * @returns kind of the symbol
        */
        Kind getKind() const noexcept;

        /** 
         * @brief getter for the type of the symbol
         * @returns type of the symbol
        */
        Type getType() const noexcept;

        /** 
         * @brief getter for the parameters (if symbol is a function)
         * @returns pointer to a vector of pointers to parameters
        */
        const std::vector<std::unique_ptr<syntax::ast::ASTParameter>>* 
        getParameters() const noexcept;

        /** 
         * @brief initializes symbol name
         * @param symName - name of the symbol
        */
        void setName(std::string_view symName);

        /** 
         * @brief initializes symbol kind
         * @param symKind - kind of the symbol
        */    
        void setKind(Kind symKind) noexcept;

        /** 
         * @brief initializes symbol type
         * @param symType - type of the symbol
        */
        void setType(Type symType) noexcept;

        /** 
         * @brief initializes symbol parameters (if the symbol is a function)
         * @param symParams - pointer to a vector of pointers to the parameters of the symbol
        */
        void setParameters(const std::vector<std::unique_ptr<syntax::ast::ASTParameter>>* symParams) noexcept;

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
        const std::vector<std::unique_ptr<syntax::ast::ASTParameter>>* parameters;

        /// kind of the symbol
        Kind kind;
        
        /// type of the symbol
        Type type;

    };

}

#endif