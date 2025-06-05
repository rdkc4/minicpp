#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>

#include "../abstract-syntax-tree/ASTree.hpp"
#include "../defs/defs.hpp"

class Symbol{
    public:

        Symbol(std::string_view name, Kinds kind = Kinds::NO_KIND, Types type = Types::NO_TYPE);

        // getters
        const std::string& getName() const noexcept;
        Kinds getKind() const noexcept;
        Types getType() const noexcept;
        ASTree* getParameters() const noexcept;

        // setters
        void setName(std::string_view _name);
        void setKind(Kinds _kind) noexcept;
        void setType(Types _type) noexcept;
        void setParameters(ASTree* par) noexcept;

        std::string symbolToString() const;

    private:
        // name of a symbol
        std::string name;
        // pointer to parameter node (only for functions)
        ASTree* parameters;
        // kind of a symbol
        Kinds kind;
        // type of a symbol
        Types type;

};

#endif
