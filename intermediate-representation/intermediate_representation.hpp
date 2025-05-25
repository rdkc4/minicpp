#ifndef INTERMEDIATE_REPRESENTATION_HPP
#define INTERMEDIATE_REPRESENTATION_HPP

#include "../common/intermediate-representation-tree/IRTree.hpp"
#include "../common/abstract-syntax-tree/ASTree.hpp"

class IntermediateRepresentation{

    public:

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // public entry point for forming intermediate representation
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        [[nodiscard]] std::unique_ptr<IRTree> formIR(const ASTree* astRoot);

    private:

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // variable count - current function variable count
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        size_t variableCount;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // function
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::unique_ptr<IRTree> function(const ASTree* node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // variables
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::unique_ptr<IRTree> parameter(const ASTree* node) const;

        std::unique_ptr<IRTree> construct(const ASTree* node);

        std::unique_ptr<IRTree> variable(const ASTree* node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // statements
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::unique_ptr<IRTree> statement(const ASTree* node);

        std::unique_ptr<IRTree> ifStatement(const ASTree* node);

        std::unique_ptr<IRTree> compoundStatement(const ASTree* node);

        std::unique_ptr<IRTree> assignmentStatement(const ASTree* node) const;

        std::unique_ptr<IRTree> returnStatement(const ASTree* node) const;
        
        std::unique_ptr<IRTree> whileStatement(const ASTree* node);

        std::unique_ptr<IRTree> forStatement(const ASTree* node);

        std::unique_ptr<IRTree> doWhileStatement(const ASTree* node);

        std::unique_ptr<IRTree> switchStatement(const ASTree* node);

        std::unique_ptr<IRTree> _case(const ASTree* node);

        std::unique_ptr<IRTree> _default(const ASTree* node);

        std::unique_ptr<IRTree> _break() const;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // expressions
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::unique_ptr<IRTree> numericalExpression(const ASTree* node) const;

        template<typename T>
        std::unique_ptr<IRTree> mergeLiterals(std::unique_ptr<IRTree>&& lchild, std::unique_ptr<IRTree>&& rchild, const ASTree* node) const;

        template<typename T>
        T mergeValues(T l, T r, const ASTree* node) const;

        std::unique_ptr<IRTree> relationalExpression(const ASTree* node) const;

        std::unique_ptr<IRTree> id(const ASTree* node) const;

        std::unique_ptr<IRTree> literal(const ASTree* node) const;

        std::unique_ptr<IRTree> functionCall(const ASTree* node) const;
    
        std::unique_ptr<IRTree> argument(const ASTree* node) const;

};

#endif
