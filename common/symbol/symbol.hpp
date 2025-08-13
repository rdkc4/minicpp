#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>
#include <vector>
#include <memory>

#include "../abstract-syntax-tree/ASTParameter.hpp"
#include "../defs/defs.hpp"

class Symbol{
    public:

        Symbol(std::string_view name, Kinds kind = Kinds::NO_KIND, Types type = Types::NO_TYPE);

        // getters
        const std::string& getName() const noexcept;
        Kinds getKind() const noexcept;
        Types getType() const noexcept;
        const std::vector<std::unique_ptr<ASTParameter>>* getParameters() const noexcept;

        // setters
        void setName(std::string_view _name);
        void setKind(Kinds _kind) noexcept;
        void setType(Types _type) noexcept;
        void setParameters(const std::vector<std::unique_ptr<ASTParameter>>* _parameters) noexcept;

        std::string symbolToString() const;

    private:
        // name of a symbol
        std::string name;
        // pointer to parameter node (only for functions)
        const std::vector<std::unique_ptr<ASTParameter>>* parameters;
        // kind of a symbol
        Kinds kind;
        // type of a symbol
        Types type;

};

#endif
