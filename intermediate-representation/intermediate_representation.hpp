#ifndef INTERMEDIATE_REPRESENTATION_HPP
#define INTERMEDIATE_REPRESENTATION_HPP

#include "../common/intermediate-representation-tree/IRTree.hpp"
#include "../common/abstract-syntax-tree/ASTree.hpp"

class IntermediateRepresentation{

    public:

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // public entry point for forming intermediate representation
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        [[nodiscard]] std::unique_ptr<IRTree> formIR(ASTree* astRoot);

    private:

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // variable count - current function variable count
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        size_t variableCount;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // function
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::unique_ptr<IRTree> function(ASTree* node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // variables
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::unique_ptr<IRTree> parameter(ASTree* node);

        std::unique_ptr<IRTree> construct(ASTree* node);

        std::unique_ptr<IRTree> variable(ASTree* node);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // statements
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::unique_ptr<IRTree> statement(ASTree* node);

        std::unique_ptr<IRTree> ifStatement(ASTree* node);

        std::unique_ptr<IRTree> compoundStatement(ASTree* node);

        std::unique_ptr<IRTree> assignmentStatement(ASTree* node);

        std::unique_ptr<IRTree> returnStatement(ASTree* node);
        
        std::unique_ptr<IRTree> whileStatement(ASTree* node);

        std::unique_ptr<IRTree> forStatement(ASTree* node);

        std::unique_ptr<IRTree> doWhileStatement(ASTree* node);

        std::unique_ptr<IRTree> switchStatement(ASTree* node);

        std::unique_ptr<IRTree> _case(ASTree* node);

        std::unique_ptr<IRTree> _default(ASTree* node);

        std::unique_ptr<IRTree> _break();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // expressions
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::unique_ptr<IRTree> numericalExpression(ASTree* node);

        template<typename T>
        std::unique_ptr<IRTree> mergeLiterals(std::unique_ptr<IRTree>&& lchild, std::unique_ptr<IRTree>&& rchild, ASTree* node);

        template<typename T>
        T mergeValues(T l, T r, ASTree* node);

        std::unique_ptr<IRTree> relationalExpression(ASTree* node);

        std::unique_ptr<IRTree> id(ASTree* node);

        std::unique_ptr<IRTree> literal(ASTree* node);

        std::unique_ptr<IRTree> functionCall(ASTree* node);
    
        std::unique_ptr<IRTree> argument(ASTree* node);

};

#endif
