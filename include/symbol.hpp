#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <iostream>
#include <vector>

#include "ASTree.hpp"
#include "defs.hpp"

class Symbol{
    public:
        Symbol(std::string& name, Kinds kind = Kinds::NO_KIND, Types type = Types::NO_TYPE);

        std::string getName() const;
        Kinds getKind() const;
        Types getType() const;
        std::shared_ptr<ASTree> getParameters() const;

        void setName(const std::string& _name);
        void setKind(const Kinds& _kind);
        void setType(const Types& _type);
        void setParameters(std::shared_ptr<ASTree> par);

        std::string symbolToString();

    private:
        std::string name;
        Kinds kind;
        Types type;
        std::shared_ptr<ASTree> parameters;

};

#endif