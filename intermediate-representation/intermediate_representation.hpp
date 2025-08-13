#ifndef INTERMEDIATE_REPRESENTATION_HPP
#define INTERMEDIATE_REPRESENTATION_HPP

#include <memory>
#include <unordered_map>
#include <mutex>

#include "defs/ir_defs.hpp"
#include "../common/intermediate-representation-tree/IRProgram.hpp"
#include "../common/intermediate-representation-tree/IRFunction.hpp"
#include "../common/intermediate-representation-tree/IRStatement.hpp"
#include "../common/intermediate-representation-tree/IRVariable.hpp"
#include "../common/intermediate-representation-tree/IRPrintfSt.hpp"
#include "../common/intermediate-representation-tree/IRSwitchSt.hpp"
#include "../common/intermediate-representation-tree/IRForSt.hpp"
#include "../common/intermediate-representation-tree/IRWhileSt.hpp"
#include "../common/intermediate-representation-tree/IRDoWhileSt.hpp"
#include "../common/intermediate-representation-tree/IRCompoundSt.hpp"
#include "../common/intermediate-representation-tree/IRIfSt.hpp"
#include "../common/intermediate-representation-tree/IRAssignSt.hpp"
#include "../common/intermediate-representation-tree/IRReturnSt.hpp"
#include "../common/intermediate-representation-tree/IRExpression.hpp"
#include "../common/intermediate-representation-tree/IRBinaryExpression.hpp"
#include "../common/intermediate-representation-tree/IRFunctionCall.hpp"
#include "../common/intermediate-representation-tree/IRId.hpp"
#include "../common/intermediate-representation-tree/IRLiteral.hpp"
#include "../common/intermediate-representation-tree/IRTemporary.hpp"

#include "../common/abstract-syntax-tree/ASTProgram.hpp"
#include "../common/abstract-syntax-tree/ASTFunction.hpp"
#include "../common/abstract-syntax-tree/ASTStatement.hpp"
#include "../common/abstract-syntax-tree/ASTVariable.hpp"
#include "../common/abstract-syntax-tree/ASTPrintfSt.hpp"
#include "../common/abstract-syntax-tree/ASTCompoundSt.hpp"
#include "../common/abstract-syntax-tree/ASTAssignSt.hpp"
#include "../common/abstract-syntax-tree/ASTReturnSt.hpp"
#include "../common/abstract-syntax-tree/ASTIfSt.hpp"
#include "../common/abstract-syntax-tree/ASTWhileSt.hpp"
#include "../common/abstract-syntax-tree/ASTDoWhileSt.hpp"
#include "../common/abstract-syntax-tree/ASTForSt.hpp"
#include "../common/abstract-syntax-tree/ASTSwitchSt.hpp"
#include "../common/abstract-syntax-tree/ASTExpression.hpp"
#include "../common/abstract-syntax-tree/ASTFunctionCall.hpp"
#include "../common/abstract-syntax-tree/ASTLiteral.hpp"
#include "../common/abstract-syntax-tree/ASTId.hpp"

/*
    Turning abstract syntax tree into intermediate representation tree
*/
class IntermediateRepresentation{

    public:

        IntermediateRepresentation();

        // simplifying ast for easier code generation
        // returns root of the intermediate representation tree
        [[nodiscard]] std::unique_ptr<IRProgram> formIR(const ASTProgram* _program);

    protected:
        // tracking variable count, temporaries count and names
        thread_local static IRThreadContext irContext;
        
        std::unordered_map<std::string,std::vector<std::string>> exceptions;
        std::mutex exceptionMtx;
        
        static constexpr size_t regSize{ 8 };

        void checkErrors(const IRProgram* _program) const;

        // function
        std::unique_ptr<IRFunction> function(const ASTFunction* _function);
        void parameter(IRFunction* _irFunction, const std::vector<std::unique_ptr<ASTParameter>>& _parameters);
        void body(IRFunction* _irFunction, const std::vector<std::unique_ptr<ASTStatement>>& _body);

        // statements
        std::unique_ptr<IRStatement> statement(const ASTStatement* _statement);
        std::unique_ptr<IRVariable> variable(const ASTVariable* _variable);
        std::unique_ptr<IRPrintfSt> printfStatement(const ASTPrintfSt* _printf);
        std::unique_ptr<IRIfSt> ifStatement(const ASTIfSt* _if);
        std::unique_ptr<IRCompoundSt> compoundStatement(const ASTCompoundSt* _compound);
        std::unique_ptr<IRAssignSt> assignmentStatement(const ASTAssignSt* _assignment);
        std::unique_ptr<IRReturnSt> returnStatement(const ASTReturnSt* _return);
        std::unique_ptr<IRWhileSt> whileStatement(const ASTWhileSt* _while);
        std::unique_ptr<IRForSt> forStatement(const ASTForSt* _for);
        std::unique_ptr<IRDoWhileSt> doWhileStatement(const ASTDoWhileSt* _dowhile);
        std::unique_ptr<IRSwitchSt> switchStatement(const ASTSwitchSt* _switch);
        std::unique_ptr<IRCaseSt> _case(const ASTCaseSt* _astCase);
        std::unique_ptr<IRDefaultSt> _default(const ASTDefaultSt* _astDefault);
        std::unique_ptr<IRSwitchBlock> switchBlock(const ASTSwitchBlock* _block);

        // expressions
        std::unique_ptr<IRExpression> numericalExpression(const ASTExpression* _numexp);

        template<typename T>
        std::unique_ptr<IRExpression> mergeLiterals(const IRLiteral* leftOperand, const IRLiteral* rightOperand, Operators op) const;

        template<typename T>
        T mergeValues(T l, T r, Operators op) const;

        std::unique_ptr<IRBinaryExpression> relationalExpression(const ASTExpression* _relexp);
        std::unique_ptr<IRId> id(const ASTId* _id) const;
        std::unique_ptr<IRLiteral> literal(const ASTLiteral* _literal) const;
        std::unique_ptr<IRFunctionCall> functionCall(const ASTFunctionCall* _functionCall);
        void argument(IRFunctionCall* _irFunctionCall, const ASTFunctionCall* _functionCall);

        // assigning function calls to temporary variables, preventing register corruption
        size_t countTemporaries(const ASTExpression* _numexp) const;
        std::string generateTemporaries();
        void assignFunctionCalls(IRTemporary* _temporaryRoot, const ASTExpression* _numexp, size_t& idx);
        std::unique_ptr<IRId> replaceFunctionCall(const ASTFunctionCall* _functionCall);
        std::unique_ptr<IRTemporary> initiateTemporaries(const ASTExpression* _numexp);

        bool eliminateDeadCode(IRNode* _construct);
        void countVariables(const IRNode* _construct);

};

#endif
