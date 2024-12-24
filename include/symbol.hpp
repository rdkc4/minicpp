#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <iostream>

#include "defs.hpp"

class Symbol{
    public:
        Symbol(std::string& name, Kinds kind = Kinds::NO_KIND, Types type = Types::NO_TYPE, unsigned atr1 = 0, unsigned atr2 = 0);

        std::string getName() const;
        Kinds getKind() const;
        Types getType() const;
        unsigned getAtribute1() const;
        unsigned getAtribute2() const;

        void setName(const std::string& _name);
        void setKind(const Kinds& _kind);
        void setType(const Types& _type);
        void setAtribute1(const unsigned atr1);
        void setAtribute2(const unsigned atr2);

        std::string kindToString(Kinds kind);
        std::string typeToString(Types type);
        std::string symbolToString();

    private:
        std::string name;
        Kinds kind;
        Types type;
        unsigned atribute1;
        unsigned atribute2;

};

#endif