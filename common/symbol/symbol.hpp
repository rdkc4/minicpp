#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>

#include "../abstract-syntax-tree/ASTree.hpp"
#include "../defs/defs.hpp"

class Symbol{
    public:

        Symbol(std::string& name, Kinds kind = Kinds::NO_KIND, Types type = Types::NO_TYPE);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // getters
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::string getName() const;
        Kinds getKind() const;
        Types getType() const;
        std::shared_ptr<ASTree> getParameters() const;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // setters
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void setName(const std::string& _name);
        void setKind(const Kinds& _kind);
        void setType(const Types& _type);
        void setParameters(std::shared_ptr<ASTree> par);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // display
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::string symbolToString();

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
        std::shared_ptr<ASTree> parameters;

};

#endif
