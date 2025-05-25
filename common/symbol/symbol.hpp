#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>

#include "../abstract-syntax-tree/ASTree.hpp"
#include "../defs/defs.hpp"

class Symbol{
    public:

        Symbol(std::string_view name, Kinds kind = Kinds::NO_KIND, Types type = Types::NO_TYPE);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // getters
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        const std::string& getName() const noexcept;
        Kinds getKind() const noexcept;
        Types getType() const noexcept;
        ASTree* getParameters() const noexcept;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // setters
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void setName(std::string_view _name);
        void setKind(Kinds _kind) noexcept;
        void setType(Types _type) noexcept;
        void setParameters(ASTree* par) noexcept;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // display
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::string symbolToString() const;

    private:

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // name - name of a symbol (variable name etc.)
        // parameters - pointer to parameter node (only for functions)
        // kind - kind of a symbol (function, variable, parameter etc.)
        // type - type of a symbol
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::string name;
        ASTree* parameters;
        Kinds kind;
        Types type;

};

#endif
