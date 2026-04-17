#ifndef IR_FUNCTION_HPP
#define IR_FUNCTION_HPP

#include <memory>
#include <vector>
#include <string>

#include "ir_node.hpp"
#include "ir_parameter.hpp"
#include "ir_stmt.hpp"
#include "../defs/types.hpp"
#include "../visitor/ir_visitor.hpp"

namespace ir {
    /**
     * @class IRFunction
     * @brief IRT representation for the function
    */
    class IRFunction final : public IRNode {
    public:
        /** 
         * @brief Creates the instance of the irt function
         * @param funcName - name of the function
         * @param type - return type of the function
        */
        IRFunction(const std::string& funcName, Type type);

        /** 
         * @brief getter for the parameters of the function
         * @returns reference to a const vector of pointers to parameters
        */
        const std::vector<std::unique_ptr<IRParameter>>& getParameters() const noexcept;

        /** 
         * @brief adds new parameter to the function
         * @param parameter - pointer to the parameter that is being added
        */
        void addParameter(std::unique_ptr<IRParameter> parameter);

        /** 
         * @brief getter for the body of the function
         * @returns reference to a const vector of pointers to statements of the body
        */
        const std::vector<std::unique_ptr<IRStmt>>& getBody() const noexcept;

        /** 
         * @brief adds new statement to the body
         * @param stmt - pointer to the statement that is being added
        */
        void addStatement(std::unique_ptr<IRStmt> stmt);

        /** 
         * @brief getter for the name of the function
         * @returns name of the function
        */
        const std::string& getFunctionName() const noexcept;

        /** 
         * @brief initializes function's name
         * @param funcName - name that is being assigned to the function
        */
        void setFunctionName(const std::string& funcName);

        /** 
         * @brief getter for the return type of the function
         * @returns return type of the function
        */
        Type getType() const noexcept;

        /** 
         * @brief initializes the return type of the function
         * @param returnType - return type of the function
        */
        void setType(Type returnType) noexcept;

        /** 
         * @brief getter for the amount of bytes function takes on the stack
         * @returns number of bytes that should be subtracted from the stack as string
        */
        const std::string& getRequiredMemory() const noexcept;

        /** 
         * @brief initializes the amount of bytes reserved for the stack
         * @param size - number of bytes reserved for the stack as string
        */
        void setRequiredMemory(const std::string& size);

        /**
         * @brief eliminates statements of the function's body that appear after the node that always returns
         * @param startIdx - index in the vector of statements where deletion starts
        */
        void eliminateDeadStmts(size_t startIdx);

        /** 
         * @brief sets predifined flag of a function
         * @param isPredefined - flag for predefined function
        */
        void setPredefined(bool isPredefined) noexcept;

        /** 
         * @brief checks if function is predifined
         * @returns true if function is predefined, false otherwise
        */
        bool isPredefined() const noexcept;

        /**
         * @brief accepts the ir visitor
         * @param visitor - reference to an ir visitor
        */
        void accept(IRVisitor& visitor) override;

    private:
        /// name of the function
        std::string functionName;

        /// memory required for the stack of the function
        std::string requiredMemory;

        /// return type of the function
        Type type;

        /// flag if function is predefined
        bool predefined;

        /// vector of pointers to parameters of the function
        std::vector<std::unique_ptr<IRParameter>> parameters;

        /// vector of pointers to statements of the body
        std::vector<std::unique_ptr<IRStmt>> body;
        
    };

}

#endif