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
        const std::string& getName() const;
        const Kinds& getKind() const;
        const Types& getType() const;
        ASTree* getParameters() const;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // setters
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void setName(std::string_view _name);
        void setKind(Kinds _kind);
        void setType(Types _type);
        void setParameters(ASTree* par);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // display
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        const std::string symbolToString() const;

    private:

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // name - name of a symbol (variable name etc.)
        // kind - kind of a symbol (function, variable, parameter etc.)
        // type - type of a symbol
        // parameters - pointer to parameter node (only for functions)
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::string name;
        Kinds kind;
        Types type;
        ASTree* parameters;

};

#endif
