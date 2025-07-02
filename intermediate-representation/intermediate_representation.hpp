#ifndef INTERMEDIATE_REPRESENTATION_HPP
#define INTERMEDIATE_REPRESENTATION_HPP

#include <memory>
#include <unordered_map>
#include <mutex>

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
        [[nodiscard]] std::unique_ptr<IRTree> formIR(const ASTree* _root);

    protected:
        // tracking variable count, temporaries count and names
        thread_local static IRThreadContext irContext;
        
        std::unordered_map<std::string,std::vector<std::string>> exceptions;
        std::mutex exceptionMtx;
        
        static constexpr size_t regSize{ 8 };

        void checkErrors(const ASTree* _functionList) const;

        // function
        std::unique_ptr<IRTree> function(const ASTree* _function);
        std::unique_ptr<IRTree> parameter(const ASTree* _parameters) const;

        // constructs
        std::unique_ptr<IRTree> construct(const ASTree* _construct);

        // variables
        std::unique_ptr<IRTree> variable(const ASTree* _variable);

        // statements
        std::unique_ptr<IRTree> statement(const ASTree* _statement);
        std::unique_ptr<IRTree> printfStatement(const ASTree* _printf);
        std::unique_ptr<IRTree> ifStatement(const ASTree* _if);
        std::unique_ptr<IRTree> compoundStatement(const ASTree* _compound);
        std::unique_ptr<IRTree> assignmentStatement(const ASTree* _assignment);
        std::unique_ptr<IRTree> returnStatement(const ASTree* _return);
        std::unique_ptr<IRTree> whileStatement(const ASTree* _while);
        std::unique_ptr<IRTree> forStatement(const ASTree* _for);
        std::unique_ptr<IRTree> doWhileStatement(const ASTree* _dowhile);
        std::unique_ptr<IRTree> switchStatement(const ASTree* _switch);
        std::unique_ptr<IRTree> _case(const ASTree* _astCase);
        std::unique_ptr<IRTree> _default(const ASTree* _astDefault);
        std::unique_ptr<IRTree> _break() const;

        // expressions
        std::unique_ptr<IRTree> numericalExpression(const ASTree* _numexp);

        template<typename T>
        std::unique_ptr<IRTree> mergeLiterals(std::unique_ptr<IRTree>&& leftOperand, std::unique_ptr<IRTree>&& rightOperand, const ASTree* _numexp) const;

        template<typename T>
        T mergeValues(T l, T r, const ASTree* _numexp) const;

        std::unique_ptr<IRTree> relationalExpression(const ASTree* _relexp);
        std::unique_ptr<IRTree> id(const ASTree* _id) const;
        std::unique_ptr<IRTree> literal(const ASTree* _literal) const;
        std::unique_ptr<IRTree> functionCall(const ASTree* _functionCall);
        std::unique_ptr<IRTree> argument(const ASTree* _functionCall);

        // assigning function calls to temporary variables, preventing register corruption
        size_t countTemporaries(const ASTree* _numexp) const;
        std::unique_ptr<IRTree> generateTemporaries();
        void assignFunctionCalls(IRTree* _temporaryRoot, const ASTree* _numexp, size_t& idx);
        std::unique_ptr<IRTree> replaceFunctionCall(const ASTree* _functionCall);
        std::unique_ptr<IRTree> initiateTemporaries(const ASTree* _numexp);

        bool eliminateDeadCode(IRTree* _construct);
        void countVariables(const IRTree* _construct);

};

#endif
