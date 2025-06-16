#ifndef INTERMEDIATE_REPRESENTATION_HPP
#define INTERMEDIATE_REPRESENTATION_HPP

#include <memory>

#include "../common/intermediate-representation-tree/IRTree.hpp"
#include "../common/abstract-syntax-tree/ASTree.hpp"
#include "defs/ir_defs.hpp"

/*
    Turning abstract syntax tree into intermediate representation tree
*/
class IntermediateRepresentation{

    public:

        IntermediateRepresentation();

        // simplifying ast for easir code generation
        // returns root of the intermediate representation tree
        [[nodiscard]] std::unique_ptr<IRTree> formIR(const ASTree* astRoot);

    private:
        // tracking variable count, temporaries count and names
        thread_local static IRThreadContext irContext;
        
        static constexpr size_t regSize{ 8 };

        // function
        std::unique_ptr<IRTree> function(const ASTree* node);
        std::unique_ptr<IRTree> parameter(const ASTree* node) const;

        // constructs
        std::unique_ptr<IRTree> construct(const ASTree* node);

        // variables
        std::unique_ptr<IRTree> variable(const ASTree* node);

        // statements
        std::unique_ptr<IRTree> statement(const ASTree* node);
        std::unique_ptr<IRTree> printfStatement(const ASTree* node);
        std::unique_ptr<IRTree> ifStatement(const ASTree* node);
        std::unique_ptr<IRTree> compoundStatement(const ASTree* node);
        std::unique_ptr<IRTree> assignmentStatement(const ASTree* node);
        std::unique_ptr<IRTree> returnStatement(const ASTree* node);
        std::unique_ptr<IRTree> whileStatement(const ASTree* node);
        std::unique_ptr<IRTree> forStatement(const ASTree* node);
        std::unique_ptr<IRTree> doWhileStatement(const ASTree* node);
        std::unique_ptr<IRTree> switchStatement(const ASTree* node);
        std::unique_ptr<IRTree> _case(const ASTree* node);
        std::unique_ptr<IRTree> _default(const ASTree* node);
        std::unique_ptr<IRTree> _break() const;

        // expressions
        std::unique_ptr<IRTree> numericalExpression(const ASTree* node);

        template<typename T>
        std::unique_ptr<IRTree> mergeLiterals(std::unique_ptr<IRTree>&& lchild, std::unique_ptr<IRTree>&& rchild, const ASTree* node) const;

        template<typename T>
        T mergeValues(T l, T r, const ASTree* node) const;

        std::unique_ptr<IRTree> relationalExpression(const ASTree* node);
        std::unique_ptr<IRTree> id(const ASTree* node) const;
        std::unique_ptr<IRTree> literal(const ASTree* node) const;
        std::unique_ptr<IRTree> functionCall(const ASTree* node);
        std::unique_ptr<IRTree> argument(const ASTree* node);

        // assigning function calls to temporary variables, preventing register corruption
        size_t countTemporaries(const ASTree* node) const;
        std::unique_ptr<IRTree> generateTemporaries();
        void assignFunctionCalls(IRTree* tmpVar, const ASTree* node, size_t& idx);
        std::unique_ptr<IRTree> replaceFunctionCall(const ASTree* node);
        std::unique_ptr<IRTree> initiateTemporaries(const ASTree* node);

};

#endif
