#ifndef IR_PARAMETER_HPP
#define IR_PARAMETER_HPP

#include <string>
#include <string_view>

#include "ir_node.hpp"
#include "../defs/types.hpp"
#include "../visitor/ir_visitor.hpp"

namespace ir {
    /** 
     * @class IRParameter
     * @brief IRT representation for the parameter node
    */
    class IRParameter final : public IRNode {
    public:
        /** 
         * @brief Creates the instance of the irt parameter
         * @param parName - name of the parameter
         * @param type - type of the parameter
        */
        IRParameter(std::string_view parName, Type type);

        /** 
         * @brief getter for the name of the parameter
         * @returns reference to a parameter's name
        */
        const std::string& getParameterName() const noexcept;

        /** 
         * @brief initializes the parameter's name
         * @param parName - the name of the parameter
        */
        void setParameterName(const std::string& parName);

        /** 
         * @brief getter for the type of the parameter
         * @returns type of the parameter
        */
        Type getType() const noexcept;

        /** 
         * @brief initializes the type of the parameter
         * @param parameterType - type of the parameter
        */
        void setType(Type parameterType) noexcept;

        /**
         * @brief accepts the ir visitor
         * @param visitor - reference to an ir visitor
        */
        void accept(IRVisitor& visitor) override;

    private:
        /// name of the parameter
        std::string parameterName;

        /// type of the parameter
        Type type;

    };

}

#endif